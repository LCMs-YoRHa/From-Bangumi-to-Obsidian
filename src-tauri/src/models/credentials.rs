// src-tauri/src/models/credentials.rs
// 认证信息数据结构

use serde::{Deserialize, Serialize};

/// 用户认证信息
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Credentials {
    /// Bangumi 用户 ID
    pub user_id: String,
    /// Bangumi API Token
    pub token: String,
}

impl Credentials {
    pub fn new(user_id: String, token: String) -> Self {
        Self { user_id, token }
    }

    /// 验证认证信息是否有效（非空）
    pub fn is_valid(&self) -> bool {
        !self.user_id.trim().is_empty() && !self.token.trim().is_empty()
    }
}

/// 从文件内容解析认证信息
impl TryFrom<&str> for Credentials {
    type Error = String;

    fn try_from(content: &str) -> Result<Self, Self::Error> {
        let lines: Vec<&str> = content.lines().collect();
        if lines.len() < 2 {
            return Err("认证文件格式错误：需要两行（user_id 和 token）".to_string());
        }
        Ok(Self {
            user_id: lines[0].trim().to_string(),
            token: lines[1].trim().to_string(),
        })
    }
}
