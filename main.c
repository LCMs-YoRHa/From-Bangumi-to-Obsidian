#include <stdio.h>

#include "work.h"

int main() {

    // 这里是为了确保控制台输出不乱码,强制将控制台输出设置为UTF-8编码
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    // 读取并生成凭证
    readcerts();
    readids();
    // menu();
    // getinfo("260619", "subject.short_summary");
    // writeinfo("383308");
    return 0;
}
