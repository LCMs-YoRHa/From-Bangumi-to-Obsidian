// src-tauri/src/utils/parallel.rs
// 并行处理工具 - 多线程批量任务执行

use crate::errors::{AppError, AppResult};
use crate::models::{ExportData, TaskProgress};
use crate::utils::{BangumiClient, MarkdownGenerator};
use std::path::PathBuf;
use std::sync::Arc;
use tauri::{AppHandle, Emitter};
use tokio::sync::{Semaphore, Mutex};

/// 并行任务配置
#[derive(Clone)]
pub struct ParallelConfig {
    /// 最大并发数
    pub max_concurrency: usize,
    /// 每个请求之间的最小间隔（毫秒）
    pub rate_limit_ms: u64,
    /// 失败重试次数
    pub retry_count: u32,
}

impl Default for ParallelConfig {
    fn default() -> Self {
        Self {
            max_concurrency: 4,      // 默认 4 个并发
            rate_limit_ms: 100,      // 100ms 间隔
            retry_count: 2,          // 重试 2 次
        }
    }
}

/// 并行处理结果
pub struct ParallelResult {
    pub success_count: u32,
    pub failed_count: u32,
    pub failed_ids: Vec<u64>,
}

/// 并行获取多个条目的完整数据
pub async fn fetch_subjects_parallel(
    client: &BangumiClient,
    subject_ids: &[u64],
    config: &ParallelConfig,
    app: Option<&AppHandle>,
) -> Vec<(u64, Result<ExportData, AppError>)> {
    let semaphore = Arc::new(Semaphore::new(config.max_concurrency));
    let client = Arc::new(client.clone());
    let results = Arc::new(Mutex::new(Vec::new()));
    let progress_counter = Arc::new(Mutex::new(0u32));
    let total = subject_ids.len() as u32;

    let mut handles = Vec::new();

    for &subject_id in subject_ids {
        let permit = semaphore.clone().acquire_owned().await.unwrap();
        let client = client.clone();
        let results = results.clone();
        let progress_counter = progress_counter.clone();
        let retry_count = config.retry_count;
        let rate_limit_ms = config.rate_limit_ms;
        let app_handle = app.cloned();

        let handle = tokio::spawn(async move {
            let result = fetch_single_subject_with_retry(&client, subject_id, retry_count).await;
            
            // 更新进度
            {
                let mut counter = progress_counter.lock().await;
                *counter += 1;
                let current = *counter;
                
                if let Some(app) = &app_handle {
                    let status = match &result {
                        Ok(data) => {
                            let name = data.name_cn.as_ref()
                                .filter(|n| !n.is_empty())
                                .unwrap_or(&data.name);
                            format!("已获取: {}", name)
                        }
                        Err(e) => format!("获取条目 {} 失败: {}", subject_id, e),
                    };
                    let progress = TaskProgress::new(current, total, status);
                    let _ = app.emit("fetch-progress", &progress);
                }
            }

            // 存储结果
            {
                let mut res = results.lock().await;
                res.push((subject_id, result));
            }

            // 速率限制
            tokio::time::sleep(tokio::time::Duration::from_millis(rate_limit_ms)).await;
            
            drop(permit);
        });

        handles.push(handle);
    }

    // 等待所有任务完成
    for handle in handles {
        let _ = handle.await;
    }

    // 所有任务完成后，Arc 应该是唯一的引用
    Arc::try_unwrap(results)
        .expect("所有任务完成后，Arc 应该是唯一的引用")
        .into_inner()
}

/// 带重试的单个条目获取
async fn fetch_single_subject_with_retry(
    client: &BangumiClient,
    subject_id: u64,
    retry_count: u32,
) -> AppResult<ExportData> {
    let mut last_error = None;
    
    for attempt in 0..=retry_count {
        if attempt > 0 {
            // 指数退避
            let delay = 500 * (2_u64.pow(attempt - 1));
            tokio::time::sleep(tokio::time::Duration::from_millis(delay)).await;
        }

        match fetch_single_subject(client, subject_id).await {
            Ok(data) => return Ok(data),
            Err(e) => {
                last_error = Some(e);
                continue;
            }
        }
    }

    Err(last_error.unwrap_or_else(|| AppError::ApiError {
        status: 500,
        message: "未知错误".to_string(),
    }))
}

/// 获取单个条目的完整数据（并行请求所有相关 API）
async fn fetch_single_subject(
    client: &BangumiClient,
    subject_id: u64,
) -> AppResult<ExportData> {
    // 使用 tokio::try_join! 并行请求所有数据
    let (subject, user_collection, persons, characters, relations) = tokio::try_join!(
        client.get_subject(subject_id),
        client.get_user_collection(subject_id),
        client.get_subject_persons(subject_id),
        client.get_subject_characters(subject_id),
        client.get_subject_relations(subject_id),
    )?;

    Ok(MarkdownGenerator::build_export_data(
        &subject,
        &user_collection,
        &persons,
        &characters,
        &relations,
    ))
}

