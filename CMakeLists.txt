cmake_minimum_required(VERSION 3.31)
project(Class_Management_System LANGUAGES C)

# 基础配置
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# 数据目录设置（使用运行时相对路径）
set(DATA_DIR_NAME "data")
set(DATA_DIR_RELATIVE "./${DATA_DIR_NAME}")

# 可执行文件
add_executable(Class_Management_System
        src/main.c
        src/student.c
)

# 头文件包含
target_include_directories(Class_Management_System PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# 正确的宏定义传递方式
target_compile_definitions(Class_Management_System PRIVATE
        DATA_DIR="${DATA_DIR_RELATIVE}"
)

# Windows配置
if(WIN32)
    target_link_libraries(Class_Management_System PRIVATE shlwapi)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()