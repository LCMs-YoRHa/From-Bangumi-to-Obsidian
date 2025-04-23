#include "work.h"

int main()
{
    SetConsoleOutputCP(65001); // 设置控制台输出用UTF-8编码
    SetConsoleCP(65001);       // 设置控制台输入也用UTF-8编码

    readcerts(); // 读取用户名和令牌
    menu0();     // 显示主菜单
    return 0;    // 程序结束，返回0表示正常
}