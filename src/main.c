//
// Created by fanyuo on 2025/6/28.
//
/*****************************************************************
 * 北京建筑大学 智能科学与技术学院
 * 课程名称：C语言程序设计课程设计
 * 设计题目：班级通讯录管理系统
 *
 * 文件名称：main.c
 * 创 建 者：樊彧（电气241）、何嘉萌（电气241）、李一诺（电气241）
 * 创建日期：2025年6月28日
 *
 * 功能描述：
 *   本模块实现班级通讯录的核心管理功能，包括：
 *   1. 班级创建/加载/删除
 *   2. 学生信息增删改查
 *   3. 数据文件持久化存储（CSV格式）
 *
 * 开发环境：
 *   - 操作系统：Windows 11
 *   - 编译器：GCC 8.1.0 (MinGW-W64)
 *   - 构建工具：CMake 3.31
 *   - IDE：CLion 2025.1.3
 *   - 标准：C99
 *   - 依赖库：标准C库（stdio.h, string.h等）
 *
 * 版权声明：仅供北京建筑大学课程设计使用，未经许可不得外传
 *****************************************************************/

#include "student.h"

int main() {
    init_system();
    int choice;
    printf("======================================================\n");
    printf("C语言程序设计-课程设计\n");
    printf("班级通讯录管理系统\n");
    printf("作者：电气241樊彧、电气241何嘉萌、电气241李一诺\n");
    printf("======================================================\n");
    while(1) {
        printf("\n1. 创建新班级\n");
        printf("2. 加载已有班级\n");
        printf("3. 删除班级\n");
        printf("4. 管理当前班级\n");
        printf("5. 保存当前班级\n");
        printf("0. 退出\n");
        printf("请选择: ");

        scanf("%d", &choice);
        while(getchar() != '\n');

        switch(choice) {
        case 1:
            create_new_class();
            break;
        case 2:
            load_existing_class();
            break;
        case 3:
            show_delete_class_ui();
            break;
        case 4:
            if(class_loaded) {
                manage_class_menu();
            } else {
                printf("请先创建或加载班级!\n");
            }
            break;
        case 5:
            if(class_loaded) {
                save_current_class();
            } else {
                printf("请先创建或加载班级!\n");
            }
            break;
        case 0:
            printf("结束进程!\n");
            system("pause");
            exit(0);
        default:
            printf("无效选择!\n");
        }
    }
    return 0;
}




