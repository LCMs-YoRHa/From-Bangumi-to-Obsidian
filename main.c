#include "work.h"

int main() {

    // 这里是为了确保通过控制台的输出输入不乱码,通过调用 Windows API 函数强制将控制台输出输入设置为 UTF-8(65501) 编码
    SetConsoleOutputCP(65001);    //输出
    SetConsoleCP(65001);          //输入

    // 读取并生成凭证
    readcerts();

    // 调用菜单,执行程序功能
    menu();
    return 0;
}
