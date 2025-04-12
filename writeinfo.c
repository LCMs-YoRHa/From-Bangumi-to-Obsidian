#include "work.h"

// 过滤非法字符
void sanitize_filename(char *filename)
{
    char *illegal_chars = "\\/:*?\"<>|";
    while (*filename)
    {
        if (strchr(illegal_chars, *filename))
        {
            *filename = '_';
        }
        filename++;
    }
}

// 获取文件名
char *creatfile(const int *collection_id)
{
    char filename[256];
    const char *final_name = "unknown";
    char *name_cn = getinfo(collection_id, "subject.name_cn");

    if (strlen(name_cn) == 0)
    {

        final_name = getinfo(collection_id, "subject.name");
    }
    else
    {
        final_name = getinfo(collection_id, "subject.name_cn");
    }

    snprintf(filename, sizeof(filename), "%s.md", final_name);
    sanitize_filename(filename);
    return strdup(filename);
}

// 开始写入信息
void writeinfo(const int *collection_id)
{

    // 获取文件名
    char *filename = creatfile(collection_id);

    // 转换为宽字符, 否则会出现乱码
    int len = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
    wchar_t wname[256];
    MultiByteToWideChar(CP_UTF8, 0, filename, -1, wname, len);

    // 使用宽字符版本的 fopen,否则会出现乱码
    FILE *file = _wfopen(wname, L"wb");
    if (file == NULL)
    {
        printf("无法创建文件 %s\n", filename);
        free(filename);
        return;
    }
    else
    {
        printf("正在写入:%s\n", filename);
    }

    // 写入简介
    fprintf(file, "# 简介\n\n");
    fprintf(file, "\n---\n");
    fprintf(file, "\n原名:%s\n", getinfo(collection_id, "subject.name"));
    fprintf(file, "\n话数:%s\n", getinfo(collection_id, "subject.eps"));
    fprintf(file, "\n版本:%s\n", getinfo(collection_id, "subject.subject_type"));
    fprintf(file, "\n收录评分:%s\n", getinfo(collection_id, "subject.score"));
    fprintf(file, "\n放送开始:%s\n", getinfo(collection_id, "subject.date"));
    fprintf(file, "\n收藏日期:%s\n", getinfo(collection_id, "updated_at"));
    fprintf(file, "\n封面链接:%s\n", getinfo(collection_id, "subject.images.large"));
    fprintf(file, "\n---\n");
    fprintf(file, "\n## 剧情梗概:\n%s\n", getinfo(collection_id, "subject.short_summary"));
    fprintf(file, "\n---\n");
    fprintf(file, "\n![](%s)\n", getinfo(collection_id, "subject.images.large"));

    // 关闭文件
    fclose(file);
    printf("文件 %s 已成功创建\n", filename);
    free(filename);
}

//
// Created by 18212 on 25-4-9.
//
