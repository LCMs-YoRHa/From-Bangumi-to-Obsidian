#include "work.h"

int main()
{

    SetConsoleOutputCP(65001); // 设置控制台输出为 UTF-8 编码
    SetConsoleCP(65001);       // 设置控制台输入为 UTF-8 编码

    readcerts(); // 读取并生成凭证
    menu0();     // 主菜单
    return 0;
}
