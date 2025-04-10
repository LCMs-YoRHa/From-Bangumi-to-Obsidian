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
    // 创建要写入的各项变量
    char *filename = creatfile(collection_id);
    char *summary = getinfo(collection_id, "subject.short_summary");
    printf("生成的文件名: %s\n", filename);
    printf("获取到的简介: %s\n", summary ? summary : "NULL");

    // 转换为宽字符
    int len = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
    wchar_t *wname = (wchar_t*)malloc(len * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, filename, -1, wname, len);

    // 创建文件
    HANDLE file = CreateFileW(wname, GENERIC_WRITE, 0, NULL,
                            CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (file == INVALID_HANDLE_VALUE) {
        printf("无法创建文件 %s (错误码: %lu)\n", filename, GetLastError());
        free(summary);
        free(filename);
        free(wname);
        return;
    }

    // UTF-8 BOM
    const char bom[] = "\xEF\xBB\xBF";
    DWORD bytes;
    WriteFile(file, bom, sizeof(bom) - 1, &bytes, NULL);

    // 写入标题
    const char header[] = "# 简介\n\n";
    WriteFile(file, header, sizeof(header) - 1, &bytes, NULL);

    // 写入简介
    if (summary) {
        WriteFile(file, summary, strlen(summary), &bytes, NULL);
        WriteFile(file, "\n", 1, &bytes, NULL);
    } else {
        const char noSummary[] = "暂无简介\n";
        WriteFile(file, noSummary, sizeof(noSummary) - 1, &bytes, NULL);
    }

    CloseHandle(file);

    printf("文件 %s 已成功创建\n", filename);
    free(summary);
    free(filename);
    free(wname);
}

//
// Created by 18212 on 25-4-9.
//
