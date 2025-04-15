#include "work.h"

// 递归解析嵌套的字段(xxx.xxx)
cJSON *get_nested_field(cJSON *json, const char *field_path)
{
    char path[256];
    strncpy(path, field_path, sizeof(path) - 1);
    // 确保字符串以空字符结尾
    path[sizeof(path) - 1] = '\0';

    char *token = strtok(path, ".");
    cJSON *current = json;

    while (token != NULL && current != NULL)
    {
        current = cJSON_GetObjectItem(current, token);
        token = strtok(NULL, ".");
    }
    return current;
}

// 获取收藏条目信息并返回字段值
char *getinfo(const int *collection_id, const char *field_path)
{
    char url[256];
    // 将格式化的url写入指定的字符数组(插入需要请求的api)
    sprintf(url, "https://api.bgm.tv/v0/users/%s/collections/%d", username, collection_id);
    // 调用http_get函数获取收藏条目信息
    char *response = http_get(url);
    // 解析JSON数据
    cJSON *json = cJSON_Parse(response);
    // 获取字段值
    cJSON *field = get_nested_field(json, field_path);
    char *result = NULL;

    if (field != NULL)
    {
        if (cJSON_IsString(field))
            result = strdup(field->valuestring); // 复制字符串值
        else if (cJSON_IsNumber(field))
        {
            // 处理数字类型
            char buffer[32];
            if (field->valuedouble == (double)(int)field->valuedouble)
                // 整数
                snprintf(buffer, sizeof(buffer), "%d", field->valueint);
            else
                // 浮点数
                snprintf(buffer, sizeof(buffer), "%f", field->valuedouble);
            result = strdup(buffer);
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
