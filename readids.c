#include "work.h"

void readids()
{
    FILE *file = fopen("ids.txt", "r"); // 打开文件
    if (!file)
    {
        printf("未读取到文件 'ids.txt'，请确保已执行前一步选项1且具备读取权限。\n");
        printf("请按任意键返回...。");
        getchar();
        return menu3();
    }

    // 读取读取 ID并导出条目
    int id;
    while (fscanf(file, "%d", &id) == 1)
        writeinfo(id);
    fclose(file);
    printf("已全部写入!\n");
}

//
// Created by 18212 on 25-4-9.
//
