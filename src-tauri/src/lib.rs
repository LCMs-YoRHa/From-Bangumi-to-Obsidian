// src-tauri/src/lib.rs
// Tauri 应用入口 - 保持精简，仅负责 Builder 配置和模块导出
//
// ⚠️ 架构规范：
// - 此文件禁止编写任何业务逻辑函数
// - 所有 Commands 必须拆分到 commands/ 目录
// - 所有数据结构必须定义在 models/ 目录
// - 所有工具函数必须放在 utils/ 目录

// 模块声明
mod commands;
mod errors;
mod models;
mod utils;

// 重新导出模块（避免命名冲突，使用模块前缀访问）
pub use errors::*;

// 使用更明确的导出方式避免冲突
pub mod api_commands {
    pub use crate::commands::*;
}

pub mod data_models {
    pub use crate::models::*;
}

pub mod api_utils {
    pub use crate::utils::*;
}

/// 应用主入口点
#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        // 插件配置
        .plugin(tauri_plugin_opener::init())
        // 注册所有 Tauri Commands
        .invoke_handler(tauri::generate_handler![
            // Bangumi API Commands (bangumi.rs)
            commands::get_subject_full_data,
            commands::get_subject,
            commands::get_subject_persons,
            commands::get_subject_characters,
            commands::get_subject_relations,
            commands::validate_credentials,
            // Collection Commands (collection.rs)
            commands::get_user_collections,
            commands::get_user_collection,
            commands::fetch_all_collection_ids,
            commands::update_collection_privacy,
            commands::batch_update_privacy,
            commands::export_subject_to_markdown,
            commands::batch_export_to_markdown,
            commands::batch_export_parallel,
            commands::batch_update_privacy_parallel,
            // File Commands (file.rs)
            commands::read_credentials_file,
            commands::save_credentials_file,
            commands::read_subject_ids_file,
            commands::save_subject_ids_file,
            commands::get_app_data_dir,
            commands::get_output_dir,
            commands::list_output_files,
            commands::open_output_dir,
            commands::delete_output_file,
            commands::file_exists,
            commands::read_file_content,
            commands::select_file,
            // Window Commands (window.rs)
            commands::is_window_maximized,
            commands::minimize_window,
            commands::toggle_maximize_window,
            commands::close_window,
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
