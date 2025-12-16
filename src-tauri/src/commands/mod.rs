// src-tauri/src/commands/mod.rs
// Tauri Commands 模块导出

pub mod bangumi;
pub mod collection;
pub mod file;
pub mod window;

// 重新导出所有 commands 以便在 lib.rs 中注册
pub use bangumi::*;
pub use collection::*;
pub use file::*;
pub use window::*;
