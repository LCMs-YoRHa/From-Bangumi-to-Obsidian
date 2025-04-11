#include "work.h"

void readids() {
    FILE *file = fopen("ids.txt", "r");
    if (file == NULL) {
        printf("未读取到文件 'ids.txt'，请确保已执行前一步选项1且具备读取权限。\n");
        printf("请按任意键返回...。");
        getchar();
        return menu3();
    }

    // 读取读取 ID并导出条目
    int id;
    // char id_str[20];
    while (fscanf(file, "%d", &id) == 1) {
        // sprintf(id_str, "%d", id);  // 将整数转换为字符串
        writeinfo(id);
    }
    printf("已全部写入!\n");
    fclose(file);
}

//
// Created by 18212 on 25-4-9.
//
