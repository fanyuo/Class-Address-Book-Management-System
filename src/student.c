//
// Created by fanyuo on 2025/6/28.
//
/**
 * @file student.c
 * @brief 班级通讯录管理系统核心实现
 * @author 樊彧（电气241）、何嘉萌（电气241）、李一诺（电气241）
 * @created 2025-06-28
 */

/*****************************************************************
 * 北京建筑大学 智能科学与技术学院
 * 课程名称：C语言程序设计课程设计
 * 设计题目：班级通讯录管理系统
 *
 * 文件功能：实现班级和学生信息的存储、排序、文件I/O等核心功能
 *****************************************************************/

#include "student.h"

Class current_class;    ///< 全局班级数据
int class_loaded = 0;   ///< 班级加载状态标志

/* ========== 系统初始化 ========== */

/**
 * @brief 初始化系统环境
 * @details 创建数据存储目录，跨平台兼容Windows和Unix-like系统
 * @note 目录路径由DATA_DIR宏定义
 */
void init_system() {
    // 创建数据目录
#ifdef _WIN32
    _mkdir(DATA_DIR);  // Windows平台
#else
    mkdir(DATA_DIR, 0755);  // Linux/macOS平台
#endif
}

/* ========== 班级管理 ========== */

/**
 * @brief 创建新班级
 * @details 初始化班级结构体并设置班级名称
 * @warning 班级名称长度限制为29个字符
 */
void create_new_class() {
    printf("\n=== 创建新班级 ===\n");
    memset(&current_class, 0, sizeof(Class));

    printf("请输入班级名称(最多29字符): ");
    fgets(current_class.class_name, sizeof(current_class.class_name), stdin);
    current_class.class_name[strcspn(current_class.class_name, "\n")] = '\0';

    current_class.count = 0;
    class_loaded = 1;
    printf("\n班级创建成功!\n");
}

/**
 * @brief 加载已有班级文件
 * @details 从DATA_DIR目录读取CSV格式的班级数据
 * @return void
 * @retval 无显式返回值，通过全局变量class_loaded传递状态
 * @par 文件格式要求:
 *   1. 第一行: class_name标题
 *   2. 第二行: 实际班级名称
 *   3. 第四行开始: 学生数据
 */
void load_existing_class() {
    DIR *dir;
    struct dirent *ent;
    char file_list[20][50];
    int file_count = 0;

    printf("\n=== 可用的班级文件 ===\n");
    if ((dir = opendir(DATA_DIR)) != NULL) {
        while ((ent = readdir(dir)) != NULL && file_count < 20) {
            if(strstr(ent->d_name, ".csv")) {
                strcpy(file_list[file_count], ent->d_name);
                printf("%d. %s\n", file_count + 1, file_list[file_count]);
                file_count++;
            }
        }
        closedir(dir);
    }

    if (file_count == 0) {
        printf("没有找到班级文件!\n");
        return;
    }

    printf("\n请输入要加载的文件编号: ");
    int choice = 0;
    while (1) {
        char input[10];
        fgets(input, sizeof(input), stdin);
        choice = atoi(input);
        if (choice > 0 && choice <= file_count) break;
        printf("无效输入，请重新输入(1-%d): ", file_count);
    }

    char path[100];
    snprintf(path, sizeof(path), "%s/%s", DATA_DIR, file_list[choice-1]);

    FILE *file = fopen(path, "r");
    if (!file) {
        printf("无法打开文件 %s\n", path);
        return;
    }

    // 初始化班级
    memset(&current_class, 0, sizeof(Class));
    current_class.count = 0;

    char line[512];
    int line_num = 0;
    int student_data_started = 0;

    while (fgets(line, sizeof(line), file)) {
        line_num++;
        line[strcspn(line, "\n")] = '\0';

        // 跳过空行
        if (strlen(line) == 0) {
            continue;
        }

        // 第一行是班级名称标题行
        if (line_num == 1) {
            if (strcmp(line, "class_name") != 0) {
                printf("错误: 无效的文件格式\n");
                fclose(file);
                return;
            }
            continue;
        }

        // 第二行是实际的班级名称
        if (line_num == 2) {
            strncpy(current_class.class_name, line, sizeof(current_class.class_name)-1);
            continue;
        }

        // 第三行应该是空行分隔
        if (line_num == 3) {
            continue;
        }

        // 第四行是学生信息标题行
        if (line_num == 4) {
            student_data_started = 1;
            continue;
        }

        // 后续行是学生数据
        if (student_data_started && current_class.count < MAX_STUDENTS) {
            Student *stu = &current_class.students[current_class.count];

            char *token = strtok(line, ",");
            if (token) strncpy(stu->id, token, sizeof(stu->id)-1);

            token = strtok(NULL, ",");
            if (token) strncpy(stu->name, token, sizeof(stu->name)-1);

            token = strtok(NULL, ",");
            if (token && token[0]) stu->gender = token[0];

            token = strtok(NULL, ",");
            if (token) strncpy(stu->phone, token, sizeof(stu->phone)-1);

            token = strtok(NULL, ",");
            if (token) strncpy(stu->address, token, sizeof(stu->address)-1);

            token = strtok(NULL, ",");
            if (token) strncpy(stu->email, token, sizeof(stu->email)-1);

            current_class.count++;
        }
    }

    fclose(file);
    class_loaded = 1;
    printf("\n班级 [%s] 加载成功! 共加载 %d 名学生\n",
           current_class.class_name, current_class.count);
}

