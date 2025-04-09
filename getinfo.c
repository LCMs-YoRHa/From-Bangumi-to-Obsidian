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

// 获取收藏条目信息
void getinfo(const char *collection_id, const char *field_path) {
    char url[256];
    snprintf(url, sizeof(url), "https://api.bgm.tv/v0/users/%s/collections/%s", username, collection_id);

    char *response = http_get(url);
    if (response == NULL) {
        printf("获取收藏条目信息失败\n");
        return;
    }

    // 打印 API 响应（调试用）
    printf("API 响应: %s\n", response);

    cJSON *json = cJSON_Parse(response);
    if (json == NULL) {
        printf("解析 JSON 数据失败\n");
        free(response);
        return;
    }

    // 获取嵌套字段
    cJSON *field = get_nested_field(json, field_path);
    if (field != NULL) {
        if (cJSON_IsString(field)) {
            printf("%s: %s\n", field_path, field->valuestring);
        } else if (cJSON_IsNumber(field)) {
            printf("%s: %lf\n", field_path, field->valuedouble);
        } else {
            printf("%s: [无法直接打印的类型]\n", field_path);
        }
    } else {
        printf("字段 %s 不存在\n", field_path);
    }

    cJSON_Delete(json);
    free(response);
}


//
// Created by 18212 on 25-4-9.
//
