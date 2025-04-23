#include "work.h"

// 定义全局变量
char auth_header[256]; // 存储身份认证头，例如 "Authorization: Bearer 用户的token"
char user_agent[256];  // 存储用户代理头，例如 "User-Agent:用户用户名/WORK"

// 申请内存函数
void init_memory(MemoryStruct *chunk)
{
    chunk->memory = malloc(1); // 分配1字节的初始内存，作为响应数据的起点
    chunk->size = 0;           // 初始化内存块大小为0
}

// 释放内存
void free_memory(MemoryStruct *chunk)
{
    free(chunk->memory); // 释放之前分配的内存块
}

// cURL 回调函数，用于处理接收到的数据
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;            // 计算本次接收到的数据总大小
    MemoryStruct *mem = (MemoryStruct *)userp; // 获取用户传入的内存结构体指针

    // 重新分配内存以容纳已有数据和新数据（加1字节用于字符串结束符）
    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) // 检查内存分配是否失败
    {
        printf("内存不足\n"); // 输出错误信息
        return 0;             // 返回0表示处理失败
    }

    // 将新数据复制到内存块的已有数据后面
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;      // 更新内存块的总大小
    mem->memory[mem->size] = 0; // 在数据末尾添加字符串结束符'\0'

    return realsize; // 返回成功处理的数据大小
}

// 通用的 HTTP GET 请求函数
char *http_get(const char *url)
{
    CURL *curl;         // cURL句柄，用于管理HTTP请求
    CURLcode res;       // cURL操作的返回值，用于检查是否成功
    MemoryStruct chunk; // 内存结构体，用于存储服务器返回的数据

    init_memory(&chunk);               // 初始化内存块以存储响应数据
    curl_global_init(CURL_GLOBAL_ALL); // 初始化cURL库的全局环境
    curl = curl_easy_init();           // 创建一个cURL句柄用于本次请求

    if (curl) // 如果cURL句柄创建成功
    {
        struct curl_slist *headers = NULL; // 定义请求头列表，初始为空
        // 添加自定义请求头
        headers = curl_slist_append(headers, "accept: application/json"); // 请求返回JSON格式数据
        headers = curl_slist_append(headers, auth_header);                // 添加身份认证信息
        headers = curl_slist_append(headers, user_agent);                 // 添加用户代理信息

        curl_easy_setopt(curl, CURLOPT_URL, url);                      // 设置请求的目标URL
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);           // 设置HTTP请求头
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback); // 指定回调函数处理响应数据
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);     // 将内存结构体传递给回调函数

        res = curl_easy_perform(curl); // 执行HTTP GET请求
        if (res != CURLE_OK)           // 如果请求失败
        {
            fprintf(stderr, "cURL 请求失败: %s\n", curl_easy_strerror(res)); // 输出具体的错误信息
            free_memory(&chunk);                                             // 释放内存
            curl_slist_free_all(headers);                                    // 释放请求头列表
            curl_easy_cleanup(curl);                                         // 清理cURL句柄
            return NULL;                                                     // 返回空指针表示失败
        }

        // 请求成功后清理资源
        curl_slist_free_all(headers); // 释放请求头列表
        curl_easy_cleanup(curl);      // 清理cURL句柄
    }

    curl_global_cleanup(); // 清理cURL库的全局环境
    return chunk.memory;   // 返回存储响应数据的内存指针
}