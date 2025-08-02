#!/usr/bin/env python3
"""
Bangumi to Obsidian GUI版本启动脚本
"""

import sys
import subprocess
import importlib.util

def check_package(package_name):
    """检查包是否已安装"""
    return importlib.util.find_spec(package_name) is not None

def install_package(package_name):
    """安装包"""
    try:
        subprocess.check_call([sys.executable, '-m', 'pip', 'install', package_name])
        return True
    except subprocess.CalledProcessError:
        return False

def check_requirements():
    """检查并安装必要的依赖"""
    required_packages = {
        'PyQt6': 'PyQt6',
        'requests': 'requests'
    }
    
    missing_packages = []
    
    for package_name, pip_name in required_packages.items():
        if not check_package(package_name):
            missing_packages.append(pip_name)
    
    if missing_packages:
        print("检测到缺少以下依赖包:")
        for package in missing_packages:
            print(f"  - {package}")
        
        response = input("\n是否自动安装这些依赖包? (y/n): ").lower().strip()
        if response in ['y', 'yes']:
            for package in missing_packages:
                print(f"\n正在安装 {package}...")
                if install_package(package):
                    print(f"✅ {package} 安装成功")
                else:
                    print(f"❌ {package} 安装失败")
                    return False
        else:
            print("请手动安装依赖包后再运行程序")
            print("安装命令: pip install PyQt6 requests")
            return False
    
    return True

def main():
    print("=" * 40)
    print("   Bangumi to Obsidian GUI版本")
    print("=" * 40)
    print()
    
    print("正在检查依赖...")
    if not check_requirements():
        input("按回车键退出...")
        return
    
    print("✅ 依赖检查完成，启动GUI...")
    
    try:
        # 添加src目录到Python路径
        import sys
        import os
        src_path = os.path.join(os.path.dirname(__file__), 'src')
        if src_path not in sys.path:
            sys.path.insert(0, src_path)
        
        # 导入并运行GUI
        from gui_main import main as gui_main
        gui_main()
    except ImportError as e:
        print(f"❌ 导入GUI模块失败: {e}")
        print("请确保 src/gui_main.py 文件存在")
        input("按回车键退出...")
    except Exception as e:
        print(f"❌ 程序运行出错: {e}")
        input("按回车键退出...")

if __name__ == "__main__":
    main()
