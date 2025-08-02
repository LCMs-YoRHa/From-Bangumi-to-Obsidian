import sys
import os
import threading
from PyQt6.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                             QHBoxLayout, QTabWidget, QLabel, QLineEdit, QPushButton, 
                             QComboBox, QSpinBox, QTextEdit, QProgressBar, QFrame,
                             QGroupBox, QGridLayout, QMessageBox, QFileDialog,
                             QCheckBox, QSplitter)
from PyQt6.QtCore import Qt, QThread, pyqtSignal, QTimer
from PyQt6.QtGui import QFont, QIcon, QPalette, QColor
import json

# 导入现有模块
import Bangumi
from getInfo import *

def get_project_root_path(filename):
    """获取项目根目录中文件的完整路径"""
    # 如果文件存在于当前目录
    if os.path.exists(filename):
        return filename
    
    # 尝试在上级目录（项目根目录）查找
    parent_path = os.path.join(os.path.dirname(os.path.dirname(__file__)), filename)
    if os.path.exists(parent_path):
        return parent_path
    
    # 如果都不存在，返回项目根目录的路径
    return os.path.join(os.path.dirname(os.path.dirname(__file__)), filename)

class WorkerThread(QThread):
    """工作线程类，用于在后台执行任务"""
    progress_updated = pyqtSignal(int)
    status_updated = pyqtSignal(str)
    finished = pyqtSignal(bool, str)
    log_updated = pyqtSignal(str)
    
    def __init__(self, task_type, user_id, token, **kwargs):
        super().__init__()
        self.task_type = task_type
        self.user_id = user_id
        self.token = token
        self.kwargs = kwargs
        self.is_cancelled = False
    
    def cancel(self):
        self.is_cancelled = True
    
    def run(self):
        try:
            if self.task_type == "fetch_all":
                self.fetch_all_collections()
            elif self.task_type == "fetch_single":
                self.fetch_single_subject()
            elif self.task_type == "process_ids":
                self.process_subject_ids()
            elif self.task_type == "update_privacy":
                self.update_privacy_settings()
        except Exception as e:
            self.finished.emit(False, f"执行出错: {str(e)}")
    
    def fetch_all_collections(self):
        self.status_updated.emit("开始获取收藏数据...")
        subject_type = self.kwargs.get('subject_type', 2)
        type_filter = self.kwargs.get('type_filter')
        limit = self.kwargs.get('limit', 30)
        offset = self.kwargs.get('offset', 0)
        
        try:
            fetch_and_write_all_subject_ids(self.user_id, self.token, subject_type, type_filter, limit, offset)
            self.log_updated.emit("✅ 收藏数据获取完成，保存为 subject_ids.txt")
            
            if self.kwargs.get('auto_process', False):
                self.status_updated.emit("开始处理条目...")
                process_subject_ids(self.user_id, self.token)
            self.log_updated.emit("✅ 所有条目处理完成，Markdown文件已保存到 output 文件夹")
            
            self.finished.emit(True, "任务完成！Markdown文件已保存到 output 文件夹")
        except Exception as e:
            self.finished.emit(False, f"获取收藏数据失败: {str(e)}")
    
    def fetch_single_subject(self):
        subject_id = self.kwargs.get('subject_id')
        self.status_updated.emit(f"正在处理条目 {subject_id}...")
        
        try:
            write_extended_subject_data(self.user_id, subject_id, self.token)
            self.log_updated.emit(f"✅ 条目 {subject_id} 处理完成，文件已保存到 output 文件夹")
            self.finished.emit(True, "单个条目处理完成！文件已保存到 output 文件夹")
        except Exception as e:
            self.finished.emit(False, f"处理条目失败: {str(e)}")
    
    def process_subject_ids(self):
        self.status_updated.emit("开始处理本地条目列表...")
        
        try:
            subject_ids_path = get_project_root_path('subject_ids.txt')
            if not os.path.exists(subject_ids_path):
                self.finished.emit(False, "找不到 subject_ids.txt 文件")
                return
            
            process_subject_ids(self.user_id, self.token)
            self.log_updated.emit("✅ 所有条目处理完成，Markdown文件已保存到 output 文件夹")
            self.finished.emit(True, "本地条目处理完成！文件已保存到 output 文件夹")
        except Exception as e:
            self.finished.emit(False, f"处理条目失败: {str(e)}")
    
    def update_privacy_settings(self):
        self.status_updated.emit("开始更新隐私设置...")
        is_private = self.kwargs.get('is_private', True)
        
        try:
            subject_ids_path = get_project_root_path('subject_ids.txt')
            if not os.path.exists(subject_ids_path):
                self.finished.emit(False, "找不到 subject_ids.txt 文件")
                return
            
            with open(subject_ids_path, 'r', encoding='utf-8') as f:
                subject_ids = f.read().split(',')
            
            success_count = 0
            for i, subject_id in enumerate(subject_ids):
                if self.is_cancelled:
                    break
                
                try:
                    result = Bangumi.update_collection_privacy(subject_id, self.token, is_private, self.user_id)
                    if result.get("status") == "success":
                        success_count += 1
                        self.log_updated.emit(f"✅ 条目 {subject_id} 隐私设置更新成功")
                    else:
                        self.log_updated.emit(f"❌ 条目 {subject_id} 隐私设置更新失败")
                except Exception as e:
                    self.log_updated.emit(f"❌ 条目 {subject_id} 更新出错: {str(e)}")
                
                progress = int((i + 1) / len(subject_ids) * 100)
                self.progress_updated.emit(progress)
            
            self.finished.emit(True, f"隐私设置更新完成！成功更新 {success_count} 个条目")
        except Exception as e:
            self.finished.emit(False, f"更新隐私设置失败: {str(e)}")


