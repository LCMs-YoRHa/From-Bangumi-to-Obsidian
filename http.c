#include "work.h"

// 定义全局变量
char auth_header[256];
char user_agent[256];

// 简单的回调函数,用于处理接收到的数据,并将其存储到内存中
/**
 * 回调函数 - 处理 HTTP 响应数据
 * @param ptr      接收到的数据块指针
 * @param size     每个数据项的大小
 * @param nmemb    数据项的数量
 * @param userdata 用户提供的数据指针（这里是响应缓冲区）
 * @return         实际处理的数据大小
 */
size_t write_data(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t realsize = size * nmemb;
    char *response = (char *)userdata;

    // 简单复制数据到缓冲区
    memcpy(response, ptr, realsize);
    response[realsize] = '\0';  // 确保字符串以NULL结尾

    return realsize;
}

// 通用 HTTP GET 请求
char *http_get(const char *url)
{
    CURL *curl;//定义curl类型的变量curl,用于存储curl句柄
    CURLcode res;//定义curlcode类型的变量res,用于存储curl请求的返回值
    char *response = malloc(1024 * 1024); // 定义一个固定大小的缓冲区,用于存储响应数据

    if (!response)
        return NULL;

    // 清空缓冲区
    memset(response, 0, 1024 * 1024);//初始化缓冲区

    curl_global_init(CURL_GLOBAL_ALL);//初始化curl全局环境
    curl = curl_easy_init();//初始化curl，将返回的句柄赋值给curl

    if (curl)//如果curl初始化成功
    {
        struct curl_slist *headers = NULL;      //定义头部列表
        headers = curl_slist_append(headers, "accept: application/json");      //添加头部
        headers = curl_slist_append(headers, auth_header);//添加身份认证头部
        headers = curl_slist_append(headers, user_agent);//添加用户代理头部

        curl_easy_setopt(curl, CURLOPT_URL, url);//设置请求的URL
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);//设置请求头部
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);//设置回调函数
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);//设置回调函数参数

        res = curl_easy_perform(curl);//将请求发送到服务器并等待响应，将结果存储在res中
        if (res != CURLE_OK)        //如果请求失败
        {
            free(response);               //释放内存
            curl_slist_free_all(headers);//释放头部列表
            curl_easy_cleanup(curl);     //清理curl资源
            curl_global_cleanup();      //清理全局curl资源
            return NULL;
        }

        curl_slist_free_all(headers);   //释放头部列表
        curl_easy_cleanup(curl);        //清理curl资源
    }
    else//如果curl初始化失败
    {
        free(response);//释放内存
        curl_global_cleanup();//清理全局curl资源
        return NULL;          //返回NULL
    }

    curl_global_cleanup();      // 清理全局curl资源
    return response;
}