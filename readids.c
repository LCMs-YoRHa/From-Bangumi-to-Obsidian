#include "work.h"

void readids() {
    FILE *file = fopen("ids.txt", "r");
    if (file == NULL) {
        printf("无法读取文件 'ids.txt'，请确保文件存在并具备读取权限。\n");
        return;
    }

    // 读取读取 ID并导出条目
    int id;
    char id_str[20];
    while (fscanf(file, "%d", &id) == 1) {
        sprintf(id_str, "%d", id);  // 将整数转换为字符串
        writeinfo(id_str);
    }
    fclose(file);
}

//
// Created by 18212 on 25-4-9.
//
