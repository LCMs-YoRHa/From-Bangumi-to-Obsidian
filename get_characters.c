#include "work.h"

Character* character_head = NULL;  // 全局变量定义, 用于存储角色信息头节点

void get_characters(const int *collection_id) {
    // 清理之前的链表
    while (character_head != NULL) {
        Character* temp = character_head;   // 释放链表节点
        character_head = character_head->next;  // 移动到下一个节点
        free(temp); // 释放当前节点
    }

    // 拼接URL
    char url[256];
    snprintf(url, sizeof(url), "https://api.bgm.tv/v0/subjects/%d/characters", collection_id);      //用于获取角色信息的API

    // 发送GET请求
    char *response = http_get(url);
    if (!response) return;

    // 解析JSON数组
    cJSON *json = cJSON_Parse(response);
    if (!cJSON_IsArray(json)) return;   // 确保JSON是数组类型

    Character* last = NULL;  //链表尾结点指针
    int size = cJSON_GetArraySize(json);    // 获取数组长度
    for (int i = 0; i < size; i++) {// 遍历数组的每个元素
        cJSON *item = cJSON_GetArrayItem(json, i);//
        if (!item) continue;//跳过空项

        // 创建新节点
        Character* ch = (Character*)malloc(sizeof(Character));  // 分配内存
        memset(ch, 0, sizeof(Character));       // 清空内存
        ch->next = NULL;    // 初始化下一个指针为NULL

        // 角色名
        cJSON *cname = cJSON_GetObjectItem(item, "name");
        if (cJSON_IsString(cname)) strncpy(ch->name, cname->valuestring, sizeof(ch->name)); //如果是字符串类型，则复制到ch->name中

        // 角色关系
        cJSON *crel = cJSON_GetObjectItem(item, "relation");
        if (cJSON_IsString(crel)) strncpy(ch->relation, crel->valuestring, sizeof(ch->relation));//同理

        // 角色图片
        cJSON *images = cJSON_GetObjectItem(item, "images");
        if (cJSON_IsObject(images)) {//
            cJSON *grid = cJSON_GetObjectItem(images, "grid");  // 获取grid字段
            if (cJSON_IsString(grid)) strncpy(ch->char_image, grid->valuestring, sizeof(ch->char_image));//同理
        }

        // 配音演员
        cJSON *actors = cJSON_GetObjectItem(item, "actors");
        if (cJSON_IsArray(actors) && cJSON_GetArraySize(actors) > 0) {// 确保actors是数组类型且长度大于0
            cJSON *actor = cJSON_GetArrayItem(actors, 0);
            if (actor) {
                cJSON *aname = cJSON_GetObjectItem(actor, "name");  // 获取name字段
                if (cJSON_IsString(aname)) strncpy(ch->actors[0].name, aname->valuestring, sizeof(ch->actors[0].name));//同理

                cJSON *aimg = cJSON_GetObjectItem(actor, "images");// 获取images字段
                if (cJSON_IsObject(aimg)) {
                    cJSON *agrid = cJSON_GetObjectItem(aimg, "grid");// 获取grid字段
                    if (cJSON_IsString(agrid)) strncpy(ch->actors[0].grid, agrid->valuestring, sizeof(ch->actors[0].grid));     //同理
                }
                ch->has_actor = 1;  // 标记为有actor
            }
        }

        // 添加到链表
        if (character_head == NULL) {// 如果链表为空，则将新节点设为头结点
            character_head = ch;
        } else {// 否则，将新节点添加到链表尾部
            last->next = ch;
        }
        last = ch;// 更新链表尾结点指针
    }
//     释放JSON对象
    cJSON_Delete(json);
    free(response);
}