/**
 * @brief 保存当前班级到文件
 * @details 按学号排序后保存为CSV格式
 * @param[in] 无
 * @param[out] 生成数据文件
 * @note 文件路径格式: data/class_[班级名称].csv
 */
void save_current_class() {
    if (!class_loaded) {
        printf("没有需要保存的班级数据!\n");
        return;
    }

    // 保存前确保数据按学号排序
    for (int i = 1; i < current_class.count; i++) {
        Student key = current_class.students[i];
        int j = i - 1;

        // 插入排序实现
        while (j >= 0 && strcmp(current_class.students[j].id, key.id) > 0) {
            current_class.students[j+1] = current_class.students[j];
            j--;
        }
        current_class.students[j+1] = key;
    }

    char filename[50];
    snprintf(filename, sizeof(filename), "class_%s.csv", current_class.class_name);

    char path[100];
    snprintf(path, sizeof(path), "%s/%s", DATA_DIR, filename);

    FILE *file = fopen(path, "w");
    if (!file) {
        printf("无法保存文件到 %s\n", path);
        return;
    }

    // 写入班级信息
    fprintf(file, "class_name\n");
    fprintf(file, "%s\n\n", current_class.class_name);

    // 写入学生信息（已排序）
    fprintf(file, "id,name,gender,phone,address,email\n");
    for (int i = 0; i < current_class.count; i++) {
        Student *s = &current_class.students[i];
        fprintf(file, "%s,%s,%c,%s,%s,%s\n",
               s->id, s->name, s->gender,
               s->phone, s->address, s->email);
    }

    fclose(file);
    printf("\n班级数据已按学号排序保存到: %s\n", path);
}

int delete_class(const char *class_name) {
    if (!class_name || strlen(class_name) == 0) {
        return 0;
    }

    // 构建文件名
    char filename[50];
    snprintf(filename, sizeof(filename), "class_%s.csv", class_name);

    char path[100];
    snprintf(path, sizeof(path), "%s/%s", DATA_DIR, filename);

    // 删除文件
    if (remove(path) != 0) {
        if (errno == ENOENT) {
            printf("班级文件不存在: %s\n", path);
        } else {
            perror("删除班级失败");
        }
        return 0;
    }

    // 如果删除的是当前加载的班级，重置状态
    if (class_loaded && strcmp(current_class.class_name, class_name) == 0) {
        memset(&current_class, 0, sizeof(Class));
        class_loaded = 0;
    }

    printf("成功删除班级: %s\n", class_name);
    return 1;
}

