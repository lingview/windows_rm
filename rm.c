#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define VERSION "1.0"

void print_help(const char *program_name) {
    printf("�÷�: rm [-r] [-f] [-i] [--version] [--help] �ļ�/Ŀ¼...\n", program_name);
    printf("ѡ��:\n");
    printf("  -r\t�ݹ�ɾ��Ŀ¼��������\n");
    printf("  -f\tǿ��ɾ��������ʾȷ��\n");
    printf("  -i\t����ʽɾ����ÿ��ɾ��ǰ��ʾȷ��\n");
    printf("  --version\t��ʾ�汾��Ϣ���˳�\n");
    printf("  --help\t��ʾ�˰�����Ϣ���˳�\n");
}

void delete_file(const char *path, int interactive) {
    if (interactive) {
        printf("�Ƿ�ȷ��ɾ��: %s? (y/n): ", path);
        char confirm[3];
        if (fgets(confirm, sizeof(confirm), stdin) && confirm[0] == 'y') {
            if (DeleteFile(path)) {
                printf("ɾ���ļ�: %s\n", path);
            } else {
                fprintf(stderr, "ɾ���ļ�ʧ��: %s\n", path);
            }
        } else {
            printf("ȡ��ɾ��: %s\n", path);
        }
    } else {
        if (DeleteFile(path)) {
            printf("ɾ���ļ�: %s\n", path);
        } else {
            fprintf(stderr, "ɾ���ļ�ʧ��: %s\n", path);
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
        fprintf(stderr, "��Ŀ¼ʧ��: %s\n", path);
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
        printf("ȷ��ɾ��Ŀ¼��: %s? (y/n): ", path);
        char confirm[3];
        if (fgets(confirm, sizeof(confirm), stdin) && confirm[0] == 'y') {
            if (RemoveDirectory(path)) {
                printf("ɾ��Ŀ¼: %s\n", path);
            } else {
                fprintf(stderr, "ɾ��Ŀ¼ʧ��: %s\n", path);
            }
        } else {
            printf("ȡ��ɾ��Ŀ¼: %s\n", path);
        }
    } else {
        if (RemoveDirectory(path)) {
            printf("ɾ��Ŀ¼: %s\n", path);
        } else {
            fprintf(stderr, "ɾ��Ŀ¼ʧ��: %s\n", path);
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
            printf("��ǰ�汾 %s\n", VERSION);
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
                        fprintf(stderr, "�﷨��ʽ: %s [-r] [-f] [-i] [--version] [--help] �ļ�/Ŀ¼...\n", argv[0]);
                        exit(EXIT_FAILURE);
                }
            }
        } else {
            break;
        }
        i++;
    }

    if (i >= argc) {
        fprintf(stderr, "û��ָ���ļ���Ŀ¼\n");
        exit(EXIT_FAILURE);
    }

    for (; i < argc; i++) {
        WIN32_FIND_DATA data;
        HANDLE hFind;
        char search_path[MAX_PATH];

        snprintf(search_path, MAX_PATH, "%s", argv[i]);
        hFind = FindFirstFile(search_path, &data);

        if (hFind == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "�Ҳ����ļ���Ŀ¼: %s\n", argv[i]);
            continue;
        }

        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (recursive) {
                delete_directory(argv[i], interactive);
            } else {
                fprintf(stderr, "����һ��Ŀ¼: %s\n", argv[i]);
            }
        } else {
            delete_file(argv[i], interactive);
        }

        FindClose(hFind);
    }

    return 0;
}