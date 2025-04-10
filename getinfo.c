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

    char *response = http_get(url);
    if (response == NULL) {
        printf("获取收藏条目信息失败\n");
        return NULL;
    }

    cJSON *json = cJSON_Parse(response);
    free(response);
    if (json == NULL) {
        printf("解析 JSON 数据失败\n");
        return NULL;
    }

    cJSON *field = get_nested_field(json, field_path);
    char *result = NULL;
    if (field != NULL && cJSON_IsString(field)) {
        result = strdup(field->valuestring); // 复制字段值
    } else {
        printf("字段 %s 不存在或类型错误\n", field_path);
    }

    cJSON_Delete(json);
    return result;
}


//
// Created by 18212 on 25-4-9.
//