void show_delete_class_ui() {
    DIR *dir;
    struct dirent *ent;
    char file_list[20][50];
    int file_count = 0;

    printf("\n=== 可删除的班级文件 ===\n");
    if ((dir = opendir(DATA_DIR)) != NULL) {
        while ((ent = readdir(dir)) != NULL && file_count < 20) {
            if (strstr(ent->d_name, ".csv")) {
                strcpy(file_list[file_count], ent->d_name);
                printf("%d. %s\n", file_count + 1, file_list[file_count]);
                file_count++;
            }
        }
        closedir(dir);
    }

    if (file_count == 0) {
        printf("没有找到班级文件!\n");
        return;
    }

    printf("\n请输入要删除的文件编号(0取消): ");
    int choice = -1;
    while (1) {
        char input[10];
        fgets(input, sizeof(input), stdin);
        choice = atoi(input);
        if (choice >= 0 && choice <= file_count) break;
        printf("无效输入，请重新输入(0-%d): ", file_count);
    }

    if (choice == 0) {
        printf("取消删除操作\n");
        return;
    }

    // 从文件名提取班级名称
    char *filename = file_list[choice - 1];
    char class_name[30] = {0};
    if (sscanf(filename, "class_%[^.]", class_name) != 1) {
        printf("无法解析班级名称\n");
        return;
    }

    printf("\n确认删除班级 %s 吗？(y/n): ", class_name);
    char confirm;
    scanf("%c", &confirm);
    while(getchar() != '\n');

    if (tolower(confirm) == 'y') {
        if (delete_class(class_name)) {
            printf("班级删除成功\n");
        }
    } else {
        printf("取消删除\n");
    }
}

/* ========== 学生管理 ========== */

/**
 * @brief 按学号有序插入学生
 * @param cls 目标班级指针
 * @param stu 待插入学生数据指针
 * @return int 插入位置的索引
 * @retval >=0 成功插入位置
 * @retval -1 学号已存在
 * @retval 0 班级已满
 * @complexity O(n) 插入排序算法
 */
int insert_student_sorted(Class *cls, const Student *stu) {
    // 找到插入位置（保持学号升序）
    int i = cls->count;
    while (i > 0 && strcmp(cls->students[i-1].id, stu->id) > 0) {
        i--;
    }
    // 移动后续元素
    for (int j = cls->count; j > i; j--) {
        cls->students[j] = cls->students[j-1];
    }
    // 插入新学生
    cls->students[i] = *stu;
    cls->count++;
    return i; // 返回插入位置
}

int add_student(Class *cls, const Student *stu) {
    if (cls->count >= MAX_STUDENTS) {
        printf("班级已满! 最大容量: %d\n", MAX_STUDENTS);
        return 0;
    }

    if (find_student(cls, stu->id) != NULL) {
        printf("学号 %s 已存在!\n", stu->id);
        return -1;
    }

    // 调用排序插入函数替代直接添加
    insert_student_sorted(cls, stu);
    return 1;
}

int delete_student(Class *cls, const char *id) {
    for (int i = 0; i < cls->count; i++) {
        if (strcmp(cls->students[i].id, id) == 0) {
            for (int j = i; j < cls->count - 1; j++) {
                cls->students[j] = cls->students[j + 1];
            }
            memset(&cls->students[cls->count - 1], 0, sizeof(Student));
            cls->count--;
            return 1;
        }
    }
    printf("未找到学号为 %s 的学生!\n", id);
    return 0;
}

int modify_student(Class *cls, const char *id, const Student *new_data) {
    Student *stu = find_student(cls, id);
    if (stu == NULL) {
        printf("未找到学号为 %s 的学生!\n", id);
        return 0;
    }

    if (strcmp(id, new_data->id) != 0 && find_student(cls, new_data->id) != NULL) {
        printf("学号 %s 已存在!\n", new_data->id);
        return -1;
    }

    *stu = *new_data;
    return 1;
}

/**
 * @brief 查找学生信息
 * @param cls 班级指针
 * @param id 要查找的学号
 * @return Student* 找到的学生指针
 * @retval NULL 未找到
 * @note 使用线性搜索算法
 */
Student* find_student(const Class *cls, const char *id) {
    for (int i = 0; i < cls->count; i++) {
        if (strcmp(cls->students[i].id, id) == 0) {
            return (Student*)&cls->students[i];
        }
    }
    return NULL;
}

