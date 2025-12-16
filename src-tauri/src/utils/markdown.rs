// src-tauri/src/utils/markdown.rs
// Markdown 文件生成器

use crate::errors::{AppError, AppResult};
use crate::models::{
    CollectionType, ExportData, Subject, SubjectCharacter, SubjectPerson, SubjectRelation,
    UserCollection,
};
use regex::Regex;
use std::fs;
use std::path::Path;

/// Markdown 生成器
pub struct MarkdownGenerator;

impl MarkdownGenerator {
    /// 清理文件名中的非法字符
    pub fn sanitize_filename(filename: &str) -> String {
        let re = Regex::new(r#"[\\/*?:"<>|]"#).unwrap();
        let sanitized = re.replace_all(filename, "").to_string();

        if sanitized.ends_with(".md") {
            sanitized
        } else {
            format!("{}.md", sanitized)
        }
    }

    /// 从条目数据构建导出数据
    pub fn build_export_data(
        subject: &Subject,
        user_collection: &UserCollection,
        persons: &[SubjectPerson],
        characters: &[SubjectCharacter],
        relations: &[SubjectRelation],
    ) -> ExportData {
        // 提取导演和制作公司
        let mut director = None;
        let mut studio = None;

        for item in &subject.infobox {
            let key = &item.key;
            if let serde_json::Value::String(value) = &item.value {
                if key == "导演" {
                    director = Some(value.clone());
                } else if key == "製作" || key == "制作" {
                    studio = Some(value.clone());
                }
            }
        }

        // 合并标签
        let mut combined_tags: std::collections::HashSet<String> =
            subject.tags.iter().map(|t| t.name.clone()).collect();
        combined_tags.extend(user_collection.tags.iter().cloned());
        let tags: Vec<String> = combined_tags.into_iter().collect();

        // 处理评论（移除换行符）
        let person_comment = user_collection
            .comment
            .as_ref()
            .map(|c| c.replace('\n', ""));

        // 观看状态映射
        let watch_status = CollectionType::from(user_collection.collection_type).display_name();

        // 构建 staff 列表
        let staff: Vec<_> = persons
            .iter()
            .map(|p| crate::models::StaffEntry {
                relation: p.relation.clone().unwrap_or_default(),
                name: p.name.clone(),
                image: p.images.as_ref().and_then(|i| i.grid.clone()),
            })
            .collect();

        // 构建角色列表
        let characters_list: Vec<_> = characters
            .iter()
            .map(|c| crate::models::CharacterEntry {
                relation: c.relation.clone().unwrap_or_default(),
                name: c.name.clone(),
                actor_name: c.actors.first().map(|a| a.name.clone()),
                character_image: c.images.as_ref().and_then(|i| i.grid.clone()),
                actor_image: c
                    .actors
                    .first()
                    .and_then(|a| a.images.as_ref())
                    .and_then(|i| i.grid.clone()),
            })
            .collect();

        // 构建关联列表
        let relations_list: Vec<_> = relations
            .iter()
            .map(|r| crate::models::RelationEntry {
                relation: r.relation.clone().unwrap_or_default(),
                name: r.name.clone(),
                name_cn: r.name_cn.clone(),
            })
            .collect();

        ExportData {
            name: subject.name.clone(),
            name_cn: subject.name_cn.clone(),
            episodes: subject.total_episodes,
            platform: subject.platform.clone(),
            director,
            studio,
            tags,
            score: subject.rating.as_ref().and_then(|r| r.score),
            person_score: user_collection.rate,
            broadcast_date: subject.date.clone(),
            watch_status: watch_status.to_string(),
            watch_date: user_collection.updated_at.clone(),
            person_comment,
            img_link: subject.images.as_ref().and_then(|i| i.large.clone()),
            summary: subject.summary.clone().map(|s| s.trim_start().to_string()),
            url: format!("https://bangumi.tv/subject/{}", subject.id),
            staff,
            characters: characters_list,
            relations: relations_list,
        }
    }

    /// 生成 Markdown 内容
    pub fn generate_markdown(data: &ExportData) -> String {
        let mut content = String::new();

        // YAML Front Matter
        content.push_str("---\n");
        content.push_str(&format!("原名: {}\n", data.name));

        if let Some(ref episodes) = data.episodes {
            content.push_str(&format!("话数: {}\n", episodes));
        }

        if let Some(ref platform) = data.platform {
            content.push_str(&format!("版本: {}\n", platform));
        }

        if let Some(ref director) = data.director {
            content.push_str(&format!("导演: {}\n", director));
        }

        content.push_str(&format!("tags: {}\n", data.tags.join(", ")));

        if let Some(ref studio) = data.studio {
            content.push_str(&format!("动画制作: {}\n", studio));
        }

        if let Some(score) = data.score {
            content.push_str(&format!("收录评分: {}\n", score));
        }

        if let Some(person_score) = data.person_score {
            content.push_str(&format!("个人评分: {}\n", person_score));
        }

        if let Some(ref date) = data.broadcast_date {
            content.push_str(&format!("放送开始: {}\n", date));
        }

        content.push_str(&format!("观看状态: {}\n", data.watch_status));

        if let Some(ref watch_date) = data.watch_date {
            content.push_str(&format!("收藏日期: {}\n", watch_date));
        }

        if let Some(ref comment) = data.person_comment {
            content.push_str(&format!("个人评论: {}\n", comment));
        }

        if let Some(ref img_link) = data.img_link {
            content.push_str(&format!("封面链接: {}\n", img_link));
        }

        content.push_str(&format!("URL: {}\n", data.url));
        content.push_str("---\n\n");

        // 剧情梗概
        content.push_str("## 剧情梗概:\n");
        if let Some(ref summary) = data.summary {
            content.push_str(&format!("{}\n\n", summary));
        } else {
            content.push_str("暂无\n\n");
        }

        content.push_str("---\n\n");

        // 制作人员
        content.push_str("## 制作人员:\n");
        content.push_str("<table style=\"table-layout: fixed; width: 100%;\">\n");

        for (i, staff) in data.staff.iter().enumerate() {
            if i % 4 == 0 {
                content.push_str("<tr>\n");
            }

            content.push_str("<td style=\"width: 25%; text-align: center; vertical-align: top;\">\n");
            content.push_str(&format!("{}<br>{}\n", staff.relation, staff.name));

            if let Some(ref image) = staff.image {
                content.push_str(&format!(
                    "<br><img src=\"{}\" style=\"max-width: 100px; max-height: 100px;\">\n",
                    image
                ));
            }

            content.push_str("</td>\n");

            if (i + 1) % 4 == 0 {
                content.push_str("</tr>\n");
            }
        }

        // 关闭未完成的行
        if !data.staff.is_empty() && data.staff.len() % 4 != 0 {
            content.push_str("</tr>\n");
        }

        content.push_str("</table>\n\n");
        content.push_str("---\n\n");

        // 出演角色
        content.push_str("## 出演角色:\n");
        content.push_str("<table style=\"table-layout: fixed; width: 100%;\">\n");

        for (i, character) in data.characters.iter().enumerate() {
            if i % 3 == 0 {
                content.push_str("<tr>\n");
            }

            content.push_str("<td style=\"width: 33.33%; text-align: center; vertical-align: top;\">\n");
            content.push_str(&format!(
                "{}<br>{}<br>配音: {}\n",
                character.relation,
                character.name,
                character.actor_name.as_deref().unwrap_or("")
            ));

            if let Some(ref char_image) = character.character_image {
                content.push_str(&format!(
                    "<br><img src=\"{}\" style=\"max-width: 100px; max-height: 100px;\">\n",
                    char_image
                ));
            }

            if let Some(ref actor_image) = character.actor_image {
                content.push_str(&format!(
                    "<br><img src=\"{}\" style=\"max-width: 100px; max-height: 100px;\">\n",
                    actor_image
                ));
            }

            content.push_str("</td>\n");

            if (i + 1) % 3 == 0 {
                content.push_str("</tr>\n");
            }
        }

        // 关闭未完成的行
        if !data.characters.is_empty() && data.characters.len() % 3 != 0 {
            content.push_str("</tr>\n");
        }

        content.push_str("</table>\n\n");
        content.push_str("---\n\n");

        // 关联条目
        content.push_str("## 关联条目:\n");
        for relation in &data.relations {
            let display_name = relation
                .name_cn
                .as_ref()
                .filter(|n| !n.is_empty())
                .unwrap_or(&relation.name);
            content.push_str(&format!("{} - [[{}]]\n", relation.relation, display_name));
        }

        content.push_str("\n---\n\n");

        // 封面图
        if let Some(ref img_link) = data.img_link {
            let alt = data.name_cn.as_deref().unwrap_or(&data.name);
            content.push_str(&format!(
                "<img src=\"{}\" alt=\"{}\" style=\"max-width: 100%;\">\n",
                img_link, alt
            ));
        }

        content
    }

    /// 写入 Markdown 文件
    pub fn write_markdown_file(
        output_dir: &Path,
        data: &ExportData,
    ) -> AppResult<String> {
        // 确保输出目录存在
        fs::create_dir_all(output_dir).map_err(AppError::IoError)?;

        // 生成文件名
        let display_name = data
            .name_cn
            .as_ref()
            .filter(|n| !n.is_empty())
            .unwrap_or(&data.name);
        let filename = Self::sanitize_filename(display_name);
        let file_path = output_dir.join(&filename);

        // 生成内容并写入
        let content = Self::generate_markdown(data);
        fs::write(&file_path, content).map_err(AppError::IoError)?;

        Ok(filename)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_sanitize_filename() {
        assert_eq!(
            MarkdownGenerator::sanitize_filename("test:file*name"),
            "testfilename.md"
        );
        assert_eq!(
            MarkdownGenerator::sanitize_filename("already.md"),
            "already.md"
        );
        assert_eq!(
            MarkdownGenerator::sanitize_filename("test<>file"),
            "testfile.md"
        );
    }
}
