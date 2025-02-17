from getInfo import *

print("欢迎使用本项目")

while True:
    print("请输入条目id，输入q退出:")
    subject_id=input()
    if subject_id=="q":
        break
    write_extended_subject_data(subject_id)
    print("已生成至当前目录")