/* ========== 信息显示 ========== */

void print_class_info(const Class *cls) {
    printf("\n=== 班级信息 ===\n");
    printf("班级名称: %s\n", cls->class_name);
    printf("学生人数: %d/%d\n", cls->count, MAX_STUDENTS);
}

void print_all_students(const Class *cls) {
    printf("\n=== 学生列表 (共 %d 人) ===\n", cls->count);

    for (int i = 0; i < cls->count; i++) {
        const Student *s = &cls->students[i];

        printf("\n────── 学生 %d ──────\n", i+1);
        printf("学号: %s\n姓名: %s\n性别: %c\n电话: %s\n邮箱: %s\n住址: %s\n",
               s->id, s->name, s->gender, s->phone, s->email, s->address);
        printf("────────────────────");
    }

    printf("\n=== 列表结束 ===\n");
}

void print_student(const Student *stu) {
    printf("\n=== 学生详细信息 ===\n");
    printf("学号: %s\n", stu->id);
    printf("姓名: %s\n", stu->name);
    printf("性别: %c\n", stu->gender);
    printf("电话: %s\n", stu->phone);
    printf("邮箱: %s\n", stu->email);
    printf("住址: %s\n", stu->address);
}

/* ========== UI 函数 ========== */

/**
 * @brief 显示添加学生界面
 * @param cls 目标班级指针
 * @details 交互式收集学生信息并验证
 * @par 输入流程:
 *   1. 学号（唯一性检查）
 *   2. 姓名
 *   3. 性别（自动转大写）
 *   4. 联系方式
 *   5. 住址信息
 */
void show_add_student_ui(Class *cls) {
    Student stu;
    memset(&stu, 0, sizeof(Student));

    printf("\n=== 添加学生 ===\n");
    printf("请输入学号: ");
    fgets(stu.id, sizeof(stu.id), stdin);
    stu.id[strcspn(stu.id, "\n")] = '\0';

    printf("请输入姓名: ");
    fgets(stu.name, sizeof(stu.name), stdin);
    stu.name[strcspn(stu.name, "\n")] = '\0';

    printf("请输入性别(M/F): ");
    stu.gender = getchar();
    while(getchar() != '\n');
    stu.gender = toupper(stu.gender);

    printf("请输入电话: ");
    fgets(stu.phone, sizeof(stu.phone), stdin);
    stu.phone[strcspn(stu.phone, "\n")] = '\0';

    printf("请输入住址: ");
    fgets(stu.address, sizeof(stu.address), stdin);
    stu.address[strcspn(stu.address, "\n")] = '\0';

    printf("请输入邮箱: ");
    fgets(stu.email, sizeof(stu.email), stdin);
    stu.email[strcspn(stu.email, "\n")] = '\0';

    int result = add_student(cls, &stu);
    if (result == 1) {
        printf("添加成功!\n");
    } else if (result == -1) {
        printf("添加失败: 学号已存在!\n");
    } else {
        printf("添加失败: 班级已满!\n");
    }
}

void show_delete_student_ui(Class *cls) {
    printf("\n=== 删除学生 ===\n");
    if (cls->count == 0) {
        printf("当前班级没有学生记录\n");
        return;
    }

    char id[15];
    printf("请输入要删除的学生学号: ");
    fgets(id, sizeof(id), stdin);
    id[strcspn(id, "\n")] = '\0';

    int result = delete_student(cls, id);
    if (result) {
        printf("删除成功!\n");
    } else {
        printf("删除失败: 学号不存在!\n");
    }
}

