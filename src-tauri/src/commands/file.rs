// src-tauri/src/commands/file.rs
// 文件操作相关 Tauri Commands

use crate::errors::{AppError, AppResult};
use crate::models::Credentials;
use std::fs;
use std::path::PathBuf;
use tauri::{AppHandle, Manager};

/// 读取认证文件
#[tauri::command]
pub async fn read_credentials_file(file_path: String) -> AppResult<Credentials> {
    let content = fs::read_to_string(&file_path).map_err(AppError::IoError)?;
    
    Credentials::try_from(content.as_str())
        .map_err(|e| AppError::InvalidParameter(e))
}

/// 保存认证信息到文件
#[tauri::command]
pub async fn save_credentials_file(
    file_path: String,
    user_id: String,
    token: String,
) -> AppResult<()> {
    let content = format!("{}\n{}", user_id, token);
    fs::write(&file_path, content).map_err(AppError::IoError)?;
    Ok(())
}

/// 读取条目 ID 列表文件
#[tauri::command]
pub async fn read_subject_ids_file(file_path: String) -> AppResult<Vec<u64>> {
    let content = fs::read_to_string(&file_path).map_err(AppError::IoError)?;
    
    let ids: Result<Vec<u64>, _> = content
        .split(',')
        .map(|s| s.trim())
        .filter(|s| !s.is_empty())
        .map(|s| s.parse::<u64>())
        .collect();

    ids.map_err(|e| AppError::InvalidParameter(format!("解析条目 ID 失败: {}", e)))
}

/// 保存条目 ID 列表到文件
#[tauri::command]
pub async fn save_subject_ids_file(file_path: String, subject_ids: Vec<u64>) -> AppResult<()> {
    let content = subject_ids
        .iter()
        .map(|id| id.to_string())
        .collect::<Vec<_>>()
        .join(",");

    fs::write(&file_path, content).map_err(AppError::IoError)?;
    Ok(())
}

/// 获取应用数据目录
#[tauri::command]
pub async fn get_app_data_dir(app: AppHandle) -> AppResult<String> {
    let app_data_dir = app
        .path()
        .app_data_dir()
        .map_err(|_| AppError::IoError(std::io::Error::new(
            std::io::ErrorKind::NotFound,
            "无法获取应用数据目录",
        )))?;

    // 确保目录存在
    fs::create_dir_all(&app_data_dir).map_err(AppError::IoError)?;

    Ok(app_data_dir.to_string_lossy().to_string())
}

/// 获取输出目录
#[tauri::command]
pub async fn get_output_dir(app: AppHandle) -> AppResult<String> {
    let app_data_dir = app
        .path()
        .app_data_dir()
        .map_err(|_| AppError::IoError(std::io::Error::new(
            std::io::ErrorKind::NotFound,
            "无法获取应用数据目录",
        )))?;

    let output_dir = app_data_dir.join("output");

    // 确保目录存在
    fs::create_dir_all(&output_dir).map_err(AppError::IoError)?;

    Ok(output_dir.to_string_lossy().to_string())
}

/// 列出输出目录中的文件
#[tauri::command]
pub async fn list_output_files(app: AppHandle) -> AppResult<Vec<FileInfo>> {
    let app_data_dir = app
        .path()
        .app_data_dir()
        .map_err(|_| AppError::IoError(std::io::Error::new(
            std::io::ErrorKind::NotFound,
            "无法获取应用数据目录",
        )))?;

    let output_dir = app_data_dir.join("output");

    if !output_dir.exists() {
        return Ok(Vec::new());
    }

    let mut files = Vec::new();

    for entry in fs::read_dir(&output_dir).map_err(AppError::IoError)? {
        let entry = entry.map_err(AppError::IoError)?;
        let path = entry.path();

        if path.is_file() {
            if let Some(ext) = path.extension() {
                if ext == "md" {
                    let metadata = entry.metadata().map_err(AppError::IoError)?;
                    files.push(FileInfo {
                        name: path.file_name().unwrap().to_string_lossy().to_string(),
                        path: path.to_string_lossy().to_string(),
                        size: metadata.len(),
                        modified: metadata
                            .modified()
                            .ok()
                            .and_then(|t| t.duration_since(std::time::UNIX_EPOCH).ok())
                            .map(|d| d.as_secs()),
                    });
                }
            }
        }
    }

    // 按修改时间降序排序
    files.sort_by(|a, b| b.modified.cmp(&a.modified));

    Ok(files)
}

/// 文件信息结构
#[derive(Debug, Clone, serde::Serialize, serde::Deserialize)]
pub struct FileInfo {
    pub name: String,
    pub path: String,
    pub size: u64,
    pub modified: Option<u64>,
}

/// 打开输出目录
#[tauri::command]
pub async fn open_output_dir(app: AppHandle) -> AppResult<()> {
    let app_data_dir = app
        .path()
        .app_data_dir()
        .map_err(|_| AppError::IoError(std::io::Error::new(
            std::io::ErrorKind::NotFound,
            "无法获取应用数据目录",
        )))?;

    let output_dir = app_data_dir.join("output");

    // 确保目录存在
    fs::create_dir_all(&output_dir).map_err(AppError::IoError)?;

    // 使用系统默认程序打开目录
    #[cfg(target_os = "windows")]
    {
        std::process::Command::new("explorer")
            .arg(&output_dir)
            .spawn()
            .map_err(|e| AppError::IoError(e))?;
    }

    #[cfg(target_os = "macos")]
    {
        std::process::Command::new("open")
            .arg(&output_dir)
            .spawn()
            .map_err(|e| AppError::IoError(e))?;
    }

    #[cfg(target_os = "linux")]
    {
        std::process::Command::new("xdg-open")
            .arg(&output_dir)
            .spawn()
            .map_err(|e| AppError::IoError(e))?;
    }

    Ok(())
}

/// 删除输出文件
#[tauri::command]
pub async fn delete_output_file(file_path: String) -> AppResult<()> {
    let path = PathBuf::from(&file_path);

    // 安全检查：确保是 .md 文件
    if path.extension().map_or(true, |ext| ext != "md") {
        return Err(AppError::InvalidParameter("只能删除 Markdown 文件".to_string()));
    }

    fs::remove_file(&path).map_err(AppError::IoError)?;
    Ok(())
}

/// 检查文件是否存在
#[tauri::command]
pub async fn file_exists(file_path: String) -> AppResult<bool> {
    Ok(PathBuf::from(&file_path).exists())
}

/// 读取文件内容
#[tauri::command]
pub async fn read_file_content(file_path: String) -> AppResult<String> {
    fs::read_to_string(&file_path).map_err(AppError::IoError)
}

/// 选择文件
#[tauri::command]
pub async fn select_file() -> AppResult<String> {
    // 在 Tauri 2.x 中，dialog API 已变更，暂时注释掉文件选择功能
    // 后续需要使用新的 dialog API 重新实现
    Err(AppError::InvalidParameter("文件选择功能暂未实现".to_string()))
}
