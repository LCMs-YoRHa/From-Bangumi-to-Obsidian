// 获取收藏条目 ID 并写入文件
#include "work.h"

void get_ids()
{
    char url[256];

    // 拼接用于获取条目id的URL
    sprintf(url, "https://api.bgm.tv/v0/users/%s/collections?subject_type=2&type=2&limit=50&offset=0", username);

    // 调用API
    char *response = http_get(url); // 发送GET请求,获取收藏条目数据后赋值给response
    if (!response)
    { // 如果获取数据失败
        printf("获取id数据失败/没有条目数据\n");
        return;
    }

    // 解析JSON数据,并将其转换为cJSON对象
    cJSON *json = cJSON_Parse(response);
    // 从 JSON 对象里读取 "data" 的值
    cJSON *data = cJSON_GetObjectItem(json, "data");
    // 写入文件
    FILE *file = fopen("ids.txt", "w"); // 以写入模式打开文件,若文件不存在则创建
    cJSON *item;                        // 定义一个指针item用于遍历JSON数组
    cJSON_ArrayForEach(item, data)      // 使用cJSON_ArrayForEach函数遍历JSON数组
    {
        cJSON *subject_id = cJSON_GetObjectItem(item, "subject_id"); // 使用cJSON_GetObjectItem函数获取JSON对象中的subject_id字段
        fprintf(file, "%d\n", subject_id->valueint);                 // 将获取到的subject_id字段的值写入文件中
    }

    fclose(file);
    printf("ID 已成功导出至 ids.txt\n");

    // 释放内存和JSON数据
    cJSON_Delete(json);
    free(response);
}