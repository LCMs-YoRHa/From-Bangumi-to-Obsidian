#include "work.h"

char auth_header[256];
char user_agent[256];

// 申请内存
void init_memory(MemoryStruct *chunk) {
    chunk->memory = malloc(1);
    chunk->size = 0;
}

// 释放内存
void free_memory(MemoryStruct *chunk) {
    free(chunk->memory);
}

// cURL 回调
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    MemoryStruct *mem = (MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL) {
        printf("内存分配失败\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

// 通用 HTTP GET 请求
char *http_get(const char *url) {
    CURL *curl;
    CURLcode res;
    MemoryStruct chunk;

    init_memory(&chunk);    //初始化内存
    curl_global_init(CURL_GLOBAL_ALL);      // 初始化全局变量
    curl = curl_easy_init();    // 初始化cURL句柄

    // 设置请求头
    if (curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "accept: application/json");
        headers = curl_slist_append(headers, auth_header);
        headers = curl_slist_append(headers, user_agent);

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        // 发送请求
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "cURL 请求失败: %s\n", curl_easy_strerror(res));
            free_memory(&chunk);
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
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
