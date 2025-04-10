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
    char *name_cn = getinfo(collection_id, "subject.name_cn");
    char *name = NULL;
    char *summary = NULL;

    if (name_cn == NULL || strlen(name_cn) == 0) {
        free(name_cn);
        name = getinfo(collection_id, "subject.name");
    }

    // 生成文件名
    snprintf(filename, sizeof(filename), "%s.md", name_cn ? name_cn : (name ? name : "unknown"));
    sanitize_filename(filename);

    // 获取 short_summary
    summary = getinfo(collection_id, "subject.short_summary");

    // 调试信息：打印 summary 值
    if (summary) {
        printf("获取到的 summary: %s\n", summary);
    } else {
        printf("未能获取到 summary 的值\n");
    }

    // 写入文件
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("无法创建文件 %s\n", filename);
        free(name_cn);
        free(name);
        free(summary);
        return;
    }

    // 设置文件为 UTF-8 编码
    fprintf(file, "\xEF\xBB\xBF"); // 写入 UTF-8 BOM

    // 写入内容
    if (summary) {
        fprintf(file, "# 简介\n\n%s\n", summary);
    } else {
        fprintf(file, "# 简介\n\n暂无简介\n");
    }

    fclose(file);

    printf("文件 %s 已成功创建\n", filename);

    free(name_cn);
    free(name);
    free(summary);
}

//
// Created by 18212 on 25-4-9.
//
