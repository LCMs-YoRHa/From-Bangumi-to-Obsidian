#ifndef WORK_H
#define WORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

// 全局变量声明
extern char username[50];
extern char token[100];
extern char auth_header[256];
extern char user_agent[256];

// 定义一个结构体用于存储内存数据
typedef struct {
    char *memory;   // 指向一个内存块的指针
    size_t size;    //记录 memory 指向的内存块的大小
} MemoryStruct;

// 菜单函数
void menu();
void menu1();
void menu2();
void menu3();

// 文件操作函数
void readcerts();
void readids();
char *creatfile(const int *collection_id);
void writeinfo(const int *collection_id);

// 通过api获取数据的函数
void get_ids();
char *getinfo(const int *collection_id, const char *field_path);

// http请求的一系列函数
void init_memory(MemoryStruct *chunk);
void free_memory(MemoryStruct *chunk);
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);
char *http_get(const char *url);

#endif//WORK_H

//
// Created by 18212 on 25-4-9.
//

