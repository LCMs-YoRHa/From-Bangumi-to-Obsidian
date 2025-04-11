// 获取收藏条目 ID 并写入文件
#include "work.h"

void get_ids() {
    char url[256];

    // 将格式化的url写入指定的字符数组(插入需要请求的api)
    sprintf(url, "https://api.bgm.tv/v0/users/%s/collections?subject_type=2&type=2&limit=50&offset=0", username);

    // 调用API
    char *response = http_get(url);
    if (response == NULL) {
        printf("获取id数据失败/没有条目数据\n");
        return;
    }

    // (调试用)打印 API 响应
    // printf("API 响应: %s\n", response);

    // 解析返回到的 JSON 字符串
    cJSON *json = cJSON_Parse(response);

    // 调试用
    /*if (json == NULL) {
        printf("解析 JSON 数据失败\n");
        free(response);
        return;
    }*/

    // 从 JSON 对象里读取某个字段的值
    cJSON *data = cJSON_GetObjectItem(json, "data");

    // 调试用
    /*if (!cJSON_IsArray(data)) {
    printf("JSON 数据格式错误\n");
    cJSON_Delete(json);
    free(response);
    return;
    }*/

    // 写入文件
    FILE *file = fopen("ids.txt", "w");
    if (file == NULL) {
        printf("无法创建文件 ids.txt\n");
        // 释放内存和JSON数据
        cJSON_Delete(json);
        free(response);
        return;
    }

    //遍历整个 data 数组,查找到 subject_id 并写入文件
    cJSON *item;
    cJSON_ArrayForEach(item, data) {
        cJSON *subject_id = cJSON_GetObjectItem(item, "subject_id");
        // 判断是否为数字类型
        if (cJSON_IsNumber(subject_id)) {
            // 获取整数值写入文件
            fprintf(file, "%d\n", subject_id->valueint);
        }
    }

    fclose(file);
    printf("ID 已成功导出至 ids.txt\n");

    // 释放内存和JSON数据
    cJSON_Delete(json);
    free(response);
}