#include "work.h"

// 主菜单
void menu()
{
    int choice;
    system("cls");
    printf("---------欢迎使用笔记生成工具---------\n");
    printf("使用前请先确保文件目录下创建了'crts.txt'文件并正确配置了用户名和token\n");
    printf("=================================\n");
    printf("1. 获取用户收藏条目ids\n");
    printf("2. 获取用户单个收藏条目\n");
    printf("3. 获取用户所有收藏条目\n");
    printf("4. 退出程序\n");
    printf("=================================\n");
    printf("请输入你的选择：");
    while (1)
    {
        if (scanf("%d", &choice) != 1) {
            printf("请输入正确的选项(1-4): ");
            while (getchar() != '\n') // 清空输入缓冲区
                continue;
        }
        // getchar(); // 清除换行符
        else
            switch (choice)
            {
                case 1:
                    menu1();
                    break;
                case 2:
                    menu2();
                    break;
                case 3:
                    menu3();
                    break;
                case 4:
                    printf("正在退出程序....\n");
                    exit(0);
                default:
                    printf("请输入正确的选项(1-4): ");
            }
    }
}

void menu1()
{
    char c;
    system("cls");    //清屏
    printf("正在导出中........\n");
    printf("是否通过id进行全部导出?(y/n):\n");
    while (1)
    {
        if (scanf(" %c", &c) != 1) {
            printf("输入无效，请输入y或n:\n");
            while (getchar() != '\n') // 清空输入缓冲区
                continue;
        }
        if (c == 'y' || c == 'Y')
        {
            // getinfo();
            break;
        }
        else if (c == 'n' || c == 'N')
        {
            printf("正在返回主菜单...\n");
            menu();
            break;
        }
        else
        {
            printf("请输入正确的选项(y/n):\n");
        }
    }
}

void menu2()
{
    char input[100];
    system("cls");
    printf("请输入条目id(输入q退出):\n");
    while (1)
    {
        if (scanf("%99s", input) != 1) {
            printf("输入无效，请重新输入: ");
            while (getchar() != '\n'); // 清空输入缓冲区
            continue;
        }

        if (input[0] == 'q' || input[0] == 'Q')
        {
            printf("正在返回主菜单...\n");
            menu();
            break;
        }
        else
        {
            int id = atoi(input);    //将字符串转换为整数
            if (id > 0) // 检查是否为有效的正整数
            {
                printf("条目id: %d\n", id);
                // writeinfo(id);
                menu2();
                break;
            }
            else
            {
                printf("请输入正确的id或输入q回退: ");
            }
        }
    }
}

void menu3()
{
    system("cls");
    int choice;
    printf("1. 从本地ids.txt中导出(需已经执行前一步选项1)\n");
    printf("2. 从服务器获取全部ids并直接导出\n");
    printf("3. 回退\n");
    printf("请输入你的选择：");
    while (1)
    {
        if (scanf("%d", &choice) != 1) {
            printf("输入无效，请输入数字(1-3): ");
            while (getchar() != '\n'); // 清空输入缓冲区
            continue;
        }
        getchar(); // 清除换行符

        switch (choice)
        {
            case 1:
                // getinfo();
                break;
            case 2:
                // get_ids();
                // getinfo();
                break;
            case 3:
                printf("正在回退....\n");
                menu();
                return;
            default:
                printf("请输入正确的选项(1-3): ");
        }
    }
}



//
// Created by 18212 on 25-4-9.
//
