#include "work.h"

Character characters[100];  // 全局变量定义
int count = 0;

void get_characters(const int *collection_id) {
    count = 0; // 初始化角色数为0

    // 获取 subject.id
    char *subject_id = getinfo(collection_id, "subject.id");
    if (strcmp(subject_id, "暂无") == 0) return;

    // 拼接URL
    char url[256];
    snprintf(url, sizeof(url), "https://api.bgm.tv/v0/subjects/%s/characters", subject_id);

    // 发送GET请求
    char *response = http_get(url);
    if (!response) return;

    // 解析JSON数组
    cJSON *json = cJSON_Parse(response);
    if (!cJSON_IsArray(json)) return;

    int size = cJSON_GetArraySize(json);
    for (int i = 0; i < size && count < 100; i++) {
        cJSON *item = cJSON_GetArrayItem(json, i);
        if (!item) continue;

        Character *ch = &characters[count];
        memset(ch, 0, sizeof(Character));

        // 角色名
        cJSON *cname = cJSON_GetObjectItem(item, "name");
        if (cJSON_IsString(cname)) strncpy(ch->name, cname->valuestring, sizeof(ch->name));

        // 角色关系
        cJSON *crel = cJSON_GetObjectItem(item, "relation");
        if (cJSON_IsString(crel)) strncpy(ch->relation, crel->valuestring, sizeof(ch->relation));

        // 角色图片
        cJSON *images = cJSON_GetObjectItem(item, "images");
        if (cJSON_IsObject(images)) {
            cJSON *grid = cJSON_GetObjectItem(images, "grid");
            if (cJSON_IsString(grid)) strncpy(ch->char_image, grid->valuestring, sizeof(ch->char_image));
        }

        // 配音演员
        cJSON *actors = cJSON_GetObjectItem(item, "actors");
        if (cJSON_IsArray(actors) && cJSON_GetArraySize(actors) > 0) {
            cJSON *actor = cJSON_GetArrayItem(actors, 0);
            if (actor) {
                cJSON *aname = cJSON_GetObjectItem(actor, "name");
                if (cJSON_IsString(aname)) strncpy(ch->actors[0].name, aname->valuestring, sizeof(ch->actors[0].name));

                cJSON *aimg = cJSON_GetObjectItem(actor, "images");
                if (cJSON_IsObject(aimg)) {
                    cJSON *agrid = cJSON_GetObjectItem(aimg, "grid");
                    if (cJSON_IsString(agrid)) strncpy(ch->actors[0].grid, agrid->valuestring, sizeof(ch->actors[0].grid));
                }
                ch->has_actor = 1;
            }
        }

        count++;
    }

    cJSON_Delete(json);
}





//
// Created by 18212 on 25-4-15.
//
