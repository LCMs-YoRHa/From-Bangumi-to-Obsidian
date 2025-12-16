// src-tauri/src/commands/collection.rs
// 收藏管理相关 Tauri Commands

use crate::errors::{AppError, AppResult};
use crate::models::{
    BatchImportParams, CollectionListResponse, Credentials, TaskProgress, UserCollection,
};
use crate::utils::{BangumiClient, MarkdownGenerator, ParallelConfig, fetch_subjects_parallel, write_markdown_parallel, update_privacy_parallel};
use std::path::PathBuf;
use tauri::{AppHandle, Emitter, Manager};

/// 获取用户收藏列表
#[tauri::command]
pub async fn get_user_collections(
    user_id: String,
    token: String,
    subject_type: u8,
    collection_type: Option<u8>,
    limit: u32,
    offset: u32,
) -> AppResult<CollectionListResponse> {
    let credentials = Credentials::new(user_id, token);
    let client = BangumiClient::new(credentials)?;
    client
        .get_user_collections(subject_type, collection_type, limit, offset)
        .await
}

/// 获取用户单个条目收藏信息
#[tauri::command]
pub async fn get_user_collection(
    user_id: String,
    token: String,
    subject_id: u64,
) -> AppResult<UserCollection> {
    let credentials = Credentials::new(user_id, token);
    let client = BangumiClient::new(credentials)?;
    client.get_user_collection(subject_id).await
}

/// 获取所有收藏的条目 ID
#[tauri::command]
pub async fn fetch_all_collection_ids(
    user_id: String,
    token: String,
    subject_type: u8,
    collection_type: Option<u8>,
    limit: u32,
) -> AppResult<Vec<u64>> {
    let credentials = Credentials::new(user_id, token);
    let client = BangumiClient::new(credentials)?;
    client
        .fetch_all_collection_ids(subject_type, collection_type, limit)
        .await
}

/// 更新收藏隐私设置
#[tauri::command]
pub async fn update_collection_privacy(
    user_id: String,
    token: String,
    subject_id: u64,
    is_private: bool,
) -> AppResult<()> {
    let credentials = Credentials::new(user_id, token);
    let client = BangumiClient::new(credentials)?;
    client.update_collection_privacy(subject_id, is_private).await
}

/// 批量更新收藏隐私设置
/// 
/// 该命令会发送进度事件到前端
#[tauri::command]
pub async fn batch_update_privacy(
    app: AppHandle,
    user_id: String,
    token: String,
    subject_ids: Vec<u64>,
    is_private: bool,
) -> AppResult<TaskProgress> {
    let credentials = Credentials::new(user_id, token);
    let client = BangumiClient::new(credentials)?;

    let total = subject_ids.len() as u32;
    let mut success_count = 0u32;

    for (i, subject_id) in subject_ids.iter().enumerate() {
        let current = (i + 1) as u32;

        match client
            .update_collection_privacy(*subject_id, is_private)
            .await
        {
            Ok(_) => {
                success_count += 1;
                let progress = TaskProgress::new(
                    current,
                    total,
                    format!("✅ 条目 {} 隐私设置更新成功", subject_id),
                );
                let _ = app.emit("privacy-update-progress", &progress);
            }
            Err(e) => {
                let progress = TaskProgress::new(
                    current,
                    total,
                    format!("❌ 条目 {} 更新失败: {}", subject_id, e),
                );
                let _ = app.emit("privacy-update-progress", &progress);
            }
        }

        // 添加小延迟以避免 API 限流
        tokio::time::sleep(tokio::time::Duration::from_millis(100)).await;
    }

    Ok(TaskProgress::completed(format!(
        "隐私设置更新完成！成功更新 {}/{} 个条目",
        success_count, total
    )))
}

