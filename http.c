#include "work.h"

// 定义全局变量
char auth_header[256];
char user_agent[256];

// 申请内存函数
void init_memory(MemoryStruct *chunk)
{
    chunk->memory = malloc(1); // malloc函数分配内存
    chunk->size = 0;           // 初始化大小
}

// 释放内存
void free_memory(MemoryStruct *chunk)
{
    free(chunk->memory); // 释放内存
}

// cURL 回调
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;            // 计算实际大小
    MemoryStruct *mem = (MemoryStruct *)userp; // 获取定义的内存结构体

    mem->memory = realloc(mem->memory, mem->size + realsize + 1); // 重新分配内存
    memcpy(&(mem->memory[mem->size]), contents, realsize);        // 将内容复制到内存中
    mem->size += realsize;                                        // 更新内存大小
    mem->memory[mem->size] = 0;                                   // 设置字符串结尾为0

    return realsize; // 返回实际大小
}

// 通用 HTTP GET 请求
char *http_get(const char *url)
{
    CURL *curl;         // 声明cURL句柄
    CURLcode res;       // 声明cURL返回值
    MemoryStruct chunk; // 声明内存结构体

    init_memory(&chunk);               // 申请内存
    curl_global_init(CURL_GLOBAL_ALL); // 初始化cURL库
    curl = curl_easy_init();           // 初始化cURL句柄

    // 构造请求头
    if (curl)
    {
        struct curl_slist *headers = NULL; // 声明请求头
        headers = curl_slist_append(headers, "accept: application/json");
        headers = curl_slist_append(headers, auth_header);
        headers = curl_slist_append(headers, user_agent);

        curl_easy_setopt(curl, CURLOPT_URL, url);                      // 设置URL
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);           // 设置请求头
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback); // 设置回调函数和用户数据
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        res = curl_easy_perform(curl); // 发送请求
        if (res != CURLE_OK)           //
        {
            fprintf(stderr, "cURL 请求失败: %s\n", curl_easy_strerror(res));
            free_memory(&chunk);
            curl_slist_free_all(headers); // 释放请求头
            curl_easy_cleanup(curl);      // 清理cURL
            return NULL;
        }

        // 释放内存和清理cURL
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    // 结束curl
    curl_global_cleanup();
    return chunk.memory;
}

//
// Created by 18212 on 25-4-9.
//
