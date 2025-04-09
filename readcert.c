#include "work.h"

// 定义全局变量
char username[50];
char token[100];

void readcerts() {
    FILE *file = fopen("certs.txt", "r");
    if (file == NULL) {
        printf("无法读取文件 'certs.txt'，请确保文件存在并具备读取权限。\n");
        exit(1);
    }

    // 读取 username 和 token
    if (fscanf(file, "username=%49s\ntoken=%99s", username, token) != 2) {
        printf("文件格式错误，请确保 'certs.txt' 文件内容格式为：\n");
        printf("username=你的用户名\ntoken=你的token\n");
        fclose(file);
        exit(1);
    }

    fclose(file);

    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", token);
    snprintf(user_agent, sizeof(user_agent), "User-Agent: %s/WORK", username);
}


//
// Created by 18212 on 25-4-9.
//
