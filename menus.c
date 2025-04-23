#include "work.h"

// 主菜单函数，程序的主要界面
void menu0()
{
    int choice;    // 用来存用户输入的选项数字
    system("cls"); // 清空屏幕，让界面看起来干净
    printf("---------欢迎使用笔记生成工具---------\n");
    printf("使用前请先确保文件目录下创建了'crts.txt'文件并正确配置了用户名和token\n");
    printf("=================================\n");
    printf("1. 获取用户收藏条目ids\n");
    printf("2. 获取用户单个收藏条目\n");
    printf("3. 获取用户所有收藏条目\n");
    printf("4. 退出程序\n");
    printf("=================================\n");
    printf("请输入你的选择：");
    while (1) // 一直循环，直到用户输入正确
    {
        if (scanf("%d", &choice) != 1) // 读取用户输入的数字，检查是不是整数
        {
            printf("请输入正确的选项(1-4): "); // 如果不是数字，提示重新输入
            while (getchar() != '\n')          // 清空输入的垃圾数据，防止程序卡住
                ;
            continue; // 跳回循环开头，重新输入
        }
        else                // 如果输入是数字
            switch (choice) // 根据用户选的数字做事
            {
            case 1:      // 选1
                menu1(); // 跳转到获取收藏条目ID的菜单
                break;
            case 2:      // 选2
                menu2(); // 跳转到获取单个条目的菜单
                break;
            case 3:      // 选3
                menu3(); // 跳转到获取所有条目的菜单
                break;
            case 4:                                // 选4
                printf("正在退出程序....\n");      // 提示退出
                exit(0);                           // 退出程序
            default:                               // 输入不在1-4之间
                printf("请输入正确的选项(1-4): "); // 提示输入错误
            }
    }
}

void menu1()
{
    char c;        // 用来存用户输入的字符（y或n）
    system("cls"); // 清空屏幕
    printf("正在导出中........\n");
    get_ids(); // 调用函数去获取收藏条目ID
    printf("是否通过id进行全部导出?(y/n):\n");
    while (1) // 一直循环，直到输入正确
    {
        scanf(" %c", &c);         // 读取用户输入的字符，前面加个空格避免读取换行符
        if (c == 'y' || c == 'Y') // 如果输入是y或Y
        {
            readids(); // 读取ID并导出
            menu0();   // 回到主菜单
            break;     // 跳出循环
        }
        else if (c == 'n' || c == 'N') // 如果输入是n或N
        {
            printf("正在返回主菜单...\n"); // 提示返回
            menu0();                       // 回到主菜单
            break;                         // 跳出循环
        }
        else // 输入不是y或n
        {
            system("cls");                      // 清空屏幕
            printf("请输入正确的选项(y/n):\n"); // 提示重新输入
        }
    }
}

void menu2()
{
    char input[20];                       // 存入用户输入的ID或退出命令，最大20个字符
    system("cls");                        // 清空屏幕
    printf("请输入条目id(输入q退出):\n"); // 提示用户输入ID或q
    while (1)                             // 一直循环，直到输入正确
    {
        if (scanf("%50s", input) != 1) // 读取用户输入，检查是否成功
        {
            printf("输入无效，请重新输入: "); // 如果输入有问题，提示重新输入
            while (getchar() != '\n')         // 清空垃圾数据
                ;
            continue; // 跳回循环开头
        }

        if (input[0] == 'q' || input[0] == 'Q') // 如果输入是q或Q
        {
            printf("正在返回主菜单...\n");
            menu0(); // 回到主菜单
            break;   // 跳出循环
        }
        else // 输入不是q
        {
            int id = atoi(input); // 把输入的字符串转成整数
            if (id > 0)           // 如果转出来的数字大于0，说明是有效的ID
            {
                printf("条目id: %d\n", id); // 显示输入的ID
                writeinfo(id);              // 把这个ID的信息写到文件里
                menu2();                    // 继续在这个菜单里，让用户可以再输入
                break;                      // 跳出循环
            }
            else                                       // 如果转出来不是正数
                printf("请输入正确的id或输入q回退: "); // 提示输入有问题
        }
    }
}

void menu3()
{
    system("cls"); // 清空屏幕
    int choice;    // 存用户输入的选项
    printf("1. 从本地ids.txt中导出(需已经执行前一步选项1)\n");
    printf("2. 从服务器获取全部ids并直接导出\n");
    printf("3. 回退\n");
    printf("请输入你的选择：");
    while (1) // 一直循环，直到输入正确
    {
        if (scanf("%d", &choice) != 1) // 检查输入是不是数字
        {
            printf("输入无效，请输入数字(1-3): "); // 提示重新输入
            while (getchar() != '\n')              // 清空垃圾数据
                ;
            continue; // 跳回循环开头
        }
        switch (choice) // 根据输入做事
        {
        case 1:            // 选1
            system("cls"); // 清空屏幕
            readids();     // 从本地文件读ID并导出
            menu0();       // 回到主菜单
            break;
        case 2:            // 选2
            system("cls"); // 清空屏幕
            get_ids();     // 从服务器拿ID
            readids();     // 读ID并导出
            menu0();       // 回到主菜单
            break;
        case 3:                                // 选3
            printf("正在回退....\n");          // 提示返回
            menu0();                           // 回到主菜单
            return;                            // 退出这个函数
        default:                               // 输入不在1-3之间
            printf("请输入正确的选项(1-3): "); // 提示输入错误
        }
    }
}