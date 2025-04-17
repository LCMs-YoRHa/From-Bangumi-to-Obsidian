#include "work.h"

Character *character_head = NULL; // 全局变量定义, 用于存储角色信息头节点

// 获取角色信息的函数
void get_characters(const int *collection_id)
{
    // 首先清理之前的链表,确保不会因为链表未释放而导致重复添加数据
    while (character_head != NULL) // 如果链表不为空,则:
    {
        Character *temp = character_head;      // 将当前头指针赋值给临时指针
        character_head = character_head->next; // 为了下一步的释放,先将头指针指向下一个节点
        free(temp);                            // 释放当前节点
    }

    // 拼接用于获取角色信息的URL
    char url[256];
    snprintf(url, sizeof(url), "https://api.bgm.tv/v0/subjects/%d/characters", collection_id); // 将用于获取角色信息的API插入到url中

    char *response = http_get(url);      // 发送GET请求,获取角色信息数据后赋值给response
    cJSON *json = cJSON_Parse(response); // 调用cJSON库解析JSON数据,将解析后的结果赋值给json
    Character *last = NULL;              // 定义一个指针last,用于遍历链表
    int size = cJSON_GetArraySize(json); // 获取JSON数组的大小,即角色数量

    // 遍历JSON数组,将每个角色信息添加到链表中
    for (int i = 0; i < size; i++)
    {
        cJSON *item = cJSON_GetArrayItem(json, i); // 获取JSON数组中的每个元素
        if (!item)
            continue; // 如果获取失败,则跳过,继续下一个元素

        // 创建新节点
        Character *ch = (Character *)malloc(sizeof(Character)); // 使用malloc动态分配内存,创建一个新的角色节点
        memset(ch, 0, sizeof(Character));                       // 首先将内存清空,避免未初始化的内存导致错误
        ch->next = NULL;                                        // 初始化下一个节点指针为NULL

        // 角色名
        cJSON *cname = cJSON_GetObjectItem(item, "name");            // 使用cJSON_GetObjectItem函数获取JSON对象中的name字段
        if (cJSON_IsString(cname))                                   // 判断是否为字符串类型:
            strncpy(ch->name, cname->valuestring, sizeof(ch->name)); // 将获取到的字符串复制到角色节点的name字段中

        // 角色关系
        cJSON *crel = cJSON_GetObjectItem(item, "relation"); // 获取relation字段
        if (cJSON_IsString(crel))
            strncpy(ch->relation, crel->valuestring, sizeof(ch->relation)); // 将获取到的字符串复制到角色节点的relation字段中

        // 角色图片
        cJSON *images = cJSON_GetObjectItem(item, "images");
        if (cJSON_IsObject(images)) // 判断是否为数组类型
        {
            cJSON *grid = cJSON_GetObjectItem(images, "grid"); // 获取grid字段
            if (cJSON_IsString(grid))
                strncpy(ch->char_image, grid->valuestring, sizeof(ch->char_image)); // 将获取到的字符串复制到角色节点的char_image字段中
        }

        // 配音演员
        cJSON *actors = cJSON_GetObjectItem(item, "actors");         // 获取actors字段
        if (cJSON_IsArray(actors) && cJSON_GetArraySize(actors) > 0) // 如果actors是数组类型且长度大于0
        {
            cJSON *actor = cJSON_GetArrayItem(actors, 0); // 获取第一个配音演员
            if (actor)                                    // 若actor不为空
            {
                cJSON *aname = cJSON_GetObjectItem(actor, "name"); // 获取name字段
                if (cJSON_IsString(aname))
                    strncpy(ch->actors[0].name, aname->valuestring, sizeof(ch->actors[0].name)); // 将获取到的字符串复制到角色节点的配音演员姓名字段中

                cJSON *aimg = cJSON_GetObjectItem(actor, "images"); // 获取images字段
                if (cJSON_IsObject(aimg))
                {
                    cJSON *agrid = cJSON_GetObjectItem(aimg, "grid"); // 获取grid字段
                    if (cJSON_IsString(agrid))
                        strncpy(ch->actors[0].grid, agrid->valuestring, sizeof(ch->actors[0].grid)); // 将获取到的字符串复制到角色节点的配音演员图片地址字段中
                }
                ch->has_actor = 1; // 标记为有actor
            }
        }

        // 如果没有配音演员,则将has_actor设为0
        if (character_head == NULL)
        {                        // 如果链表为空
            character_head = ch; // 将新节点设为头结点,为了避免每次都从头开始遍历
        }
        else
        { // 否则，将新节点添加到链表尾部,为了避免每次都从头开始遍历
            last->next = ch;
        }
        last = ch; // 更新链表尾结点指针
    }
    //     释放JSON对象
    cJSON_Delete(json);
    free(response);
}