#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
#endif
#include "exact_searching.h"
#include "parse_json_to_struct.h"
#include "../data_structures.h"
#include "menu.h"

void displayMenu() {
    printf("\n================ CHUONG TRINH HOC TIENG NHAT ================\n");
    printf("1. Tim kiem chinh xac\n");
    printf("2. Tim kiem mo\n");
    printf("3. Quan ly tu dien ca nhan\n");
    printf("4. Loc theo Lesson\n");
    printf("5. Loc tu vung Kanji thong minh\n");
    printf("6. Phan tich cau\n");
    printf("7. Trac nghiem\n");
    printf("8. On tap tu sai\n");
    printf("9. Kanji ho hang (Bo thu)\n");
    printf("0. Thoat chuong trinh\n");
    printf("============================================================\n");
    printf("Chon chuc nang (0-9): ");
}

void inputString(char *buffer, int maxLength) {
    // Xóa bộ đệm trôi từ các lệnh scanf trước đó
    fflush(stdin);

#ifdef _WIN32
    // Sử dụng WinAPI để đọc Unicode (UTF-16)
    wchar_t *wBuf = malloc(maxLength * sizeof(wchar_t));
    DWORD read;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);

    if (ReadConsoleW(hStdin, wBuf, maxLength - 1, &read, NULL)) {
        // Xử lý ký tự xuống dòng tùy theo Terminal (Windows Terminal/CMD/PowerShell)
        if (read >= 2 && wBuf[read - 2] == L'\r') wBuf[read - 2] = L'\0';
        else if (read >= 1 && wBuf[read - 1] == L'\n') wBuf[read - 1] = L'\0';
        else wBuf[read] = L'\0';

        // Chuyển đổi sang UTF-8 để trả về buffer
        WideCharToMultiByte(CP_UTF8, 0, wBuf, -1, buffer, maxLength, NULL, NULL);
    }
    free(wBuf);
#else
    // Dành cho Linux hoặc macOS (mặc định đã là UTF-8)
    if (fgets(buffer, maxLength, stdin)) {
        buffer[strcspn(buffer, "\n")] = 0;
    }
#endif
}

void handleMenuSelection(char *rawJson) {
    int choice;


    printf("Debug: Đang parse JSON...\n");
    KanjiList myData = parseJsonToStruct(rawJson);

    while (1) {
        displayMenu();
        if (scanf("%d", &choice) != 1) {
            printf("Vui long nhap so hop le.\n");
            while (getchar() != '\n'); // Clear buffer
            continue;
        }
        getchar(); // Clear newline character

        switch (choice) {
            case 1: {
                
                HashTable *vocabHT = createHashTable(HASH_TABLE_SIZE);
                buildHashTableForVocab(&myData, vocabHT);
                char keyword[256];
                printf("Nhap tu khoa Kanji/Hiragana: ");
                
                // Gọi hàm vừa viết
                inputString(keyword, 256);

                if (strlen(keyword) > 0) {
                    printf("Debug: Dang tim kiem: %s\n", keyword);
                    exactlySearching(vocabHT, keyword);
                }

                // Ghi chú: Cần viết hàm freeHashTable(vocabHT) để tránh rò rỉ bộ nhớ
                break;
                break;
            }
            case 2:
                printf("Chuc nang 'Tim kiem mo' dang duoc phat trien.\n");
                break;
            case 3:
                printf("Chuc nang 'Quan ly tu dien ca nhan' dang duoc phat trien.\n");
                break;
            case 4:
                printf("Chuc nang 'Loc theo Lesson' dang duoc phat trien.\n");
                break;
            case 5:
                printf("Chuc nang 'Loc tu vung Kanji thong minh' dang duoc phat trien.\n");
                break;
            case 6:
                printf("Chuc nang 'Phan tich cau' dang duoc phat trien.\n");
                break;
            case 7:
                printf("Chuc nang 'Trac nghiem' dang duoc phat trien.\n");
                break;
            case 8:
                printf("Chuc nang 'On tap tu sai' dang duoc phat trien.\n");
                break;
            case 9:
                printf("Chuc nang 'Kanji ho hang' dang duoc phat trien.\n");
                break;
            case 0:
                printf("Dang thoat chuong trinh...\n");
                return;
            default:
                printf("Lua chon khong hop le. Vui long chon lai.\n");
        }
    }
}