class BangumiGUI(QMainWindow):
    def __init__(self):
        super().__init__()
        self.user_id = ""
        self.token = ""
        self.worker_thread = None
        self.init_ui()
        self.load_credentials()
        self.apply_dark_theme()
    
    def init_ui(self):
        self.setWindowTitle("Bangumi to Obsidian - GUI版本")
        self.setGeometry(100, 100, 900, 700)
        
        # 创建中央widget和主布局
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)
        
        # 创建顶部认证区域
        self.create_auth_section(main_layout)
        
        # 创建选项卡
        self.create_tabs(main_layout)
        
        # 创建底部状态区域
        self.create_status_section(main_layout)
    
    def create_auth_section(self, parent_layout):
        """创建认证信息输入区域"""
        auth_group = QGroupBox("认证信息")
        auth_layout = QGridLayout(auth_group)
        
        # 用户ID输入
        auth_layout.addWidget(QLabel("用户ID:"), 0, 0)
        self.user_id_input = QLineEdit()
        self.user_id_input.setPlaceholderText("请输入Bangumi用户ID")
        auth_layout.addWidget(self.user_id_input, 0, 1)
        
        # Token输入
        auth_layout.addWidget(QLabel("Token:"), 1, 0)
        self.token_input = QLineEdit()
        self.token_input.setPlaceholderText("请输入Bangumi API Token")
        self.token_input.setEchoMode(QLineEdit.EchoMode.Password)
        auth_layout.addWidget(self.token_input, 1, 1)
        
        # 按钮区域
        button_layout = QHBoxLayout()
        
        load_btn = QPushButton("📁 从文件加载")
        load_btn.clicked.connect(self.load_credentials)
        button_layout.addWidget(load_btn)
        
        save_btn = QPushButton("💾 保存到文件")
        save_btn.clicked.connect(self.save_credentials)
        button_layout.addWidget(save_btn)
        
        button_layout.addStretch()
        
        auth_layout.addLayout(button_layout, 2, 0, 1, 2)
        parent_layout.addWidget(auth_group)
    
    def create_tabs(self, parent_layout):
        """创建功能选项卡"""
        self.tab_widget = QTabWidget()
        
        # 批量导入选项卡
        self.create_batch_tab()
        
        # 单个导入选项卡
        self.create_single_tab()
        
        # 本地处理选项卡
        self.create_local_tab()
        
        # 隐私设置选项卡
        self.create_privacy_tab()
        
        parent_layout.addWidget(self.tab_widget)
    
    def create_batch_tab(self):
        """创建批量导入选项卡"""
        tab = QWidget()
        layout = QVBoxLayout(tab)
        
        # 设置区域
        settings_group = QGroupBox("导入设置")
        settings_layout = QGridLayout(settings_group)
        
        # 条目类型选择
        settings_layout.addWidget(QLabel("条目类型:"), 0, 0)
        self.subject_type_combo = QComboBox()
        self.subject_type_combo.addItems(["书籍 (1)", "动画 (2)", "音乐 (3)", "游戏 (4)", "三次元 (6)"])
        self.subject_type_combo.setCurrentIndex(1)  # 默认选择动画
        settings_layout.addWidget(self.subject_type_combo, 0, 1)
        
        # 收藏状态选择
        settings_layout.addWidget(QLabel("收藏状态:"), 1, 0)
        self.status_combo = QComboBox()
        self.status_combo.addItems(["全部", "想看 (1)", "看过 (2)", "在看 (3)", "搁置 (4)", "抛弃 (5)"])
        settings_layout.addWidget(self.status_combo, 1, 1)
        
        # 每次获取数量
        settings_layout.addWidget(QLabel("每次获取数量:"), 2, 0)
        self.limit_spin = QSpinBox()
        self.limit_spin.setRange(1, 100)
        self.limit_spin.setValue(30)
        settings_layout.addWidget(self.limit_spin, 2, 1)
        
        # 起始位置
        settings_layout.addWidget(QLabel("起始位置:"), 3, 0)
        self.offset_spin = QSpinBox()
        self.offset_spin.setRange(0, 10000)
        self.offset_spin.setValue(0)
        settings_layout.addWidget(self.offset_spin, 3, 1)
        
        # 自动处理选项
        self.auto_process_check = QCheckBox("获取完成后自动生成Markdown文件")
        self.auto_process_check.setChecked(True)
        settings_layout.addWidget(self.auto_process_check, 4, 0, 1, 2)
        
        layout.addWidget(settings_group)
        
        # 输出说明
        output_info = QLabel("📁 生成的Markdown文件将保存到项目根目录的 output 文件夹中")
        output_info.setStyleSheet("color: #4a9eff; font-weight: bold; padding: 5px; background-color: rgba(74, 158, 255, 0.1); border-radius: 3px;")
        layout.addWidget(output_info)
        
        # 执行按钮
        batch_btn = QPushButton("🚀 开始批量导入")
        batch_btn.setStyleSheet("QPushButton { padding: 10px; font-size: 14px; }")
        batch_btn.clicked.connect(self.start_batch_import)
        layout.addWidget(batch_btn)
        
        layout.addStretch()
        self.tab_widget.addTab(tab, "📚 批量导入")
    
    def create_single_tab(self):
        """创建单个导入选项卡"""
        tab = QWidget()
        layout = QVBoxLayout(tab)
        
        # 输入区域
        input_group = QGroupBox("条目信息")
        input_layout = QVBoxLayout(input_group)
        
        input_layout.addWidget(QLabel("条目ID:"))
        self.subject_id_input = QLineEdit()
        self.subject_id_input.setPlaceholderText("请输入要处理的条目ID")
        input_layout.addWidget(self.subject_id_input)
        
        layout.addWidget(input_group)
        
        # 输出说明
        output_info = QLabel("📁 生成的Markdown文件将保存到项目根目录的 output 文件夹中")
        output_info.setStyleSheet("color: #4a9eff; font-weight: bold; padding: 5px; background-color: rgba(74, 158, 255, 0.1); border-radius: 3px;")
        layout.addWidget(output_info)
        
        # 执行按钮
        single_btn = QPushButton("📄 处理单个条目")
        single_btn.setStyleSheet("QPushButton { padding: 10px; font-size: 14px; }")
        single_btn.clicked.connect(self.start_single_import)
        layout.addWidget(single_btn)
        
        layout.addStretch()
        self.tab_widget.addTab(tab, "📄 单个导入")
    
    def create_local_tab(self):
        """创建本地处理选项卡"""
        tab = QWidget()
        layout = QVBoxLayout(tab)
        
        # 说明文本
        info_label = QLabel("处理本地 subject_ids.txt 文件中的条目ID列表")
        info_label.setStyleSheet("color: #666; font-style: italic;")
        layout.addWidget(info_label)
        
        # 文件状态显示
        self.file_status_label = QLabel()
        self.update_file_status()
        layout.addWidget(self.file_status_label)
        
        # 刷新按钮
        refresh_btn = QPushButton("🔄 刷新文件状态")
        refresh_btn.clicked.connect(self.update_file_status)
        layout.addWidget(refresh_btn)
        
        # 输出说明
        output_info = QLabel("📁 生成的Markdown文件将保存到项目根目录的 output 文件夹中")
        output_info.setStyleSheet("color: #4a9eff; font-weight: bold; padding: 5px; background-color: rgba(74, 158, 255, 0.1); border-radius: 3px;")
        layout.addWidget(output_info)
        
        # 执行按钮
        process_btn = QPushButton("⚙️ 处理本地条目列表")
        process_btn.setStyleSheet("QPushButton { padding: 10px; font-size: 14px; }")
        process_btn.clicked.connect(self.start_local_processing)
        layout.addWidget(process_btn)
        
        layout.addStretch()
        self.tab_widget.addTab(tab, "⚙️ 本地处理")
    
    def create_privacy_tab(self):
        """创建隐私设置选项卡"""
        tab = QWidget()
        layout = QVBoxLayout(tab)
        
        # 设置区域
        privacy_group = QGroupBox("隐私设置")
        privacy_layout = QVBoxLayout(privacy_group)
        
        info_label = QLabel("批量修改 subject_ids.txt 中所有条目的隐私设置")
        info_label.setStyleSheet("color: #666; font-style: italic;")
        privacy_layout.addWidget(info_label)
        
        # 隐私选项
        self.private_radio_layout = QVBoxLayout()
        
        self.public_radio = QCheckBox("设为公开")
        self.private_radio = QCheckBox("设为私密")
        self.private_radio.setChecked(True)  # 默认选择私密
        
        # 实现单选效果
        self.public_radio.toggled.connect(lambda checked: self.private_radio.setChecked(not checked) if checked else None)
        self.private_radio.toggled.connect(lambda checked: self.public_radio.setChecked(not checked) if checked else None)
        
        privacy_layout.addWidget(self.public_radio)
        privacy_layout.addWidget(self.private_radio)
        
        layout.addWidget(privacy_group)
        
        # 执行按钮
        privacy_btn = QPushButton("🔒 更新隐私设置")
        privacy_btn.setStyleSheet("QPushButton { padding: 10px; font-size: 14px; }")
        privacy_btn.clicked.connect(self.start_privacy_update)
        layout.addWidget(privacy_btn)
        
        layout.addStretch()
        self.tab_widget.addTab(tab, "🔒 隐私设置")
    
    def create_status_section(self, parent_layout):
        """创建状态显示区域"""
        # 创建水平分割窗口
        splitter = QSplitter(Qt.Orientation.Horizontal)
        
        # 左侧：状态和进度
        status_widget = QWidget()
        status_layout = QVBoxLayout(status_widget)
        
        # 当前状态
        status_layout.addWidget(QLabel("当前状态:"))
        self.status_label = QLabel("就绪")
        self.status_label.setStyleSheet("color: #4CAF50; font-weight: bold;")
        status_layout.addWidget(self.status_label)
        
        # 进度条
        self.progress_bar = QProgressBar()
        self.progress_bar.setVisible(False)
        status_layout.addWidget(self.progress_bar)
        
        # 控制按钮
        button_layout = QHBoxLayout()
        self.cancel_btn = QPushButton("❌ 取消")
        self.cancel_btn.clicked.connect(self.cancel_task)
        self.cancel_btn.setVisible(False)
        button_layout.addWidget(self.cancel_btn)
        button_layout.addStretch()
        
        status_layout.addLayout(button_layout)
        status_layout.addStretch()
        
        # 右侧：日志输出
        log_widget = QWidget()
        log_layout = QVBoxLayout(log_widget)
        
        log_layout.addWidget(QLabel("运行日志:"))
        self.log_text = QTextEdit()
        self.log_text.setMaximumHeight(150)
        self.log_text.setReadOnly(True)
        log_layout.addWidget(self.log_text)
        
        # 清空日志按钮
        clear_log_btn = QPushButton("🗑️ 清空日志")
        clear_log_btn.clicked.connect(self.log_text.clear)
        log_layout.addWidget(clear_log_btn)
        
        # 添加到分割窗口
        splitter.addWidget(status_widget)
        splitter.addWidget(log_widget)
        splitter.setSizes([300, 400])
        
        parent_layout.addWidget(splitter)
    
    def apply_dark_theme(self):
        """应用深色主题"""
        self.setStyleSheet("""
            QMainWindow {
                background-color: #2b2b2b;
                color: #ffffff;
            }
            QWidget {
                background-color: #2b2b2b;
                color: #ffffff;
            }
            QGroupBox {
                font-weight: bold;
                border: 2px solid #555;
                border-radius: 5px;
                margin-top: 1ex;
                padding-top: 10px;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 10px;
                padding: 0 5px 0 5px;
            }
            QPushButton {
                background-color: #4CAF50;
                border: none;
                color: white;
                padding: 8px 16px;
                border-radius: 4px;
                font-weight: bold;
            }
            QPushButton:hover {
                background-color: #45a049;
            }
            QPushButton:pressed {
                background-color: #3d8b40;
            }
            QLineEdit, QSpinBox, QComboBox {
                padding: 5px;
                border: 1px solid #555;
                border-radius: 3px;
                background-color: #3c3c3c;
                color: #ffffff;
            }
            QTextEdit {
                border: 1px solid #555;
                border-radius: 3px;
                background-color: #3c3c3c;
                color: #ffffff;
            }
            QTabWidget::pane {
                border: 1px solid #555;
                background-color: #2b2b2b;
            }
            QTabBar::tab {
                background-color: #404040;
                padding: 8px 16px;
                margin-right: 2px;
                border-top-left-radius: 4px;
                border-top-right-radius: 4px;
            }
            QTabBar::tab:selected {
                background-color: #4CAF50;
                color: white;
            }
            QProgressBar {
                border: 1px solid #555;
                border-radius: 3px;
                text-align: center;
            }
            QProgressBar::chunk {
                background-color: #4CAF50;
                border-radius: 2px;
            }
            QCheckBox::indicator {
                width: 13px;
                height: 13px;
            }
            QCheckBox::indicator:unchecked {
                border: 1px solid #555;
                background-color: #3c3c3c;
            }
            QCheckBox::indicator:checked {
                border: 1px solid #4CAF50;
                background-color: #4CAF50;
            }
        """)
    
    def load_credentials(self):
        """从文件加载认证信息"""
        try:
            credentials_path = get_project_root_path('credentials.txt')
            if os.path.exists(credentials_path):
                user_id, token = read_credentials('credentials.txt')
                self.user_id_input.setText(user_id)
                self.token_input.setText(token)
                self.log_text.append("✅ 从 credentials.txt 加载认证信息成功")
            else:
                self.log_text.append("⚠️ 未找到 credentials.txt 文件")
        except Exception as e:
            self.log_text.append(f"❌ 加载认证信息失败: {str(e)}")
    
    def save_credentials(self):
        """保存认证信息到文件"""
        try:
            user_id = self.user_id_input.text().strip()
            token = self.token_input.text().strip()
            
            if not user_id or not token:
                QMessageBox.warning(self, "警告", "请输入用户ID和Token")
                return
            
            credentials_path = get_project_root_path('credentials.txt')
            with open(credentials_path, 'w', encoding='utf-8') as f:
                f.write(f"{user_id}\n{token}")
            
            self.log_text.append("✅ 认证信息已保存到 credentials.txt")
            QMessageBox.information(self, "成功", "认证信息保存成功！")
        except Exception as e:
            self.log_text.append(f"❌ 保存认证信息失败: {str(e)}")
            QMessageBox.critical(self, "错误", f"保存失败: {str(e)}")
    
    def update_file_status(self):
        """更新文件状态显示"""
        subject_ids_path = get_project_root_path('subject_ids.txt')
        if os.path.exists(subject_ids_path):
            try:
                with open(subject_ids_path, 'r', encoding='utf-8') as f:
                    content = f.read().strip()
                    if content:
                        ids = content.split(',')
                        self.file_status_label.setText(f"✅ subject_ids.txt 存在，包含 {len(ids)} 个条目ID")
                        self.file_status_label.setStyleSheet("color: #4CAF50;")
                    else:
                        self.file_status_label.setText("⚠️ subject_ids.txt 存在但为空")
                        self.file_status_label.setStyleSheet("color: #FF9800;")
            except Exception as e:
                self.file_status_label.setText(f"❌ 读取文件出错: {str(e)}")
                self.file_status_label.setStyleSheet("color: #F44336;")
        else:
            self.file_status_label.setText("❌ 未找到 subject_ids.txt 文件")
            self.file_status_label.setStyleSheet("color: #F44336;")
    
    def get_auth_info(self):
        """获取认证信息"""
        user_id = self.user_id_input.text().strip()
        token = self.token_input.text().strip()
        
        if not user_id or not token:
            QMessageBox.warning(self, "警告", "请输入用户ID和Token")
            return None, None
        
        return user_id, token
    
    def start_batch_import(self):
        """开始批量导入"""
        user_id, token = self.get_auth_info()
        if not user_id or not token:
            return
        
        # 获取设置
        subject_type_text = self.subject_type_combo.currentText()
        subject_type = int(subject_type_text.split('(')[1].split(')')[0])
        
        status_text = self.status_combo.currentText()
        type_filter = None if status_text == "全部" else int(status_text.split('(')[1].split(')')[0])
        
        limit = self.limit_spin.value()
        offset = self.offset_spin.value()
        auto_process = self.auto_process_check.isChecked()
        
        # 启动工作线程
        self.worker_thread = WorkerThread(
            "fetch_all", user_id, token,
            subject_type=subject_type,
            type_filter=type_filter,
            limit=limit,
            offset=offset,
            auto_process=auto_process
        )
        
        self.start_worker_thread()
    
    def start_single_import(self):
        """开始单个导入"""
        user_id, token = self.get_auth_info()
        if not user_id or not token:
            return
        
        subject_id = self.subject_id_input.text().strip()
        if not subject_id:
            QMessageBox.warning(self, "警告", "请输入条目ID")
            return
        
        # 启动工作线程
        self.worker_thread = WorkerThread(
            "fetch_single", user_id, token,
            subject_id=subject_id
        )
        
        self.start_worker_thread()
    
    def start_local_processing(self):
        """开始本地处理"""
        user_id, token = self.get_auth_info()
        if not user_id or not token:
            return
        
        subject_ids_path = get_project_root_path('subject_ids.txt')
        if not os.path.exists(subject_ids_path):
            QMessageBox.warning(self, "警告", "未找到 subject_ids.txt 文件")
            return
        
        # 启动工作线程
        self.worker_thread = WorkerThread("process_ids", user_id, token)
        self.start_worker_thread()
    
    def start_privacy_update(self):
        """开始隐私设置更新"""
        user_id, token = self.get_auth_info()
        if not user_id or not token:
            return
        
        subject_ids_path = get_project_root_path('subject_ids.txt')
        if not os.path.exists(subject_ids_path):
            QMessageBox.warning(self, "警告", "未找到 subject_ids.txt 文件")
            return
        
        is_private = self.private_radio.isChecked()
        
        # 启动工作线程
        self.worker_thread = WorkerThread(
            "update_privacy", user_id, token,
            is_private=is_private
        )
        
        self.start_worker_thread()
    
    def start_worker_thread(self):
        """启动工作线程"""
        self.worker_thread.progress_updated.connect(self.update_progress)
        self.worker_thread.status_updated.connect(self.update_status)
        self.worker_thread.finished.connect(self.task_finished)
        self.worker_thread.log_updated.connect(self.update_log)
        
        self.worker_thread.start()
        
        # 更新UI状态
        self.progress_bar.setVisible(True)
        self.cancel_btn.setVisible(True)
        self.tab_widget.setEnabled(False)
    
    def update_progress(self, value):
        """更新进度条"""
        self.progress_bar.setValue(value)
    
    def update_status(self, status):
        """更新状态标签"""
        self.status_label.setText(status)
        self.status_label.setStyleSheet("color: #FF9800; font-weight: bold;")
    
    def update_log(self, message):
        """更新日志"""
        self.log_text.append(message)
        # 自动滚动到底部
        self.log_text.verticalScrollBar().setValue(
            self.log_text.verticalScrollBar().maximum()
        )
    
    def task_finished(self, success, message):
        """任务完成处理"""
        self.progress_bar.setVisible(False)
        self.cancel_btn.setVisible(False)
        self.tab_widget.setEnabled(True)
        
        if success:
            self.status_label.setText("任务完成")
            self.status_label.setStyleSheet("color: #4CAF50; font-weight: bold;")
            self.update_log(f"🎉 {message}")
            QMessageBox.information(self, "成功", message)
        else:
            self.status_label.setText("任务失败")
            self.status_label.setStyleSheet("color: #F44336; font-weight: bold;")
            self.update_log(f"❌ {message}")
            QMessageBox.critical(self, "错误", message)
        
        # 更新文件状态
        self.update_file_status()
    
    def cancel_task(self):
        """取消当前任务"""
        if self.worker_thread and self.worker_thread.isRunning():
            self.worker_thread.cancel()
            self.worker_thread.quit()
            self.worker_thread.wait()
            
            self.progress_bar.setVisible(False)
            self.cancel_btn.setVisible(False)
            self.tab_widget.setEnabled(True)
            
            self.status_label.setText("任务已取消")
            self.status_label.setStyleSheet("color: #FF9800; font-weight: bold;")
            self.update_log("⏹️ 用户取消了任务")


def main():
    app = QApplication(sys.argv)
    app.setApplicationName("Bangumi to Obsidian")
    
    # 设置应用图标（如果有的话）
    # app.setWindowIcon(QIcon("icon.png"))
    
    window = BangumiGUI()
    window.show()
    
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
