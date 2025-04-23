#include "work.h"

// 从本地文件读ID并导出
void readids()
{
    FILE *file = fopen("ids.txt", "r"); // 打开"ids.txt"文件，只读
    if (!file) // 如果打不开
    {
        printf("未读取到文件 'ids.txt'，请确保已执行前一步选项1且具备读取权限。\n"); // 提示用户
        printf("请按任意键返回...。"); // 提示按键返回
        getchar(); // 等待用户按键
        return menu3(); // 回到菜单3
    }
    int id; // 存读取的ID
    // 挨个读文件里的ID
    while (fscanf(file, "%d", &id) == 1)
        writeinfo(id); // 把ID的信息写到文件里
    fclose(file); // 关闭文件
    printf("已全部写入!\n"); // 告诉用户写完了
}

// 文件创建信息
// Created by 18212 on 25-4-9.