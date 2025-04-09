#include "work.h"

// 获取收藏条目 ID 并写入文件
void get_ids() {
    char url[256];
    snprintf(url, sizeof(url), "https://api.bgm.tv/v0/users/%s/collections?subject_type=2&type=2&limit=50&offset=0", username);

    char *response = http_get(url);
    if (response == NULL) {
        printf("获取id数据失败\n");
        return;
    }

    // 打印 API 响应
    printf("API 响应: %s\n", response);

    cJSON *json = cJSON_Parse(response);
    if (json == NULL) {
        printf("解析 JSON 数据失败\n");
        free(response);
        return;
    }

    cJSON *data = cJSON_GetObjectItem(json, "data");
    if (!cJSON_IsArray(data)) {
        printf("JSON 数据格式错误\n");
        cJSON_Delete(json);
        free(response);
        return;
    }

    FILE *file = fopen("ids.txt", "w");
    if (file == NULL) {
        printf("无法打开文件 ids.txt\n");
        cJSON_Delete(json);
        free(response);
        return;
    }

    cJSON *item;
    cJSON_ArrayForEach(item, data) {
        cJSON *subject_id = cJSON_GetObjectItem(item, "subject_id");
        if (cJSON_IsNumber(subject_id)) {
            fprintf(file, "%d\n", subject_id->valueint);
        }
    }

    fclose(file);
    printf("ID 已成功写入 ids.txt\n");

    cJSON_Delete(json);
    free(response);
}