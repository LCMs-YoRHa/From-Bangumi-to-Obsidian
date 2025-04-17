#include "work.h"

// 读取本地文件ids.txt中的条目id并导出
void readids()
{
    FILE *file = fopen("ids.txt", "r"); // 打开文件
    if (!file)
    { // 如果打开文件失败
        printf("未读取到文件 'ids.txt'，请确保已执行前一步选项1且具备读取权限。\n");
        printf("请按任意键返回...。");
        getchar(); // 等待用户输入任意字符后返回
        return menu3();
    }
    int id;
    // 循环读取文件内的每行数据
    while (fscanf(file, "%d", &id) == 1)
        writeinfo(id); // 调用writeinfo函数将读取到的id写入文件
    fclose(file);
    printf("已全部写入!\n");
}

//
// Created by 18212 on 25-4-9.
//
