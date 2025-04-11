#include "work.h"

// 递归解析嵌套字段
cJSON *get_nested_field(cJSON *json, const char *field_path) {
    char path[256];
    strncpy(path, field_path, sizeof(path) - 1);
    path[sizeof(path) - 1] = '\0';

    char *token = strtok(path, ".");
    cJSON *current = json;

    while (token != NULL && current != NULL) {
        current = cJSON_GetObjectItem(current, token);
        token = strtok(NULL, ".");
    }

    return current;
}

// 获取收藏条目信息并返回字段值
char *getinfo(const char *collection_id, const char *field_path) {
    char url[256];
    snprintf(url, sizeof(url), "https://api.bgm.tv/v0/users/%s/collections/%s", username, collection_id);

    // 调试用
    char *response = http_get(url);
    // if (response == NULL) {
    //     printf("获取收藏条目信息失败\n");
    //     return NULL;
    // }
    //
    cJSON *json = cJSON_Parse(response);
    // free(response);
    // if (json == NULL) {
    //     printf("解析 JSON 数据失败\n");
    //     return NULL;
    // }

    cJSON *field = get_nested_field(json, field_path);
    char *result = NULL;

    if (field != NULL) {
        if (cJSON_IsString(field)) {
            result = strdup(field->valuestring); // 复制字符串值
        } else if (cJSON_IsNumber(field)) {
            // 处理数字类型
            char buffer[32];
            if (field->valuedouble == (double)(int)field->valuedouble) {
                // 整数
                snprintf(buffer, sizeof(buffer), "%d", field->valueint);
            } else {
                // 浮点数
                snprintf(buffer, sizeof(buffer), "%f", field->valuedouble);
            }
            result = strdup(buffer);
        }
    }

    // 释放JSON内存
    cJSON_Delete(json);
    if (result != NULL)
        return result;
    return "暂无";
}


//
// Created by 18212 on 25-4-9.
//
