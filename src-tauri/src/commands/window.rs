// src-tauri/src/commands/window.rs
// 窗口控制相关 Tauri Commands

use tauri::WebviewWindow;

/// 获取当前窗口最大化状态
#[tauri::command]
pub async fn is_window_maximized(window: WebviewWindow) -> bool {
    window.is_maximized().unwrap_or(false)
}

/// 最小化窗口
#[tauri::command]
pub async fn minimize_window(window: WebviewWindow) {
    let _ = window.minimize();
}

/// 最大化/还原窗口
#[tauri::command]
pub async fn toggle_maximize_window(window: WebviewWindow) {
    if window.is_maximized().unwrap_or(false) {
        let _ = window.unmaximize();
    } else {
        let _ = window.maximize();
    }
}

/// 关闭窗口
#[tauri::command]
pub async fn close_window(window: WebviewWindow) {
    let _ = window.close();
}
