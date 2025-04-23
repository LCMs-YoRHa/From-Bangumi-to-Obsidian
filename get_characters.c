#include "work.h"

Character *character_head = NULL; // 全局变量，是角色链表的起点，初始为空

// 获取角色的信息
void get_characters(const int *collection_id)
{
    // 先清理旧的角色链表，防止数据重复
    while (character_head != NULL) // 如果链表里有东西
    {
        Character *temp = character_head; // 用临时指针记住当前节点
        character_head = character_head->next; // 链表头指向下一个
        free(temp); // 释放旧节点
    }

    // 拼出请求角色的网络地址
    char url[256];
    snprintf(url, sizeof(url), "https://api.bgm.tv/v0/subjects/%d/characters", collection_id); // 用条目ID填地址

    char *response = http_get(url); // 从网络拿数据
    cJSON *json = cJSON_Parse(response); // 把数据转成JSON格式，方便读取
    Character *last = NULL; // 用来记住链表最后一个节点
    int size = cJSON_GetArraySize(json); // 看看有多少个角色

    // 挨个读取JSON里的角色信息，加到链表里
    for (int i = 0; i < size; i++)
    {
        cJSON *item = cJSON_GetArrayItem(json, i); // 拿出第i个角色
        if (!item) // 如果拿不到
            continue; // 跳过这个，继续下一个

        // 创建一个新角色节点
        Character *ch = (Character *)malloc(sizeof(Character)); // 开一块内存
        memset(ch, 0, sizeof(Character)); // 清空内存，避免有乱数据
        ch->next = NULL; // 新节点的下一个指针设为空

        // 拿角色名字
        cJSON *cname = cJSON_GetObjectItem(item, "name"); // 从JSON里找"name"
        if (cJSON_IsString(cname)) // 如果是个字符串
            strncpy(ch->name, cname->valuestring, sizeof(ch->name)); // 复制到节点

        // 拿角色关系
        cJSON *crel = cJSON_GetObjectItem(item, "relation"); // 找"relation"
        if (cJSON_IsString(crel))
            strncpy(ch->relation, crel->valuestring, sizeof(ch->relation)); // 复制

        // 拿角色图片
        cJSON *images = cJSON_GetObjectItem(item, "images");
        if (cJSON_IsObject(images)) // 如果有图片信息
        {
            cJSON *grid = cJSON_GetObjectItem(images, "grid"); // 找"grid"
            if (cJSON_IsString(grid))
                strncpy(ch->char_image, grid->valuestring, sizeof(ch->char_image)); // 复制图片地址
        }

        // 拿配音演员信息
        cJSON *actors = cJSON_GetObjectItem(item, "actors"); // 找"actors"
        if (cJSON_IsArray(actors) && cJSON_GetArraySize(actors) > 0) // 如果有配音演员
        {
            cJSON *actor = cJSON_GetArrayItem(actors, 0); // 只拿第一个演员
            if (actor) // 如果有这个演员
            {
                cJSON *aname = cJSON_GetObjectItem(actor, "name"); // 找名字
                if (cJSON_IsString(aname))
                    strncpy(ch->actors[0].name, aname->valuestring, sizeof(ch->actors[0].name)); // 复制名字

                cJSON *aimg = cJSON_GetObjectItem(actor, "images"); // 找图片
                if (cJSON_IsObject(aimg))
                {
                    cJSON *agrid = cJSON_GetObjectItem(aimg, "grid"); // 找"grid"
                    if (cJSON_IsString(agrid))
                        strncpy(ch->actors[0].grid, agrid->valuestring, sizeof(ch->actors[0].grid)); // 复制图片地址
                }
                ch->has_actor = 1; // 标记这个角色有配音演员
            }
        }

        // 把新节点加到链表里
        if (character_head == NULL) // 如果链表是空的
        {
            character_head = ch; // 新节点变成头
        }
        else // 如果链表有东西
        {
            last->next = ch; // 加到最后面
        }
        last = ch; // 更新最后一个节点
    }
    // 清理JSON数据和网络返回的数据
    cJSON_Delete(json);
    free(response);
}