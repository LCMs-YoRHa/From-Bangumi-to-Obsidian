cmake_minimum_required(VERSION 3.30)
project(WORK C)

set(CMAKE_C_STANDARD 11)
set(CURL_INCLUDE_DIR "D:/VS/vcpkg/installed/x64-windows/include")
set(CURL_LIBRARY "D:/VS/vcpkg/installed/x64-windows/lib/libcurl.lib")
set(CJSON_INCLUDE_DIR "D:/VS/vcpkg/installed/x64-windows/include")
set(CJSON_LIBRARY "D:/VS/vcpkg/installed/x64-windows/lib/cjson.lib")

include_directories(${CURL_INCLUDE_DIR} ${CJSON_INCLUDE_DIR})

add_executable(WORK main.c
        work.h
        menus.c
        read_certs.c
        get_ids.c
        http.c
        read_ids.c
        get_info.c
        writeinfo.c
        get_characters.c)

target_link_libraries(WORK ${CURL_LIBRARY} ${CJSON_LIBRARY})