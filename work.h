#ifndef WORK_H
#define WORK_H

// 导入头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <cjson/cJSON.h>
#include <curl/curl.h>

// 定义一个结构体用于存储内存数据
typedef struct
{
    char *memory; // 指向一个内存块的指针
    size_t size;  // 记录 memory 指向的内存块的大小
} MemoryStruct;

typedef struct
{
    char name[100]; // 配音演员姓名
    char grid[200]; // 图像地址
} Actor;

typedef struct Character
{
    char relation[100];     // 角色关系
    char name[100];         // 角色姓名
    char char_image[200];   // 角色图片地址
    Actor actors[1];        // 简化处理，只取第一个配音演员
    int has_actor;          // 定义有无配音演员
    struct Character *next; // 指向下一个节点的指针
} Character;

// 全局变量声明
extern char username[50];         // 用户的用户名
extern char token[100];           // 用户的token
extern char auth_header[256];     // 构造的请求头
extern char user_agent[256];      // 构造的请求头
extern Character *character_head; // 角色链表头指针
extern int count;                 // 角色数量计数

// 菜单函数
void menu0();                     //主菜单
void menu1();
void menu2();
void menu3();

// 文件操作函数
void readcerts();                           //读取用户凭证
void readids();                             //读取条目id
char *creatfile(const int *collection_id);  //获取、判断、生成文件名
void writeinfo(const int *collection_id);   //写入条目信息

// 通过api获取数据的函数
void get_ids();                                                         //获取收藏条目id
char *getinfo(const int *collection_id, const char *field_path);        //获取收藏条目特定信息
void get_characters(const int *collection_id);                          //获取角色信息

// 用于http请求的函数
char *http_get(const char *url);

#endif

// WORK_H

//
// Created by 18212 on 25-4-9.
//
