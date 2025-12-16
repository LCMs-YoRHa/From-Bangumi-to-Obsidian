// src-tauri/src/models/subject.rs
// 条目相关数据结构

use serde::{Deserialize, Serialize};

/// 条目基本信息
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Subject {
    pub id: u64,
    pub name: String,
    #[serde(default)]
    pub name_cn: Option<String>,
    #[serde(default)]
    pub summary: Option<String>,
    #[serde(default)]
    pub date: Option<String>,
    #[serde(default)]
    pub platform: Option<String>,
    #[serde(default)]
    pub total_episodes: Option<u32>,
    #[serde(default)]
    pub images: Option<SubjectImages>,
    #[serde(default)]
    pub rating: Option<SubjectRating>,
    #[serde(default)]
    pub tags: Vec<SubjectTag>,
    #[serde(default)]
    pub infobox: Vec<InfoboxItem>,
}

/// 条目图片
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SubjectImages {
    pub large: Option<String>,
    pub common: Option<String>,
    pub medium: Option<String>,
    pub small: Option<String>,
    pub grid: Option<String>,
}

/// 条目评分
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SubjectRating {
    pub rank: Option<u32>,
    pub total: Option<u32>,
    pub score: Option<f64>,
}

/// 条目标签
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SubjectTag {
    pub name: String,
    pub count: u32,
}

/// Infobox 条目
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct InfoboxItem {
    pub key: String,
    pub value: serde_json::Value,
}

/// 条目职员
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SubjectPerson {
    pub id: u64,
    pub name: String,
    #[serde(default)]
    pub relation: Option<String>,
    #[serde(default)]
    pub images: Option<PersonImages>,
}

/// 人物图片
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct PersonImages {
    pub large: Option<String>,
    pub medium: Option<String>,
    pub small: Option<String>,
    pub grid: Option<String>,
}

/// 条目角色
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SubjectCharacter {
    pub id: u64,
    pub name: String,
    #[serde(default)]
    pub relation: Option<String>,
    #[serde(default)]
    pub images: Option<PersonImages>,
    #[serde(default)]
    pub actors: Vec<Actor>,
}

/// 声优/演员
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Actor {
    pub id: u64,
    pub name: String,
    #[serde(default)]
    pub images: Option<PersonImages>,
}

/// 关联条目
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SubjectRelation {
    pub id: u64,
    pub name: String,
    #[serde(default)]
    pub name_cn: Option<String>,
    #[serde(default)]
    pub relation: Option<String>,
    #[serde(rename = "type")]
    pub subject_type: Option<u8>,
}

/// 条目类型枚举
#[derive(Debug, Clone, Copy, Serialize, Deserialize, PartialEq, Eq)]
#[serde(from = "u8", into = "u8")]
pub enum SubjectType {
    Book = 1,
    Anime = 2,
    Music = 3,
    Game = 4,
    Real = 6,
}

impl From<u8> for SubjectType {
    fn from(value: u8) -> Self {
        match value {
            1 => SubjectType::Book,
            2 => SubjectType::Anime,
            3 => SubjectType::Music,
            4 => SubjectType::Game,
            6 => SubjectType::Real,
            _ => SubjectType::Anime, // 默认
        }
    }
}

impl From<SubjectType> for u8 {
    fn from(value: SubjectType) -> Self {
        value as u8
    }
}

impl Default for SubjectType {
    fn default() -> Self {
        SubjectType::Anime
    }
}

/// 完整的条目数据（包含所有关联信息）
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SubjectFullData {
    pub subject: Subject,
    pub persons: Vec<SubjectPerson>,
    pub characters: Vec<SubjectCharacter>,
    pub relations: Vec<SubjectRelation>,
}
