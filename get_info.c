#include "work.h"

// 定义一个函数用于处理当getinfo中的field_path参数为嵌套字段时的特殊情况(xxx.xxx.xxx)
cJSON *get_nested_field(cJSON *json, const char *field_path)
{
    char path[256];
    strcpy(path, field_path);    // 复制字段路径到一个临时变量path中
    char *x = strtok(path, "."); // 以"."分割字符串,获取第一个字段
    cJSON *current = json;       // 首先将当前JSON对象完整赋值给current

    while (x != NULL && current != NULL) // 如果x不为空切current不为空
    {
        current = cJSON_GetObjectItem(current, x); // 使用cJSON_GetObjectItem函数获取当前JSON对象中的x字段
        x = strtok(NULL, ".");                     // 获取"."后面的下一个字段
    }
    return current; // 返回最终获取到的JSON对象
}

// 获取收藏条目的特定信息并返回字段值
char *getinfo(const int *collection_id, const char *field_path)
{
    char url[256];
    // 拼接用于获取条目信息的URL
    sprintf(url, "https://api.bgm.tv/v0/users/%s/collections/%d", username, collection_id);
    char *response = http_get(url);                    // 发送请求,获取收藏条目数据后赋值给response
    cJSON *json = cJSON_Parse(response);               // 解析JSON数据,将解析后的结果赋值给json
    cJSON *field = get_nested_field(json, field_path); // 调用上面的get_nested_field函数处理嵌套字段,并将结果赋值给field
    char *result = NULL;                               // 定义一个指针result用于存储字段值

    if (field != NULL)
    {                                            // 如果获取到的字段不为空,开始判断字段类型
        if (cJSON_IsString(field))               // 若为字符串类型
            result = strdup(field->valuestring); // 使用strdup函数复制字符串,并将结果赋值给result
        else if (cJSON_IsNumber(field))          // 若为数字类型
        {
            char buffer[32];                                           // 首先定义一个字符数组buffer用于存储数字字符串
            if (field->valuedouble == (double)(int)field->valuedouble) // 使用valuedouble判断是否为整数
                // 若为整数
                snprintf(buffer, sizeof(buffer), "%d", field->valueint);
            else
                // 若为浮点数
                snprintf(buffer, sizeof(buffer), "%f", field->valuedouble);
            result = strdup(buffer); // 将数字字符串复制到result中
        }
    }

    // 若字段值不为空，则返回字段值，否则返回"暂无"
    if (result != NULL)
        return result;
    return "暂无";
}

//
// Created by 18212 on 25-4-9.
//
