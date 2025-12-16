// src-tauri/src/commands/bangumi.rs
// Bangumi API 相关 Tauri Commands

use crate::errors::AppResult;
use crate::models::{
    Credentials, Subject, SubjectCharacter, SubjectFullData, SubjectPerson, SubjectRelation,
};
use crate::utils::BangumiClient;

/// 获取条目完整信息
/// 
/// 该命令获取条目的基本信息、职员表、角色表和关联条目
#[tauri::command]
pub async fn get_subject_full_data(
    user_id: String,
    token: String,
    subject_id: u64,
) -> AppResult<SubjectFullData> {
    let credentials = Credentials::new(user_id, token);
    let client = BangumiClient::new(credentials)?;

    // 并发获取所有数据
    let (subject, persons, characters, relations) = tokio::try_join!(
        client.get_subject(subject_id),
        client.get_subject_persons(subject_id),
        client.get_subject_characters(subject_id),
        client.get_subject_relations(subject_id),
    )?;

    Ok(SubjectFullData {
        subject,
        persons,
        characters,
        relations,
    })
}

/// 获取条目基本信息
#[tauri::command]
pub async fn get_subject(
    user_id: String,
    token: String,
    subject_id: u64,
) -> AppResult<Subject> {
    let credentials = Credentials::new(user_id, token);
    let client = BangumiClient::new(credentials)?;
    client.get_subject(subject_id).await
}

/// 获取条目职员表
#[tauri::command]
pub async fn get_subject_persons(
    user_id: String,
    token: String,
    subject_id: u64,
) -> AppResult<Vec<SubjectPerson>> {
    let credentials = Credentials::new(user_id, token);
    let client = BangumiClient::new(credentials)?;
    client.get_subject_persons(subject_id).await
}

/// 获取条目角色表
#[tauri::command]
pub async fn get_subject_characters(
    user_id: String,
    token: String,
    subject_id: u64,
) -> AppResult<Vec<SubjectCharacter>> {
    let credentials = Credentials::new(user_id, token);
    let client = BangumiClient::new(credentials)?;
    client.get_subject_characters(subject_id).await
}

/// 获取条目关联项目
#[tauri::command]
pub async fn get_subject_relations(
    user_id: String,
    token: String,
    subject_id: u64,
) -> AppResult<Vec<SubjectRelation>> {
    let credentials = Credentials::new(user_id, token);
    let client = BangumiClient::new(credentials)?;
    client.get_subject_relations(subject_id).await
}

/// 验证认证信息是否有效
#[tauri::command]
pub async fn validate_credentials(
    user_id: String,
    token: String,
) -> AppResult<bool> {
    let credentials = Credentials::new(user_id, token);
    
    if !credentials.is_valid() {
        return Ok(false);
    }

    let client = BangumiClient::new(credentials)?;
    
    // 尝试获取用户收藏列表来验证 token
    match client.get_user_collections(2, None, 1, 0).await {
        Ok(_) => Ok(true),
        Err(_) => Ok(false),
    }
}
