#include "work.h"

// 过滤非法字符
void error(char *filename)
{
    char *error_chars = "\\/:*?\"<>|"; // 列出常见的文件名中的非法字符
    while (*filename)                  // 遍历文件名字符串
    {
        if (strchr(error_chars, *filename)) // 如果当前字符是非法字符，则替换为下划线
            *filename = '_';
        filename++; // 指针操作,移动到下一个字符
    }
}

// 获取文件名
char *creatfile(const int *collection_id)
{
    char filename[256];
    const char *final_name = "unknown";                        // 规定默认文件名为unknown
    char *name_cn = getinfo(collection_id, "subject.name_cn"); // 优先获取中文名
    // 如果中文名为空，则使用原名, 否则使用中文名
    if (strlen(name_cn) == 0)
        final_name = getinfo(collection_id, "subject.name");
    else
        final_name = getinfo(collection_id, "subject.name_cn");

    sprintf(filename, "%s.md", final_name); // 创建最终文件名
    error(filename);                        // 过滤非法字符
    return strdup(filename);                // 返回动态动态内存分配的字符串
}

// 开始写入信息
void writeinfo(const int *collection_id)
{
    CreateDirectory("Output", NULL);// 创建输出目录,如果目录已存在则忽略错误
    char *filename = creatfile(collection_id); // 调用creatfile函数获取文件名
    // 创建完整文件路径
    char filepath[300];
    sprintf(filepath, "Output/%s", filename);

    // 转换为宽字符，否则会出现乱码
    wchar_t wpath[300];
    MultiByteToWideChar(CP_UTF8, 0, filepath, -1, wpath, 300);

    // 使用_wfopen以宽字符模式打开文件
    FILE *file = _wfopen(wpath, L"wb");
    printf("正在写入: %s\n", filepath);

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
    get_characters(collection_id); // 获取角色信息
    // 写入出演角色表格(使用HTML代码渲染)
    fprintf(file, "<table style=\"table-layout: fixed; width: 100%%;\">\n"); // 设置表格样式(HTML)
    Character *current = character_head;                                           // 获取角色链表头结点
    int cell_count = 0;

    while (current != NULL)
    {                                      // 遍历角色链表
        if (cell_count % 3 == 0)           // 每行开始
            fprintf(file, "<tr>\n"); // 开始新行

        fprintf(file, "<td style=\"width: 33.33%%; text-align: center; vertical-align: top;\">\n"); // 设置单元格样式(HTML)

        // 写入角色信息
        fprintf(file, "%s<br>%s<br>配音: %s\n",
                current->relation,                                      // 角色关系
                current->name,                                          // 角色名
                current->has_actor ? current->actors[0].name : "暂无"); // 如果有配音，则显示配音角色名,否则显示暂无

        // 写入角色图像
        if (strlen(current->char_image) > 0)
        {
            fprintf(file, "<br><img src=\"%s\" style=\"max-width: 100px; max-height: 100px;\">\n", // 设置图像样式(HTML)
                    current->char_image);                                                          // 写入角色图像
        }

        // 写入配音图像
        if (current->has_actor && strlen(current->actors[0].grid) > 0)
        {
            fprintf(file, "<br><img src=\"%s\" style=\"max-width: 100px; max-height: 100px;\">\n", // 设置图像样式(HTML)
                    current->actors[0].grid);                                                      // 写入配音图像
        }

        fprintf(file, "</td>\n"); // 结束单元格

        if ((cell_count + 1) % 3 == 0) // 如果列数是3的倍数，结束行
            fprintf(file, "</tr>\n");

        current = current->next; // 链表移动到下一个节点(角色)
        cell_count++;            // 列数加1
    }

    // 如果列数不是3的倍数则补全行
    if (cell_count % 3 != 0)
    {
        for (int i = cell_count % 3; i < 3; i++)
        {                                       // 补全表格
            fprintf(file, "<td></td>\n"); // 添加空单元格
        }
        fprintf(file, "</tr>\n");         // 结束行
    }

    fprintf(file, "</table>\n\n");                                                         // 结束表格
    fprintf(file, "\n![](%s)\n", getinfo(collection_id, "subject.images.large")); // 写入封面

    // 关闭文件
    fclose(file);
    printf("文件 %s 已成功创建\n", filename);
    free(filename);
}

//
// Created by 18212 on 25-4-9.
//
