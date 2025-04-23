// 获取收藏条目ID并写入文件
#include "work.h"

void get_ids()
{
    char url[256];
    // 拼出请求收藏ID的地址，用用户名
    sprintf(url, "https://api.bgm.tv/v0/users/%s/collections?subject_type=2&type=2&limit=50&offset=0", username);

    // 从网络拿数据
    char *response = http_get(url);
    if (!response) // 如果没拿到数据
    {
        printf("获取id数据失败/没有条目数据\n"); // 提示失败
        return;                                  // 退出函数
    }

    // 解析数据成JSON
    cJSON *json = cJSON_Parse(response);
    cJSON *data = cJSON_GetObjectItem(json, "data"); // 找"data"部分
    // 打开文件准备写入
    FILE *file = fopen("ids.txt", "w"); // 以写模式打开，覆盖旧内容
    cJSON *item;                        // 用来遍历数据
    cJSON_ArrayForEach(item, data)      // 挨个看data里的东西
    {
        cJSON *subject_id = cJSON_GetObjectItem(item, "subject_id"); // 找ID
        fprintf(file, "%d\n", subject_id->valueint);                 // 写到文件，每行一个
    }

    fclose(file);                        // 关闭文件
    printf("ID 已成功导出至 ids.txt\n"); // 告诉用户写好了

    // 清理数据
    cJSON_Delete(json);
    free(response);
}