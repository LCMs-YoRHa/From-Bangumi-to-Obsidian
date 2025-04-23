#ifndef WORK_H // 防止头文件被重复包含
#define WORK_H

// 导入一些常用的工具库
#include <stdio.h> // 输入输出
#include <stdlib.h> // 内存管理
#include <string.h> // 字符串操作
#include <windows.h> // Windows系统功能
#include <cjson/cJSON.h> // 解析JSON数据
#include <curl/curl.h> // 网络请求

// 定义一个结构，存内存数据
typedef struct
{
    char *memory; // 指向内存的指针
    size_t size;  // 内存大小
} MemoryStruct;

// 定义一个结构，存配音演员信息
typedef struct
{
    char name[100]; // 演员名字
    char grid[200]; // 图片地址
} Actor;

// 定义一个结构，存角色信息
typedef struct Character
{
    char relation[100];     // 角色关系
    char name[100];         // 角色名字
    char char_image[200];   // 角色图片地址
    Actor actors[1];        // 只存一个配音演员
    int has_actor;          // 标记有没有配音演员
    struct Character *next; // 指向下一个角色
} Character;

// 全局变量，程序各处都能用
extern char username[50];         // 用户名
extern char token[100];           // 令牌
extern char auth_header[256];     // 身份认证头
extern char user_agent[256];      // 用户代理头
extern Character *character_head; // 角色链表头
extern int count;                 // 角色数量

// 菜单函数
void menu0();                     // 主菜单
void menu1();                     // 子菜单1
void menu2();                     // 子菜单2
void menu3();                     // 子菜单3

// 文件操作函数
void readcerts();                           // 读凭证
void readids();                             // 读ID
char *creatfile(const int *collection_id);  // 生成文件名
void writeinfo(const int *collection_id);   // 写信息

// 从网络拿数据的函数
void get_ids();                                                         // 拿收藏ID
char *getinfo(const int *collection_id, const char *field_path);        // 拿具体信息
void get_characters(const int *collection_id);                          // 拿角色信息

// 网络请求函数
char *http_get(const char *url); // 通用网络请求

#endif // 结束定义

// 文件创建信息
// Created by 18212 on 25-4-9.