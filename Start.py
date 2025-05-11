from getInfo import *
import Bangumi
import requests

print("欢迎使用本项目")
print("请确保当前目录下存在credentials.txt文件，其中第一行为user_id,第二行为token")

# 读取用户user_id和token
user_id, token = read_credentials('credentials.txt')

print("请选择功能:")
print("1.导入用户个人全部收藏条目")
print("2.导入用户个人单个收藏条目")
print("3.根据本地subject_ids.txt文件逐个写入条目")
print("4.修改全部条目隐私设置")
choice = input("请输入数字选择功能:")

if choice == "1":
    print("1为书籍 2为动画 3为音乐 4为游戏 6为三次元")
    subject_type = input("请输入数字选择条目类型(默认为动画):") or 2
    print("1为在看 2为看过 3为想看 4为搁置 5为抛弃")
    type = input("请输入数字选择条目状态(默认为全部):") or None
    print("请输入每次获取的数量:")
    limit = int(input())
    print("请输入起始位置:")
    offset = int(input())
    print("开始获取数据.........")
    fetch_and_write_all_subject_ids(user_id, token, subject_type, type, limit, offset)
    print("数据获取完成")
    if input("是否开始逐个写入条目?(y/n):") == "y":
        print("开始逐个写入文件.........")
        process_subject_ids(user_id, token)
        print("文件写入完成")

elif choice == "2":
    while True:
        subject_id = input("请输入条目id,输入q退出:")
        if subject_id == "q":
            break
        print("开始写入文件.........")
        write_extended_subject_data(user_id, subject_id, token)
        print("文件写入完成")

elif choice == "3":
    print("开始写入文件.........")
    process_subject_ids(user_id, token)
    print("文件写入完成")

elif choice == "4":
    print("开始读取ids.txt中的条目ID...")
    with open('subject_ids.txt', 'r', encoding='utf-8') as f:
        subject_ids = f.read().split(',')
    privacy_choice = input("是否将所有条目设置为私密?(y/n):")
    is_private = True if privacy_choice.lower() == "y" else False
    for count, subject_id in enumerate(subject_ids, start=1):
        try:
            Bangumi.update_collection_privacy(subject_id.strip(), token, is_private, user_id)
            print(f"第{count}个条目 {subject_id} 的隐私设置已更新为 {'私密' if is_private else '公开'}")
        except requests.exceptions.RequestException as e:
            print(f"条目 {subject_id} 更新失败: {e}")
    print("所有条目隐私设置更新完成。")
