// src-tauri/src/models/collection.rs
// 收藏相关数据结构

use serde::{Deserialize, Serialize};

/// 收藏状态类型
#[derive(Debug, Clone, Copy, Serialize, Deserialize, PartialEq, Eq)]
#[serde(from = "u8", into = "u8")]
pub enum CollectionType {
    Wish = 1,     // 想看
    Collect = 2,  // 看过
    Doing = 3,    // 在看
    OnHold = 4,   // 搁置
    Dropped = 5,  // 抛弃
}

impl From<u8> for CollectionType {
    fn from(value: u8) -> Self {
        match value {
            1 => CollectionType::Wish,
            2 => CollectionType::Collect,
            3 => CollectionType::Doing,
            4 => CollectionType::OnHold,
            5 => CollectionType::Dropped,
            _ => CollectionType::Wish,
        }
    }
}

impl From<CollectionType> for u8 {
    fn from(value: CollectionType) -> Self {
        value as u8
    }
}

impl CollectionType {
    /// 获取中文显示名称
    pub fn display_name(&self) -> &'static str {
        match self {
            CollectionType::Wish => "想看",
            CollectionType::Collect => "看过",
            CollectionType::Doing => "在看",
            CollectionType::OnHold => "搁置",
            CollectionType::Dropped => "抛弃",
        }
    }
}

/// 收藏列表 API 响应
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CollectionListResponse {
    pub total: u32,
    pub limit: u32,
    pub offset: u32,
    pub data: Vec<CollectionItem>,
}

/// 收藏列表项
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CollectionItem {
    pub subject_id: u64,
    pub subject_type: u8,
    #[serde(rename = "type")]
    pub collection_type: u8,
    pub rate: Option<u8>,
    pub comment: Option<String>,
    pub tags: Vec<String>,
    #[serde(rename = "private")]
    pub is_private: bool,
    pub updated_at: Option<String>,
}

/// 单个收藏详情
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct UserCollection {
    pub subject_id: u64,
    #[serde(rename = "type")]
    pub collection_type: u8,
    pub rate: Option<u8>,
    pub comment: Option<String>,
    pub tags: Vec<String>,
    #[serde(rename = "private")]
    pub is_private: bool,
    pub updated_at: Option<String>,
}

/// 更新收藏请求
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct UpdateCollectionRequest {
    #[serde(skip_serializing_if = "Option::is_none")]
    pub rate: Option<u8>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub comment: Option<String>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub tags: Option<Vec<String>>,
    #[serde(rename = "private", skip_serializing_if = "Option::is_none")]
    pub is_private: Option<bool>,
}

/// 导出的 Markdown 数据
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ExportData {
    pub name: String,
    pub name_cn: Option<String>,
    pub episodes: Option<u32>,
    pub platform: Option<String>,
    pub director: Option<String>,
    pub studio: Option<String>,
    pub tags: Vec<String>,
    pub score: Option<f64>,
    pub person_score: Option<u8>,
    pub broadcast_date: Option<String>,
    pub watch_status: String,
    pub watch_date: Option<String>,
    pub person_comment: Option<String>,
    pub img_link: Option<String>,
    pub summary: Option<String>,
    pub url: String,
    pub staff: Vec<StaffEntry>,
    pub characters: Vec<CharacterEntry>,
    pub relations: Vec<RelationEntry>,
}

/// 制作人员条目
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct StaffEntry {
    pub relation: String,
    pub name: String,
    pub image: Option<String>,
}

/// 角色条目
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CharacterEntry {
    pub relation: String,
    pub name: String,
    pub actor_name: Option<String>,
    pub character_image: Option<String>,
    pub actor_image: Option<String>,
}

/// 关联条目
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct RelationEntry {
    pub relation: String,
    pub name: String,
    pub name_cn: Option<String>,
}

/// 批量导入参数
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct BatchImportParams {
    pub subject_type: u8,
    pub collection_type: Option<u8>,
    pub limit: u32,
    pub offset: u32,
    pub auto_process: bool,
}

impl Default for BatchImportParams {
    fn default() -> Self {
        Self {
            subject_type: 2, // 动画
            collection_type: None,
            limit: 30,
            offset: 0,
            auto_process: true,
        }
    }
}

/// 任务进度信息
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TaskProgress {
    pub current: u32,
    pub total: u32,
    pub message: String,
    pub is_completed: bool,
    pub is_error: bool,
}

impl TaskProgress {
    pub fn new(current: u32, total: u32, message: impl Into<String>) -> Self {
        Self {
            current,
            total,
            message: message.into(),
            is_completed: false,
            is_error: false,
        }
    }

    pub fn completed(message: impl Into<String>) -> Self {
        Self {
            current: 100,
            total: 100,
            message: message.into(),
            is_completed: true,
            is_error: false,
        }
    }

    pub fn error(message: impl Into<String>) -> Self {
        Self {
            current: 0,
            total: 0,
            message: message.into(),
            is_completed: false,
            is_error: true,
        }
    }

    pub fn percentage(&self) -> f64 {
        if self.total == 0 {
            0.0
        } else {
            (self.current as f64 / self.total as f64) * 100.0
        }
    }
}
