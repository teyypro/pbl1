#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
#endif
#include "exact_searching_vocab.h"
#include "exact_searching_kanji.h"
#include "fuzzy_searching.h"
#include "parse_json_to_struct.h"
#include "../data_structures.h"
#include "menu.h"

void pauseAndClear() {
    printf("\nNhấn phím bất kỳ để quay lại Dashboard...");
    getchar(); // chờ người dùng nhấn phím
    system("cls");   // Windows
}


void displayMenu() {
    printf("\n======== BẢNG ĐIỀU KHIỂN ========\n");
    printf("[1]. Tìm kiếm chính xác\n");
    printf("[2]. Tìm kiếm mờ\n");
    printf("[3]. Quản lý từ điển cá nhân\n");
    printf("[4]. Lọc theo Bài học\n");
    printf("[5]. Lọc từ vựng Kanji thông minh\n");
    printf("[6]. Phân tích câu\n");
    printf("[7]. Trắc nghiệm\n");
    printf("[8]. Ôn tập từ sai\n");
    printf("[9]. Kanji họ hàng (Bộ thủ)\n");
    printf("[0]. Thoát chương trình\n");
    printf(">>> Nhập lựa chọn [0-9]: ");
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

void caseNo1(HashTable *vocabHT, HashTableK *kanjiHT) {
    int searchSubChoice;
    char keyword[256];

    system("cls");
    printf("\n--- CHỌN KIỂU TÌM KIẾM CHÍNH XÁC ---\n");
    printf("[1]. Tìm kiếm Kanji (Mặt chữ, Hán Việt)\n");
    printf("[2]. Tìm kiếm Từ vựng (Nhật, Furigana, Romaji)\n");
    printf("Chọn [1-2]: ");

    
    if (scanf("%d", &searchSubChoice) != 1) {
        printf("Lua chon khong hop le.\n");
        while (getchar() != '\n'); 
        return;
    }
    getchar(); 

    if (searchSubChoice == 1) {
        printf("Nhập kí tự Kanji / âm Hán - Việt cần tra cứu: ");
        inputString(keyword, 256);
        if (strlen(keyword) > 0) {
            system("cls");
            exactlySearchingKanji(kanjiHT, keyword);
        }
    } else if (searchSubChoice == 2) {
        printf("Nhập từ vựng (chữ Hán, cách đọc, phiên âm Latin, nghĩa): ");
        inputString(keyword, 256);
        if (strlen(keyword) > 0) {
            system("cls");
            exactlySearching(vocabHT, keyword);
        }
    } else {
        printf("Lựa chọn không hợp lệ.\n");
    }
}

void caseNo2(KanjiList *L) {
    int choice;
    char keyword[256];

    system("cls");
    printf("\n--- TÌM KIẾM MỜ ---\n");
    printf("[1]. Tìm theo Từ vựng (Kanji)\n");
    printf("[2]. Tìm theo Phiên Âm (Hiragana)\n");
    printf("[3]. Tìm theo Cách đọc (Romaji)\n");
    printf("[4]. Tìm theo Nghĩa (Tiếng Việt)\n");
    printf("Chọn [1-4]: ");
    scanf("%d", &choice);
    getchar(); // Xóa bộ nhớ đệm

    printf("Nhập từ cần tìm: ");
    inputString(keyword, 256); // Dung ham inputString ban da co

    fuzzySearching(L, keyword, choice);
}

void handleMenuSelection(char *rawJson) {
    int choice;


    //printf("Debug: Đang parse JSON...\n");
    KanjiList myData = parseJsonToStruct(rawJson);

    HashTable *vocabHT = createHashTable(HASH_TABLE_SIZE);
    HashTableK *kanjiHT = createHashTableK(HASH_TABLE_SIZE);

    buildHashTableForVocab(&myData, vocabHT);
    buildHashTableForKanji(&myData, kanjiHT);

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
                caseNo1(vocabHT, kanjiHT);
                pauseAndClear();
                break;
            }
            case 2:
                caseNo2(&myData);
                pauseAndClear();
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