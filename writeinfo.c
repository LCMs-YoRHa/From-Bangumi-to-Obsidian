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

// 获取文件名
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

// 开始写入信息
void writeinfo(const char *collection_id) {
    // 创建要写入的各项变量
    char *filename = creatfile(collection_id);
    // 打印调试信息,判断BUg
    // printf("生成的文件名: %s\n", filename);
    // printf("获取到的简介: %s\n", summary ? summary : "NULL");
    // printf("获取到的集数: %s\n", eps ? eps : "NULL");
    // printf("获取到的url: %s\n", image_url ? image_url : "NULL");

    // 转换为宽字符
    int len = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
    wchar_t wname[256];
    MultiByteToWideChar(CP_UTF8, 0, filename, -1, wname, len);

    // 使用宽字符版本的 fopen
    FILE *file = _wfopen(wname, L"wb");
    if (file == NULL) {
        printf("无法创建文件 %s\n", filename);
        free(filename);
        return;
    }else {
        printf("正在写入:%s\n", filename);
    }

    // UTF-8 BOM
    const char bom[] = "\xEF\xBB\xBF";
    fwrite(bom, 1, 3, file);

    // 写入标题
    fprintf(file, "# 简介\n\n");

    // 写入简介
    fprintf(file, "\n---\n");
    fprintf(file, "\n原名:%s\n",getinfo(collection_id,"subject.name"));
    fprintf(file, "\n话数:%s\n",getinfo(collection_id,"subject.eps"));
    fprintf(file, "\n版本:%s\n",getinfo(collection_id,"subject.subject_type"));
    fprintf(file, "\n收录评分:%s\n",getinfo(collection_id,"subject.score"));
    fprintf(file, "\n放送开始:%s\n",getinfo(collection_id,"subject.date"));
    fprintf(file, "\n收藏日期:%s\n",getinfo(collection_id,"updated_at"));
    fprintf(file, "\n封面链接:%s\n", getinfo(collection_id,"subject.images.large"));
    fprintf(file, "\n---\n");
    fprintf(file, "\n## 剧情梗概:\n%s\n", getinfo(collection_id, "subject.short_summary"));
    fprintf(file, "\n---\n");
    fprintf(file, "\n![](%s)\n", getinfo(collection_id, "subject.images.large"));
    fclose(file);
    printf("文件 %s 已成功创建\n", filename);
    free(filename);
}

//
// Created by 18212 on 25-4-9.
//
