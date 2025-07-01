//
// Created by fanyuo on 2025/6/28.
//
/**
 * @file student.h
 * @brief 班级通讯录管理系统核心头文件
 * @author 樊彧（电气241）、何嘉萌（电气241）、李一诺（电气241）
 * @created 2025-06-28
 */

/*****************************************************************
 * 北京建筑大学 智能科学与技术学院
 * 课程名称：C语言程序设计课程设计
 * 设计题目：班级通讯录管理系统
 *
 * 文件功能：定义系统核心数据结构和接口声明，包括：
 *         1. 学生/班级数据结构
 *         2. 文件操作接口
 *         3. 用户界面控制接口
 *****************************************************************/

#ifndef STUDENT_H
#define STUDENT_H

/*--------------------------------
 *          系统头文件
 *--------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>

#ifdef _WIN32
#include <direct.h>         // Windows目录操作
#else
#include <sys/stat.h>       // POSIX目录操作
#endif

/*--------------------------------
 *          常量定义
 *--------------------------------*/
#define MAX_NAME_LEN 20     ///< 姓名最大字符数
#define MAX_PHONE_LEN 15    ///< 电话号码最大长度
#define MAX_ADDR_LEN 50     ///< 地址信息最大长度
#define MAX_EMAIL_LEN 30    ///< 邮箱地址最大长度
#define MAX_STUDENTS 100    ///< 单个班级最大容量
#ifndef DATA_DIR            ///< 数据存储目录名
#define DATA_DIR "data"  // 仅当未定义时设置默认值
#endif

/*--------------------------------
 *          数据结构
 *--------------------------------*/
/**
 * @struct Student
 * @brief 学生信息结构
 */
typedef struct {
    char id[15];                    ///< 学号（格式：20240402XXXX）
    char name[MAX_NAME_LEN];        ///< 学生姓名
    char gender;                    ///< 性别（M/F）
    char phone[MAX_PHONE_LEN];      ///< 联系电话
    char address[MAX_ADDR_LEN];     ///< 家庭住址
    char email[MAX_EMAIL_LEN];      ///< 电子邮箱
} Student;

/**
 * @struct Class
 * @brief 班级信息结构
 */
typedef struct {
    char class_name[30];            ///< 班级名称（如"电气241"）
    Student students[MAX_STUDENTS]; ///< 学生数组
    int count;                      ///< 当前学生人数
} Class;

/*--------------------------------
 *          全局变量
 *--------------------------------*/
extern Class current_class;         ///< 当前操作的班级实例
extern int class_loaded;            ///< 班级加载状态标志（1=已加载）

/*--------------------------------
 *          功能接口
 *--------------------------------*/

// 系统初始化
void init_system();
void create_new_class();
void load_existing_class();
void save_current_class();
void show_delete_class_ui();
int delete_class(const char *class_name);

// 学生管理
int add_student(Class *cls, const Student *stu);
int delete_student(Class *cls, const char *id);
int modify_student(Class *cls, const char *id, const Student *new_data);
Student* find_student(const Class *cls, const char *id);
int insert_student_sorted(Class *cls, const Student *stu);

// 信息显示
void print_class_info(const Class *cls);
void print_all_students(const Class *cls);
void print_student(const Student *stu);

// 用户界面
void show_main_menu();
void show_add_student_ui(Class *cls);
void show_delete_student_ui(Class *cls);
void show_modify_student_ui(Class *cls);
void show_search_student_ui(const Class *cls);
void show_main_menu();
void manage_class_menu();

#endif // STUDENT_H