开发环境:Clion
Cmake
编译器:mingw64
运行环境:Windows

使用的第三方库:CJson,curl

main.c(主函数)

work.h(头文件)

menus.c(菜单)

readcert.c(读取用户凭证,生成请求头)

test.c(测试使用)

get_ids.c(从服务器通过调用api获取条目id并写入ids.txt)

http.c (http处理请求的一系列函数)

readids.cpp (读取用户id,准备逐条请求api查询)

getinfo.c(获取特定id的特定信息)

CMakeLists(运行Cmake所用的配置文件)