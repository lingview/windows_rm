#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define VERSION "1.0"

void print_help(const char *program_name) {
    printf("用法: rm [-r] [-f] [-i] [--version] [--help] 文件/目录...\n", program_name);
    printf("选项:\n");
    printf("  -r\t递归删除目录及其内容\n");
    printf("  -f\t强制删除，不提示确认\n");
    printf("  -i\t交互式删除，每次删除前提示确认\n");
    printf("  --version\t显示版本信息并退出\n");
    printf("  --help\t显示此帮助信息并退出\n");
}

void delete_file(const char *path, int interactive) {
    if (interactive) {
        printf("是否确认删除: %s? (y/n): ", path);
        char confirm[3];
        if (fgets(confirm, sizeof(confirm), stdin) && confirm[0] == 'y') {
            if (DeleteFile(path)) {
                printf("删除文件: %s\n", path);
            } else {
                fprintf(stderr, "删除文件失败: %s\n", path);
            }
        } else {
            printf("取消删除: %s\n", path);
        }
    } else {
        if (DeleteFile(path)) {
            printf("删除文件: %s\n", path);
        } else {
            fprintf(stderr, "删除文件失败: %s\n", path);
        }
    }
}

void delete_directory(const char *path, int interactive) {
    WIN32_FIND_DATA data;
    HANDLE hFind;
    char search_path[MAX_PATH];

    snprintf(search_path, MAX_PATH, "%s\\*", path);
    hFind = FindFirstFile(search_path, &data);

    if (hFind == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "打开目录失败: %s\n", path);
        return;
    }

    do {
        if (strcmp(data.cFileName, ".") != 0 && strcmp(data.cFileName, "..") != 0) {
            char full_path[MAX_PATH];
            snprintf(full_path, MAX_PATH, "%s\\%s", path, data.cFileName);

            if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                delete_directory(full_path, interactive);
            } else {
                delete_file(full_path, interactive);
            }
        }
    } while (FindNextFile(hFind, &data));

    FindClose(hFind);

    if (interactive) {
        printf("确认删除目录吗: %s? (y/n): ", path);
        char confirm[3];
        if (fgets(confirm, sizeof(confirm), stdin) && confirm[0] == 'y') {
            if (RemoveDirectory(path)) {
                printf("删除目录: %s\n", path);
            } else {
                fprintf(stderr, "删除目录失败: %s\n", path);
            }
        } else {
            printf("取消删除目录: %s\n", path);
        }
    } else {
        if (RemoveDirectory(path)) {
            printf("删除目录: %s\n", path);
        } else {
            fprintf(stderr, "删除目录失败: %s\n", path);
        }
    }
}

int main(int argc, char *argv[]) {
    int recursive = 0;
    int force = 0;
    int interactive = 0;
    int i = 1;

    while (i < argc) {
        char *arg = argv[i];
        if (strcmp(arg, "--version") == 0) {
            printf("当前版本 %s\n", VERSION);
            return 0;
        } else if (strcmp(arg, "--help") == 0) {
            print_help(argv[0]);
            return 0;
        } else if (arg[0] == '-') {
            for (int j = 1; arg[j] != '\0'; j++) {
                switch (arg[j]) {
                    case 'r':
                        recursive = 1;
                        break;
                    case 'f':
                        force = 1;
                        break;
                    case 'i':
                        interactive = 1;
                        break;
                    default:
                        fprintf(stderr, "语法格式: %s [-r] [-f] [-i] [--version] [--help] 文件/目录...\n", argv[0]);
                        exit(EXIT_FAILURE);
                }
            }
        } else {
            break;
        }
        i++;
    }

    if (i >= argc) {
        fprintf(stderr, "没有指定文件或目录\n");
        exit(EXIT_FAILURE);
    }

    for (; i < argc; i++) {
        WIN32_FIND_DATA data;
        HANDLE hFind;
        char search_path[MAX_PATH];

        snprintf(search_path, MAX_PATH, "%s", argv[i]);
        hFind = FindFirstFile(search_path, &data);

        if (hFind == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "找不到文件或目录: %s\n", argv[i]);
            continue;
        }

        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (recursive) {
                delete_directory(argv[i], interactive);
            } else {
                fprintf(stderr, "这是一个目录: %s\n", argv[i]);
            }
        } else {
            delete_file(argv[i], interactive);
        }

        FindClose(hFind);
    }

    return 0;
}