/// 导出单个条目为 Markdown
#[tauri::command]
pub async fn export_subject_to_markdown(
    app: AppHandle,
    user_id: String,
    token: String,
    subject_id: u64,
    output_dir: Option<String>,
) -> AppResult<String> {
    let credentials = Credentials::new(user_id.clone(), token.clone());
    let client = BangumiClient::new(credentials)?;

    // 获取所有需要的数据
    let (subject, user_collection, persons, characters, relations) = tokio::try_join!(
        client.get_subject(subject_id),
        client.get_user_collection(subject_id),
        client.get_subject_persons(subject_id),
        client.get_subject_characters(subject_id),
        client.get_subject_relations(subject_id),
    )?;

    // 构建导出数据
    let export_data =
        MarkdownGenerator::build_export_data(&subject, &user_collection, &persons, &characters, &relations);

    // 确定输出目录
    let output_path = if let Some(dir) = output_dir {
        PathBuf::from(dir)
    } else {
        // 使用应用数据目录
        let app_data_dir = app
            .path()
            .app_data_dir()
            .map_err(|_| AppError::IoError(std::io::Error::new(
                std::io::ErrorKind::NotFound,
                "无法获取应用数据目录",
            )))?;
        app_data_dir.join("output")
    };

    // 写入文件
    let filename = MarkdownGenerator::write_markdown_file(&output_path, &export_data)?;

    Ok(format!(
        "文件已保存: {}",
        output_path.join(&filename).display()
    ))
}

/// 批量导出条目为 Markdown
/// 
/// 该命令会发送进度事件到前端
#[tauri::command]
pub async fn batch_export_to_markdown(
    app: AppHandle,
    user_id: String,
    token: String,
    params: BatchImportParams,
    output_dir: Option<String>,
) -> AppResult<TaskProgress> {
    let credentials = Credentials::new(user_id.clone(), token.clone());
    let client = BangumiClient::new(credentials)?;

    // 第一步：获取所有条目 ID
    let _ = app.emit(
        "export-progress",
        TaskProgress::new(0, 100, "正在获取收藏列表..."),
    );

    let subject_ids = client
        .fetch_all_collection_ids(params.subject_type, params.collection_type, params.limit)
        .await?;

    if subject_ids.is_empty() {
        return Ok(TaskProgress::completed("没有找到任何收藏条目"));
    }

    let _ = app.emit(
        "export-progress",
        TaskProgress::new(0, subject_ids.len() as u32, format!("找到 {} 个条目", subject_ids.len())),
    );

    // 确定输出目录
    let output_path = if let Some(dir) = output_dir {
        PathBuf::from(dir)
    } else {
        let app_data_dir = app
            .path()
            .app_data_dir()
            .map_err(|_| AppError::IoError(std::io::Error::new(
                std::io::ErrorKind::NotFound,
                "无法获取应用数据目录",
            )))?;
        app_data_dir.join("output")
    };

    let total = subject_ids.len() as u32;
    let mut success_count = 0u32;

    // 第二步：逐个处理条目
    for (i, subject_id) in subject_ids.iter().enumerate() {
        let current = (i + 1) as u32;

        // 获取条目数据
        match async {
            let (subject, user_collection, persons, characters, relations) = tokio::try_join!(
                client.get_subject(*subject_id),
                client.get_user_collection(*subject_id),
                client.get_subject_persons(*subject_id),
                client.get_subject_characters(*subject_id),
                client.get_subject_relations(*subject_id),
            )?;

            let export_data = MarkdownGenerator::build_export_data(
                &subject,
                &user_collection,
                &persons,
                &characters,
                &relations,
            );

            let display_name = export_data
                .name_cn
                .as_ref()
                .filter(|n| !n.is_empty())
                .unwrap_or(&export_data.name)
                .clone();

            MarkdownGenerator::write_markdown_file(&output_path, &export_data)?;

            Ok::<String, AppError>(display_name)
        }
        .await
        {
            Ok(name) => {
                success_count += 1;
                let progress = TaskProgress::new(
                    current,
                    total,
                    format!("✅ 正在写入第 {} 篇: {}", current, name),
                );
                let _ = app.emit("export-progress", &progress);
            }
            Err(e) => {
                let progress = TaskProgress::new(
                    current,
                    total,
                    format!("条目 {} 处理失败: {}", subject_id, e),
                );
                let _ = app.emit("export-progress", &progress);
            }
        }

        // 添加延迟以避免 API 限流
        tokio::time::sleep(tokio::time::Duration::from_millis(200)).await;
    }

    Ok(TaskProgress::completed(format!(
        "导出完成！成功导出 {}/{} 个条目到 {}",
        success_count,
        total,
        output_path.display()
    )))
}

