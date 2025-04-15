#include "work.h"

// 过滤非法字符
void sanitize_filename(char *filename)
{
    char *illegal_chars = "\\/:*?\"<>|";    // 列出常见的文件名中的非法字符
    while (*filename)
    {
        if (strchr(illegal_chars, *filename))   // 如果当前字符是非法字符，则替换为下划线
            *filename = '_';
        filename++;
    }
}

// 获取文件名
char *creatfile(const int *collection_id)
{
    char filename[256];
    const char *final_name = "unknown";// 默认文件名为unknown
    char *name_cn = getinfo(collection_id, "subject.name_cn"); // 获取中文名
    // 如果中文名为空，则使用原名, 否则使用中文名
    if (strlen(name_cn) == 0)
        final_name = getinfo(collection_id, "subject.name");
    else
        final_name = getinfo(collection_id, "subject.name_cn");

    snprintf(filename, sizeof(filename), "%s.md", final_name);// 创建最终文件名
    sanitize_filename(filename);// 过滤非法字符
    return strdup(filename);//返回动态动态内存分配的字符串
}

// 开始写入信息
void writeinfo(const int *collection_id)
{

    // 获取文件名
    char *filename = creatfile(collection_id);

    // 转换为宽字符, 否则会出现乱码
    int len = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
    wchar_t wname[256];// 创建宽字符数组
    MultiByteToWideChar(CP_UTF8, 0, filename, -1, wname, len);// 将宽字符数组转换为多字节字符数组

    // 使用宽字符版本的 fopen,否则会出现乱码
    FILE *file = _wfopen(wname, L"wb");
    if (file == NULL)
    {
        printf("无法创建文件 %s\n", filename);
        free(filename);
        return;
    }
    else
        printf("正在写入:%s\n", filename);

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
    fprintf(file, "\n## 出演角色:\n");
    get_characters(collection_id);  // <== 获取角色信息
    // 写入出演角色表格
    fprintf(file, "<table style=\"table-layout: fixed; width: 100%%;\">\n");
    Character *current = character_head;
    int cell_count = 0;

    while (current != NULL) {
        if (cell_count % 3 == 0)
            fprintf(file, "<tr>\n");

        fprintf(file, "<td style=\"width: 33.33%%; text-align: center; vertical-align: top;\">\n");

        // 写入角色信息
        fprintf(file, "%s<br>%s<br>配音: %s\n",
                current->relation,
                current->name,
                current->has_actor ? current->actors[0].name : "暂无");

        // 写入角色图像
        if (strlen(current->char_image) > 0) {
            fprintf(file, "<br><img src=\"%s\" style=\"max-width: 100px; max-height: 100px;\">\n",
                    current->char_image);
        }

        // 写入配音图像
        if (current->has_actor && strlen(current->actors[0].grid) > 0) {
            fprintf(file, "<br><img src=\"%s\" style=\"max-width: 100px; max-height: 100px;\">\n",
                    current->actors[0].grid);
        }

        fprintf(file, "</td>\n");

        if ((cell_count + 1) % 3 == 0)
            fprintf(file, "</tr>\n");

        current = current->next;
        cell_count++;
    }

    // 如果列数不是3的倍数，补全表格
    if (cell_count % 3 != 0) {
        for (int i = cell_count % 3; i < 3; i++) {
            fprintf(file, "<td></td>\n");
        }
        fprintf(file, "</tr>\n");
    }

    fprintf(file, "</table>\n\n");
    fprintf(file, "\n![](%s)\n", getinfo(collection_id, "subject.images.large"));

    // 关闭文件
    fclose(file);
    printf("文件 %s 已成功创建\n", filename);
    free(filename);
}

//
// Created by 18212 on 25-4-9.
//
