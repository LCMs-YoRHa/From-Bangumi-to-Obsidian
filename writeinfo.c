#include "work.h"

// 过滤非法字符
void sanitize_filename(char *filename) {
    char *illegal_chars = "\\/:*?\"<>|";
    while (*filename) {
        if (strchr(illegal_chars, *filename)) {
            *filename = '_';
        }
        filename++;
    }
}

void writeinfo(const char *collection_id) {
    char filename[256];
    char name_cn[256] = {0};
    char name[256] = {0};
    char summary[1024] = {0};

    // 获取 name_cn 或 name 作为文件名
    getinfo(collection_id, "subject.name_cn");
    strncpy(name_cn, "subject.name_cn", sizeof(name_cn) - 1);
    if (strlen(name_cn) == 0) {
        getinfo(collection_id, "subject.name");
        strncpy(name, "subject.name", sizeof(name) - 1);
    }

    // 生成文件名
    snprintf(filename, sizeof(filename), "%s.md", name_cn[0] ? name_cn : name);
    sanitize_filename(filename);

    // 获取 short_summary
    getinfo(collection_id, "subject.short_summary");
    strncpy(summary, "subject.short_summary", sizeof(summary) - 1);

    // 写入文件
    FILE *file = fopen(filename, "w, ccs=UTF-8");
    if (file == NULL) {
        printf("无法创建文件 %s\n", filename);
        return;
    }

    fprintf(file, "# 简介\n\n%s\n", summary);
    fclose(file);

    printf("文件 %s 已成功创建\n", filename);
}

//
// Created by 18212 on 25-4-9.
//