void show_modify_student_ui(Class *cls) {
    printf("\n=== 修改学生信息 ===\n");
    if (cls->count == 0) {
        printf("当前班级没有学生记录\n");
        return;
    }

    char id[15];
    printf("请输入要修改的学生学号: ");
    fgets(id, sizeof(id), stdin);
    id[strcspn(id, "\n")] = '\0';

    Student *stu = find_student(cls, id);
    if (stu == NULL) {
        printf("未找到该学号的学生\n");
        return;
    }

    printf("\n当前学生信息:\n");
    print_student(stu);

    Student new_stu = *stu;
    printf("\n请输入新信息(留空保持不变):\n");

    printf("新学号(当前:%s): ", stu->id);
    char input[20];
    if (fgets(input, sizeof(input), stdin) && input[0] != '\n') {
        input[strcspn(input, "\n")] = '\0';
        strncpy(new_stu.id, input, sizeof(new_stu.id)-1);
    }

    printf("新姓名(当前:%s): ", stu->name);
    if (fgets(input, sizeof(input), stdin) && input[0] != '\n') {
        input[strcspn(input, "\n")] = '\0';
        strncpy(new_stu.name, input, sizeof(new_stu.name)-1);
    }

    printf("新性别(当前:%c): ", stu->gender);
    if (fgets(input, sizeof(input), stdin) && input[0] != '\n') {
        new_stu.gender = toupper(input[0]);
    }

    printf("新电话(当前:%s): ", stu->phone);
    if (fgets(input, sizeof(input), stdin) && input[0] != '\n') {
        input[strcspn(input, "\n")] = '\0';
        strncpy(new_stu.phone, input, sizeof(new_stu.phone)-1);
    }

    printf("新住址(当前:%s): ", stu->address);
    if (fgets(input, sizeof(input), stdin) && input[0] != '\n') {
        input[strcspn(input, "\n")] = '\0';
        strncpy(new_stu.address, input, sizeof(new_stu.address)-1);
    }

    printf("新邮箱(当前:%s): ", stu->email);
    if (fgets(input, sizeof(input), stdin) && input[0] != '\n') {
        input[strcspn(input, "\n")] = '\0';
        strncpy(new_stu.email, input, sizeof(new_stu.email)-1);
    }

    int result = modify_student(cls, id, &new_stu);
    printf(result ? "修改成功!\n" : "修改失败!\n");
}

void show_search_student_ui(const Class *cls) {
    printf("\n=== 查找学生 ===\n");
    if (cls->count == 0) {
        printf("当前班级没有学生记录\n");
        return;
    }

    int choice;
    printf("\n1. 按学号查找\n");
    printf("2. 按姓名查找\n");
    printf("请选择查找方式: ");
    scanf("%d", &choice);
    while(getchar() != '\n');

    if (choice == 1) {
        char id[15];
        printf("请输入学号: ");
        fgets(id, sizeof(id), stdin);
        id[strcspn(id, "\n")] = '\0';

        Student *stu = find_student(cls, id);
        if (stu) {
            print_student(stu);
        } else {
            printf("未找到该学号的学生\n");
        }
    }
    else if (choice == 2) {
        char name[20];
        printf("请输入姓名: ");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = '\0';

        int found = 0;
        for (int i = 0; i < cls->count; i++) {
            if (strcmp(cls->students[i].name, name) == 0) {
                print_student(&cls->students[i]);
                found = 1;
            }
        }
        if (!found) {
            printf("未找到该姓名的学生\n");
        }
    }
    else {
        printf("无效的选择\n");
    }
}

void show_main_menu() {
    printf("\n班级通讯录管理系统\n");
    printf("1. 添加学生\n");
    printf("2. 删除学生\n");
    printf("3. 修改学生信息\n");
    printf("4. 查找学生\n");
    printf("5. 显示所有学生\n");
    printf("6. 显示班级信息\n");
    printf("0. 退出\n");
    printf("请选择: ");
}

/**
 * @brief 主菜单控制循环
 * @details 处理用户选择并调用对应功能模块
 * @par 功能分支:
 *   1-添加 2-删除 3-修改 4-查询 5-显示 6-班级信息 0-退出
 */
void manage_class_menu() {
    int choice;
    do {
        show_main_menu();
        scanf("%d", &choice);
        while(getchar() != '\n');

        switch(choice) {
        case 1: show_add_student_ui(&current_class); break;
        case 2: show_delete_student_ui(&current_class); break;
        case 3: show_modify_student_ui(&current_class); break;
        case 4: show_search_student_ui(&current_class); break;
        case 5: print_all_students(&current_class); break;
        case 6: print_class_info(&current_class); break;
        case 0: return;
        default: printf("无效选择!\n");
        }
    } while(1);
}