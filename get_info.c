#include "work.h"

// 处理嵌套字段，比如"subject.name"这样带点的
cJSON *get_nested_field(cJSON *json, const char *field_path)
{
    char path[256];
    strcpy(path, field_path);    // 复制字段路径
    char *x = strtok(path, "."); // 用点分割，拿到第一个字段
    cJSON *current = json;       // 从整个JSON开始

    while (x != NULL && current != NULL) // 只要还有字段没处理
    {
        current = cJSON_GetObjectItem(current, x); // 找当前字段
        x = strtok(NULL, ".");                     // 拿下一个字段
    }
    return current; // 返回找到的内容
}

// 拿条目的某个具体信息
char *getinfo(const int *collection_id, const char *field_path)
{
    char url[256];
    // 拼出请求地址，用用户名和条目ID
    sprintf(url, "https://api.bgm.tv/v0/users/%s/collections/%d", username, collection_id);
    char *response = http_get(url);                    // 从网络拿数据
    cJSON *json = cJSON_Parse(response);               // 解析成JSON
    cJSON *field = get_nested_field(json, field_path); // 找具体字段
    char *result = NULL;                               // 存结果

    if (field != NULL) // 如果找到字段了
    {
        if (cJSON_IsString(field))               // 如果是字符串
            result = strdup(field->valuestring); // 复制出来
        else if (cJSON_IsNumber(field))          // 如果是数字
        {
            char buffer[32];                                             // 临时存数字的字符串
            if (field->valuedouble == (double)(int)field->valuedouble)   // 检查是不是整数
                snprintf(buffer, sizeof(buffer), "%d", field->valueint); // 整数格式
            else
                snprintf(buffer, sizeof(buffer), "%f", field->valuedouble); // 浮点数格式
            result = strdup(buffer);                                        // 复制出来
        }
    }

    // 清理JSON和网络数据
    cJSON_Delete(json);
    free(response);

    // 如果有结果就返回，没结果返回"暂无"
    if (result != NULL)
        return result;
    return "暂无";
}