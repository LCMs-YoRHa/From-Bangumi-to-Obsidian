#include "work.h"

// 定义全局变量
char username[50]; // 用来保存用户名的字符串，最大长度50个字符
char token[100];   // 用来保存令牌（token）的字符串，最大长度100个字符

void readcerts()
{
    FILE *file = fopen("certs.txt", "r"); // 打开名叫"certs.txt"的文件，只读模式
    if (!file) // 如果文件没打开成功（可能是文件不存在或没权限）
    {
        printf("无法读取文件 'certs.txt'，请确保文件存在并具备读取权限。\n"); // 告诉用户文件打不开
        exit(1); // 退出程序，返回1表示有错误
    }

    // 读取文件里的内容，检查格式是否正确
    if (fscanf(file, "username=%49s\ntoken=%99s", username, token) != 2) // 从文件读两行，第一行是username=后面接用户名，第二行是token=后面接令牌
    { // 如果读到的不是正好两条信息（比如格式不对）
        printf("文件格式错误，请确保 'certs.txt' 文件内容格式为：\n"); // 提示用户文件内容有问题
        printf("username=你的用户名\ntoken=你的token\n"); // 展示正确的格式例子
        fclose(file); // 关闭文件
        exit(1); // 退出程序，返回1表示有错误
    }

    fclose(file); // 文件用完了，关闭它

    // 用读到的用户名和令牌，拼出HTTP请求需要的两行头信息
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", token); // 拼出身份认证头，比如"Authorization: Bearer 你的token"
    snprintf(user_agent, sizeof(user_agent), "User-Agent: %s/WORK", username); // 拼出用户代理头，比如"User-Agent: 你的用户名/WORK"
}

// 文件创建信息，记录作者和时间
// Created by 18212 on 25-4-9.