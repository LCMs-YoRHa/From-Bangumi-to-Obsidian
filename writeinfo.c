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

char *creatfile(const char *collection_id) {
    char filename[256];
    char *name_cn = getinfo(collection_id, "subject.name_cn");
    char *name = NULL;

    // 修复逻辑：仅在 name_cn 无效时获取 name
    if (name_cn == NULL || strlen(name_cn) == 0) {
        free(name_cn);  // 允许 name_cn 为 NULL 时调用 free
        name_cn = NULL; // 显式置空避免悬垂指针
        name = getinfo(collection_id, "subject.name");
    }

    // 安全处理空指针
    const char *final_name = "unknown";
    if (name_cn) final_name = name_cn;
    else if (name) final_name = name;

    snprintf(filename, sizeof(filename), "%s.md", final_name);
    sanitize_filename(filename);

    free(name_cn);
    free(name);  // name 可能为 NULL，但 free(NULL) 是安全的

    return strdup(filename);
}

void writeinfo(const char *collection_id) {

    // 创建要写入的各项变量,同时通过api获取对应值的值进行赋值
    char *filename = creatfile(collection_id);
    char *summary = getinfo(collection_id, "subject.short_summary");
    printf("生成的文件名: %s\n", filename);
    printf("获取到的简介: %s\n", summary ? summary : "NULL");
    // char *

    // 开发期间用于测试：打印 summary 值,检测是否成功获取
    // if (summary) {
    //     printf("获取到的 summary: %s\n", summary);
    // } else {
    //     printf("未能获取到 summary 的值\n");
    // }

    // 写入文件


    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("无法创建文件 %s\n", filename);
        free(summary);
        free(filename);
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
    free(summary);
    free(filename);
}

//
// Created by 18212 on 25-4-9.
//