/// 并行写入 Markdown 文件
pub async fn write_markdown_parallel(
    data_list: &[ExportData],
    output_path: &PathBuf,
    config: &ParallelConfig,
    app: Option<&AppHandle>,
) -> ParallelResult {
    let semaphore = Arc::new(Semaphore::new(config.max_concurrency * 2)); // 写入可以更多并发
    let success_count = Arc::new(Mutex::new(0u32));
    let failed_ids = Arc::new(Mutex::new(Vec::new()));
    let progress_counter = Arc::new(Mutex::new(0u32));
    let total = data_list.len() as u32;

    let mut handles = Vec::new();

    for data in data_list {
        let permit = semaphore.clone().acquire_owned().await.unwrap();
        let output_path = output_path.clone();
        let data = data.clone();
        let success_count = success_count.clone();
        let _failed_ids = failed_ids.clone();
        let progress_counter = progress_counter.clone();
        let app_handle = app.cloned();

        let handle = tokio::spawn(async move {
            // 使用 spawn_blocking 进行文件 IO
            let write_result = tokio::task::spawn_blocking(move || {
                MarkdownGenerator::write_markdown_file(&output_path, &data)
            })
            .await;

            // 更新进度
            {
                let mut counter = progress_counter.lock().await;
                *counter += 1;
                let current = *counter;

                let (is_success, status) = match write_result {
                    Ok(Ok(filename)) => (true, format!("已写入: {}", filename)),
                    Ok(Err(e)) => (false, format!("写入失败: {}", e)),
                    Err(e) => (false, format!("任务失败: {}", e)),
                };

                if is_success {
                    let mut count = success_count.lock().await;
                    *count += 1;
                }

                if let Some(app) = &app_handle {
                    let progress = TaskProgress::new(current, total, status);
                    let _ = app.emit("write-progress", &progress);
                }
            }

            drop(permit);
        });

        handles.push(handle);
    }

    // 等待所有任务完成
    for handle in handles {
        let _ = handle.await;
    }

    let success = *success_count.lock().await;
    let failed = failed_ids.lock().await.clone();

    ParallelResult {
        success_count: success,
        failed_count: total - success,
        failed_ids: failed,
    }
}

/// 并行更新隐私设置
pub async fn update_privacy_parallel(
    client: &BangumiClient,
    subject_ids: &[u64],
    is_private: bool,
    config: &ParallelConfig,
    app: Option<&AppHandle>,
) -> ParallelResult {
    let semaphore = Arc::new(Semaphore::new(config.max_concurrency));
    let client = Arc::new(client.clone());
    let success_count = Arc::new(Mutex::new(0u32));
    let failed_ids = Arc::new(Mutex::new(Vec::new()));
    let progress_counter = Arc::new(Mutex::new(0u32));
    let total = subject_ids.len() as u32;

    let mut handles = Vec::new();

    for &subject_id in subject_ids {
        let permit = semaphore.clone().acquire_owned().await.unwrap();
        let client = client.clone();
        let success_count = success_count.clone();
        let failed_ids = failed_ids.clone();
        let progress_counter = progress_counter.clone();
        let rate_limit_ms = config.rate_limit_ms;
        let app_handle = app.cloned();

        let handle = tokio::spawn(async move {
            let result = client.update_collection_privacy(subject_id, is_private).await;

            // 更新进度
            {
                let mut counter = progress_counter.lock().await;
                *counter += 1;
                let current = *counter;

                let status = match &result {
                    Ok(_) => {
                        let mut count = success_count.lock().await;
                        *count += 1;
                        format!("条目 {} 隐私设置已更新", subject_id)
                    }
                    Err(e) => {
                        let mut ids = failed_ids.lock().await;
                        ids.push(subject_id);
                        format!("条目 {} 更新失败: {}", subject_id, e)
                    }
                };

                if let Some(app) = &app_handle {
                    let progress = TaskProgress::new(current, total, status);
                    let _ = app.emit("privacy-update-progress", &progress);
                }
            }

            // 速率限制
            tokio::time::sleep(tokio::time::Duration::from_millis(rate_limit_ms)).await;

            drop(permit);
        });

        handles.push(handle);
    }

    // 等待所有任务完成
    for handle in handles {
        let _ = handle.await;
    }

    let success = *success_count.lock().await;
    let failed = failed_ids.lock().await.clone();

    ParallelResult {
        success_count: success,
        failed_count: failed.len() as u32,
        failed_ids: failed,
    }
}
