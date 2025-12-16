// src-tauri/src/utils/http.rs
// HTTP 客户端封装

use reqwest::{Client, header};
use crate::errors::{AppError, AppResult};
use crate::models::{
    Credentials, Subject, SubjectPerson, SubjectCharacter, SubjectRelation,
    CollectionListResponse, UserCollection, UpdateCollectionRequest
};

/// Bangumi API 客户端
#[derive(Clone)]
pub struct BangumiClient {
    client: Client,
    credentials: Credentials,
}

impl BangumiClient {
    const BASE_URL: &'static str = "https://api.bgm.tv/v0";

    /// 创建新的 API 客户端
    pub fn new(credentials: Credentials) -> AppResult<Self> {
        if !credentials.is_valid() {
            return Err(AppError::AuthError("认证信息无效".to_string()));
        }

        let client = Client::builder()
            .timeout(std::time::Duration::from_secs(30))
            .build()?;

        Ok(Self { client, credentials })
    }

    /// 构建请求头
    fn build_headers(&self, with_auth: bool) -> header::HeaderMap {
        let mut headers = header::HeaderMap::new();
        headers.insert(
            header::ACCEPT,
            header::HeaderValue::from_static("application/json"),
        );
        headers.insert(
            header::USER_AGENT,
            header::HeaderValue::from_str(&format!(
                "{}/fbto-app",
                self.credentials.user_id
            ))
            .unwrap_or_else(|_| header::HeaderValue::from_static("fbto-app")),
        );

        if with_auth {
            if let Ok(auth_value) =
                header::HeaderValue::from_str(&format!("Bearer {}", self.credentials.token))
            {
                headers.insert(header::AUTHORIZATION, auth_value);
            }
        }

        headers
    }

    /// 获取用户收藏列表
    pub async fn get_user_collections(
        &self,
        subject_type: u8,
        collection_type: Option<u8>,
        limit: u32,
        offset: u32,
    ) -> AppResult<CollectionListResponse> {
        let url = format!(
            "{}/users/{}/collections",
            Self::BASE_URL,
            self.credentials.user_id
        );

        let mut params = vec![
            ("subject_type", subject_type.to_string()),
            ("limit", limit.to_string()),
            ("offset", offset.to_string()),
        ];

        if let Some(ct) = collection_type {
            params.push(("type", ct.to_string()));
        }

        let response = self
            .client
            .get(&url)
            .headers(self.build_headers(true))
            .query(&params)
            .send()
            .await?;

        if !response.status().is_success() {
            let status = response.status().as_u16();
            let message = response.text().await.unwrap_or_default();
            return Err(AppError::ApiError { status, message });
        }

        let data = response.json::<CollectionListResponse>().await?;
        Ok(data)
    }

    /// 获取用户单个条目收藏信息
    pub async fn get_user_collection(&self, subject_id: u64) -> AppResult<UserCollection> {
        let url = format!(
            "{}/users/{}/collections/{}",
            Self::BASE_URL,
            self.credentials.user_id,
            subject_id
        );

        let response = self
            .client
            .get(&url)
            .headers(self.build_headers(true))
            .send()
            .await?;

        if !response.status().is_success() {
            let status = response.status().as_u16();
            let message = response.text().await.unwrap_or_default();
            return Err(AppError::ApiError { status, message });
        }

        let data = response.json::<UserCollection>().await?;
        Ok(data)
    }

    /// 获取条目公共信息
    pub async fn get_subject(&self, subject_id: u64) -> AppResult<Subject> {
        let url = format!("{}/subjects/{}", Self::BASE_URL, subject_id);

        let response = self
            .client
            .get(&url)
            .headers(self.build_headers(false))
            .send()
            .await?;

        if !response.status().is_success() {
            let status = response.status().as_u16();
            let message = response.text().await.unwrap_or_default();
            return Err(AppError::ApiError { status, message });
        }

        let data = response.json::<Subject>().await?;
        Ok(data)
    }

    /// 获取条目职员表
    pub async fn get_subject_persons(&self, subject_id: u64) -> AppResult<Vec<SubjectPerson>> {
        let url = format!("{}/subjects/{}/persons", Self::BASE_URL, subject_id);

        let response = self
            .client
            .get(&url)
            .headers(self.build_headers(false))
            .send()
            .await?;

        if !response.status().is_success() {
            let status = response.status().as_u16();
            let message = response.text().await.unwrap_or_default();
            return Err(AppError::ApiError { status, message });
        }

        let data = response.json::<Vec<SubjectPerson>>().await?;
        Ok(data)
    }

    /// 获取条目角色表
    pub async fn get_subject_characters(
        &self,
        subject_id: u64,
    ) -> AppResult<Vec<SubjectCharacter>> {
        let url = format!("{}/subjects/{}/characters", Self::BASE_URL, subject_id);

        let response = self
            .client
            .get(&url)
            .headers(self.build_headers(false))
            .send()
            .await?;

        if !response.status().is_success() {
            let status = response.status().as_u16();
            let message = response.text().await.unwrap_or_default();
            return Err(AppError::ApiError { status, message });
        }

        let data = response.json::<Vec<SubjectCharacter>>().await?;
        Ok(data)
    }

    /// 获取条目关联项目
    pub async fn get_subject_relations(&self, subject_id: u64) -> AppResult<Vec<SubjectRelation>> {
        let url = format!("{}/subjects/{}/subjects", Self::BASE_URL, subject_id);

        let response = self
            .client
            .get(&url)
            .headers(self.build_headers(false))
            .send()
            .await?;

        if !response.status().is_success() {
            let status = response.status().as_u16();
            let message = response.text().await.unwrap_or_default();
            return Err(AppError::ApiError { status, message });
        }

        let data = response.json::<Vec<SubjectRelation>>().await?;
        Ok(data)
    }

    /// 更新收藏隐私设置
    pub async fn update_collection_privacy(
        &self,
        subject_id: u64,
        is_private: bool,
    ) -> AppResult<()> {
        let url = format!(
            "{}/users/-/collections/{}",
            Self::BASE_URL,
            subject_id
        );

        let request = UpdateCollectionRequest {
            rate: None,
            comment: None,
            tags: None,
            is_private: Some(is_private),
        };

        let response = self
            .client
            .post(&url)
            .headers(self.build_headers(true))
            .json(&request)
            .send()
            .await?;

        if response.status().as_u16() == 202 {
            return Ok(());
        }

        if !response.status().is_success() {
            let status = response.status().as_u16();
            let message = response.text().await.unwrap_or_default();
            return Err(AppError::ApiError { status, message });
        }

        Ok(())
    }

    /// 获取所有收藏的条目 ID
    pub async fn fetch_all_collection_ids(
        &self,
        subject_type: u8,
        collection_type: Option<u8>,
        limit: u32,
    ) -> AppResult<Vec<u64>> {
        let mut all_ids = Vec::new();
        let mut offset = 0u32;

        loop {
            let response = self
                .get_user_collections(subject_type, collection_type, limit, offset)
                .await?;

            if response.data.is_empty() {
                break;
            }

            all_ids.extend(response.data.iter().map(|item| item.subject_id));
            offset += limit;

            // 如果返回的数量小于 limit，说明已经是最后一页
            if response.data.len() < limit as usize {
                break;
            }
        }

        Ok(all_ids)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_credentials_validation() {
        let valid = Credentials::new("12345".to_string(), "token123".to_string());
        assert!(valid.is_valid());

        let invalid = Credentials::new("".to_string(), "token".to_string());
        assert!(!invalid.is_valid());
    }
}
