# FBTO - From Bangumi to Obsidian

<p align="center">
  <img src="src-tauri/icons/128x128.png" alt="FBTO Logo" width="128" height="128">
</p>

<p align="center">
  <strong>将 Bangumi 收藏条目导出为 Obsidian 搭建本地动漫库知识库的工具</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Tauri-2.0-blue?logo=tauri" alt="Tauri">
  <img src="https://img.shields.io/badge/Vue-3.5-green?logo=vue.js" alt="Vue 3">
  <img src="https://img.shields.io/badge/Rust-2021-orange?logo=rust" alt="Rust">
  <img src="https://img.shields.io/badge/TypeScript-5.6-blue?logo=typescript" alt="TypeScript">
</p>

## ✨ 功能特性

- 🚀 **批量导入** - 支持一键导出所有 Bangumi 收藏条目
- 📝 **单个导入** - 支持精准导出指定条目的详细信息
- 📁 **本地管理** - 方便管理已导出的 Markdown 文件
- 🔒 **隐私设置** - 批量管理收藏的隐私状态

## 🖥️ 应用截图

![alt text](/docs/image.png)

## 📦 安装

### 从 Release 下载

前往 [Releases](../../releases) 页面下载适合你操作系统的可执行文件：

- **Windows**: `.exe`
- **macOS**: `.dmg`

## 🔧 使用方法

### 1. 获取 Bangumi API 凭证

1. 访问 [Bangumi API](https://next.bgm.tv/demo/access-token) 创建一个 Access Token
2. 获取 Access Token
3. 在应用中输入你的用户名和 Access Token

### 2. 批量导入

1. 切换到「批量导入」选项卡
2. 选择要导出的收藏类型（动画、书籍、游戏等）
3. 点击「开始导出」
4. 等待导出完成

### 3. 单个导入

1. 切换到「单个导入」选项卡
2. 输入条目 ID 或从文件导入 ID 列表
3. 点击导出按钮

### 4. 本地管理

1. 切换到「本地管理」选项卡
2. 查看、打开或删除已导出的 Markdown 文件

## 📄 导出格式

导出的 Markdown 文件包含以下信息：

- 基本信息（标题、类型、话数等）
- 制作人员（Staff）
- 角色信息（Characters）
- 关联条目（Relations）
- 用户收藏状态和评分

## 🤝 贡献

欢迎提交 Issue 和 Pull Request！

## 📜 许可证

[MIT License](LICENSE)

## 🙏 致谢

- [Bangumi API](https://bangumi.github.io/api/) - 提供数据接口
- [Obsidian](https://obsidian.md/) - 优秀的知识管理工具