/// 并行批量导出条目为 Markdown（多线程优化版本）
/// 
/// 使用多线程并行处理，显著提升批量导出速度
#[tauri::command]
pub async fn batch_export_parallel(
    app: AppHandle,
    user_id: String,
    token: String,
    params: BatchImportParams,
    output_dir: Option<String>,
    max_concurrency: Option<usize>,
) -> AppResult<TaskProgress> {
    let credentials = Credentials::new(user_id.clone(), token.clone());
    let client = BangumiClient::new(credentials)?;

    // 配置并行参数
    let config = ParallelConfig {
        max_concurrency: max_concurrency.unwrap_or(4),
        rate_limit_ms: 100,
        retry_count: 2,
    };

    // 第一步：获取所有条目 ID
    let _ = app.emit(
        "export-progress",
        TaskProgress::new(0, 100, "正在获取收藏列表..."),
    );

    let subject_ids = client
        .fetch_all_collection_ids(params.subject_type, params.collection_type, params.limit)
        .await?;

    if subject_ids.is_empty() {
        return Ok(TaskProgress::completed("没有找到任何收藏条目"));
    }

    let total = subject_ids.len();
    let _ = app.emit(
        "export-progress",
        TaskProgress::new(0, total as u32, format!("找到 {} 个条目，开始并行获取数据...", total)),
    );

    // 第二步：并行获取所有条目数据
    let results = fetch_subjects_parallel(&client, &subject_ids, &config, Some(&app)).await;

    // 过滤成功的结果
    let successful_data: Vec<_> = results
        .into_iter()
        .filter_map(|(_, result)| result.ok())
        .collect();

    let fetch_success = successful_data.len();
    let _ = app.emit(
        "export-progress",
        TaskProgress::new(
            total as u32,
            total as u32,
            format!("数据获取完成，成功 {}/{}，开始写入文件...", fetch_success, total),
        ),
    );

    // 确定输出目录
    let output_path = if let Some(dir) = output_dir {
        PathBuf::from(dir)
    } else {
        let app_data_dir = app
            .path()
            .app_data_dir()
            .map_err(|_| AppError::IoError(std::io::Error::new(
                std::io::ErrorKind::NotFound,
                "无法获取应用数据目录",
            )))?;
        app_data_dir.join("output")
    };

    // 第三步：并行写入文件
    let write_result = write_markdown_parallel(&successful_data, &output_path, &config, Some(&app)).await;

    Ok(TaskProgress::completed(format!(
        "导出完成！成功导出 {}/{} 个条目到 {}",
        write_result.success_count,
        total,
        output_path.display()
    )))
}

/// 并行批量更新隐私设置（多线程优化版本）
#[tauri::command]
pub async fn batch_update_privacy_parallel(
    app: AppHandle,
    user_id: String,
    token: String,
    subject_ids: Vec<u64>,
    is_private: bool,
    max_concurrency: Option<usize>,
) -> AppResult<TaskProgress> {
    let credentials = Credentials::new(user_id, token);
    let client = BangumiClient::new(credentials)?;

    let config = ParallelConfig {
        max_concurrency: max_concurrency.unwrap_or(4),
        rate_limit_ms: 100,
        retry_count: 2,
    };

    let total = subject_ids.len() as u32;
    let _ = app.emit(
        "privacy-update-progress",
        TaskProgress::new(0, total, format!("开始并行更新 {} 个条目的隐私设置...", total)),
    );

    let result = update_privacy_parallel(&client, &subject_ids, is_private, &config, Some(&app)).await;

    Ok(TaskProgress::completed(format!(
        "隐私设置更新完成！成功 {}/{}，失败 {}",
        result.success_count,
        total,
        result.failed_count
    )))
}
