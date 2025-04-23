#include "work.h"

// 把文件名里的非法字符换成下划线
void error(char *filename)
{
    char *error_chars = "\\/:*?\"<>|"; // 这些是Windows文件名里不能用的字符
    while (*filename)                  // 挨个检查文件名里的每个字符
    {
        if (strchr(error_chars, *filename)) // 如果这个字符是非法的
            *filename = '_';                // 换成下划线
        filename++;                         // 看下一个字符
    }
}

// 帮我们生成一个文件名
char *creatfile(const int *collection_id)
{
    char filename[256];                                        // 存文件名的字符串
    const char *final_name = "unknown";                        // 默认文件名是"unknown"
    char *name_cn = getinfo(collection_id, "subject.name_cn"); // 先试着拿中文名
    // 检查中文名有没有，如果没有就用原名
    if (strlen(name_cn) == 0)
        final_name = getinfo(collection_id, "subject.name"); // 用原名
    else
        final_name = getinfo(collection_id, "subject.name_cn"); // 用中文名

    sprintf(filename, "%s.md", final_name); // 把名字加上".md"，做成Markdown文件名
    error(filename);                        // 检查并替换非法字符
    return strdup(filename);                // 返回一份新的文件名字符串
}

// 把条目信息写到文件里
void writeinfo(const int *collection_id)
{
    CreateDirectory("Output", NULL);           // 创建一个"Output"文件夹，放输出的文件
    char *filename = creatfile(collection_id); // 调用函数生成文件名
    // 拼出完整的文件路径
    char filepath[300];
    sprintf(filepath, "Output/%s", filename);

    // 把路径转成宽字符，防止中文乱码
    wchar_t wpath[300];
    MultiByteToWideChar(CP_UTF8, 0, filepath, -1, wpath, 300);

    // 用宽字符模式打开文件，准备写入
    FILE *file = _wfopen(wpath, L"wb");
    printf("正在写入: %s\n", filepath); // 告诉用户正在写哪个文件

    // 开始写文件内容
    fprintf(file, "# 简介\n\n");                                                            // 一级标题
    fprintf(file, "\n---\n");                                                               // 分隔线
    fprintf(file, "\n原名:%s\n", getinfo(collection_id, "subject.name"));                   // 写原名
    fprintf(file, "\n话数:%s\n", getinfo(collection_id, "subject.eps"));                    // 写话数
    fprintf(file, "\n版本:%s\n", getinfo(collection_id, "subject.subject_type"));           // 写版本
    fprintf(file, "\n收录评分:%s\n", getinfo(collection_id, "subject.score"));              // 写评分
    fprintf(file, "\n放送开始:%s\n", getinfo(collection_id, "subject.date"));               // 写开始时间
    fprintf(file, "\n收藏日期:%s\n", getinfo(collection_id, "updated_at"));                 // 写收藏时间
    fprintf(file, "\n封面链接:%s\n", getinfo(collection_id, "subject.images.large"));       // 写封面地址
    fprintf(file, "\n---\n");                                                               // 分隔线
    fprintf(file, "\n## 剧情梗概:\n%s\n", getinfo(collection_id, "subject.short_summary")); // 写剧情简介
    fprintf(file, "\n---\n");                                                               // 分隔线
    fprintf(file, "\n## 出演角色:\n");                                                      // 角色信息二级标题
    get_characters(collection_id);                                                          // 获取角色信息
    // 用HTML表格展示角色信息
    fprintf(file, "<table style=\"table-layout: fixed; width: 100%%;\">\n"); // 开始表格，设置样式
    Character *current = character_head;                                     // 从角色链表的头开始
    int cell_count = 0;                                                      // 计数器，记录写了几个格子

    while (current != NULL) // 挨个处理链表里的角色
    {
        if (cell_count % 3 == 0)     // 每3个格子换一行
            fprintf(file, "<tr>\n"); // 开始新行

        fprintf(file, "<td style=\"width: 33.33%%; text-align: center; vertical-align: top;\">\n"); // 开始一个格子，设置样式

        // 写角色信息
        fprintf(file, "%s<br>%s<br>配音: %s\n",
                current->relation,                                      // 角色关系
                current->name,                                          // 角色名字
                current->has_actor ? current->actors[0].name : "暂无"); // 有配音就写名字，没有就写"暂无"

        // 如果有角色图片，就写进去
        if (strlen(current->char_image) > 0)
        {
            fprintf(file, "<br><img src=\"%s\" style=\"max-width: 100px; max-height: 100px;\">\n", // 加图片标签
                    current->char_image);                                                          // 图片地址
        }

        // 如果有配音演员的图片，也写进去
        if (current->has_actor && strlen(current->actors[0].grid) > 0)
        {
            fprintf(file, "<br><img src=\"%s\" style=\"max-width: 100px; max-height: 100px;\">\n", // 加图片标签
                    current->actors[0].grid);                                                      // 配音演员图片地址
        }

        fprintf(file, "</td>\n"); // 结束这个格子

        if ((cell_count + 1) % 3 == 0) // 如果写了3个格子
            fprintf(file, "</tr>\n");  // 结束这行

        current = current->next; // 移动到下一个角色
        cell_count++;            // 格子数加1
    }

    // 如果格子数不是3的倍数，补齐空格子
    if (cell_count % 3 != 0)
    {
        for (int i = cell_count % 3; i < 3; i++) // 补上缺少的格子
        {
            fprintf(file, "<td></td>\n"); // 空格子
        }
        fprintf(file, "</tr>\n"); // 结束行
    }

    fprintf(file, "</table>\n\n");                                                // 结束表格
    fprintf(file, "\n![](%s)\n", getinfo(collection_id, "subject.images.large")); // 写封面图片

    // 文件写完了，关闭并释放内存
    fclose(file);
    printf("文件 %s 已成功创建\n", filename); // 告诉用户文件写好了
    free(filename);                           // 释放文件名内存
}