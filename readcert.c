#include "work.h"

// 定义全局变量
char username[50];
char token[100];

void readcerts()
{
    FILE *file = fopen("certs.txt", "r"); // 打开文件,并处理错误
    if (!file)
    {
        printf("无法读取文件 'certs.txt'，请确保文件存在并具备读取权限。\n");
        exit(1);
    }

    // 读取并检查内容是否正确
    if (fscanf(file, "username=%49s\ntoken=%99s", username, token) != 2)
    { // 如果两个内容有任何一个不正确，则输出错误信息并退出程序
        printf("文件格式错误，请确保 'certs.txt' 文件内容格式为：\n");
        printf("username=你的用户名\ntoken=你的token\n");
        fclose(file);
        exit(1);
    }

    fclose(file);

    // 构造请求头中的身份认证和ua
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", token);
    snprintf(user_agent, sizeof(user_agent), "User-Agent: %s/WORK", username);
}

//
// Created by 18212 on 25-4-9.
//
