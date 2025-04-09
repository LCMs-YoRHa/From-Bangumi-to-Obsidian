#include <stdio.h>

#include "work.h"

int main() {

    // 这里是为了确保控制台输出不乱码
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    // 读取并生成凭证
    readcerts();
    // menu();
    // getinfo("260619", "subject.short_summary");
    writeinfo("260619");
    return 0;
}
