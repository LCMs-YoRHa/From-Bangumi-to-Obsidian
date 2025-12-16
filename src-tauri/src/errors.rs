// src-tauri/src/errors.rs
// 统一错误处理模块

use thiserror::Error;

/// 应用统一错误类型
#[derive(Error, Debug)]
pub enum AppError {
    #[error("HTTP 请求失败: {0}")]
    HttpError(#[from] reqwest::Error),

    #[error("JSON 解析失败: {0}")]
    JsonError(#[from] serde_json::Error),

    #[error("文件操作失败: {0}")]
    IoError(#[from] std::io::Error),

    #[error("API 错误: {status} - {message}")]
    ApiError { status: u16, message: String },

    #[error("认证失败: {0}")]
    AuthError(String),

    #[error("无效参数: {0}")]
    InvalidParameter(String),

    #[error("未找到资源: {0}")]
    NotFound(String),
}

/// 实现 Serialize 以便可以传递给前端
impl serde::Serialize for AppError {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        serializer.serialize_str(&self.to_string())
    }
}

/// 应用 Result 类型别名
pub type AppResult<T> = Result<T, AppError>;
