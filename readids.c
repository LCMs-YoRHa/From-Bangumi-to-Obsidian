#include "work.h"

void readids() {
    FILE *file = fopen("ids.txt", "r");
    if (file == NULL) {
        printf("无法读取文件 'ids.txt'，请确保文件存在并具备读取权限。\n");
        exit(1);
    }

    // 读取 ID
    int id;
    while (fscanf(file, "%d", &id) == 1) {
        printf("读取到 ID: %d\n", id);
    }

    fclose(file);
}

//
// Created by 18212 on 25-4-9.
//
