#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
#endif
#include "kanji_search_exact.h"
#include "vocab_search_exact.h"
#include "fuzzy_search.h"
#include "substring_search.h"
#include "multiple_choice.h"
#include "lessons_management.h"
#include "filter_learned_lesson.h"
#include "parse_json_to_struct.h"
#include "../data_structures.h"
#include "dashboard.h"

void pauseAndClear() {
    printf("\nNhấn phím bất kỳ để quay lại Dashboard...");
    getchar(); // chờ người dùng nhấn phím
    system("cls");   // Windows
}


void displayMenu() {
    printf("\n======== BẢNG ĐIỀU KHIỂN ========\n");
    printf("[1]. Tìm kiếm từ điển (Chính xác / KMP / Tìm mờ)\n");
    printf("[2]. Quản lý từ điển cá nhân\n");
    printf("[3]. Lọc từ vựng đã học\n");
    printf("[4]. Lọc từ vựng Kanji thông minh\n");
    printf("[5]. Phân tích câu\n");
    printf("[6]. Trắc nghiệm\n");
    printf("[7]. Ôn tập từ sai\n");
    printf("[8]. Kanji họ hàng (Bộ thủ)\n");
    printf("[0]. Thoát chương trình\n");
    printf(">>> Nhập lựa chọn [0-8]: ");
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


// --- MENU CẤP 2: TÌM KIẾM CHÍNH XÁC (EXACT SEARCH) ---
void menuExactSearch(HashTable *vocabHT, HashTableK *kanjiHT) {
    int choice;
    char keyword[256];
    system("cls");
    printf("\n==== TÌM KIẾM CHÍNH XÁC ====\n");
    printf("[1]. Tra cứu Kanji (Mặt chữ/Hán Việt)\n");
    printf("[2]. Tra cứu Từ vựng (Nhật/Hiragana/Romaji/Tiếng Việt)\n");
    printf("[0]. Quay lại\n");
    printf("Chọn: ");
    scanf("%d", &choice); getchar();

    if (choice == 0) return;
    printf("Nhập từ khóa chính xác: ");
    inputString(keyword, 256);

    system("cls");
    printf("Kết quả tìm kiếm cho: %s\n", keyword);
    if (choice == 1) exactlySearchingKanji(kanjiHT, keyword);
    else if (choice == 2) exactlySearching(vocabHT, keyword);
    
    printf("\n-------------------------------------------\n");
}


void menuKMPSearch(KanjiList *L) {
    int option;
    char keyword[256];
    system("cls");
    printf("\n==== TÌM KIẾM THEO MẪU ====\n");
    printf("[1]. Khớp theo mặt chữ Kanji\n");
    printf("[2]. Khớp theo cách đọc Hiragana\n");
    printf("[3]. Khớp theo phiên âm Romaji\n");
    printf("[4]. Khớp theo nghĩa Tiếng Việt\n");
    printf("[0]. Quay lại\n");
    printf("Chọn [1-4]: ");
    scanf("%d", &option); getchar();

    if (option < 1 || option > 4) return;

    printf("Nhập từ cần tìm : ");
    inputString(keyword, 256);

    system("cls");
    printf("Kết quả tìm kiếm cho: %s\n", keyword);
    substringSearching(L, keyword, option); 
    printf("\n-------------------------------------------\n");
}

void menuFuzzySearch(KanjiList *L) {
    int option;
    char keyword[256];
    system("cls");
    printf("\n==== TÌM KIẾM MỜ ====\n");
    printf("[1]. Khớp theo mặt chữ Kanji\n");
    printf("[2]. Khớp theo cách đọc Hiragana\n");
    printf("[3]. Khớp theo phiên âm Romaji\n");
    printf("[4]. Khớp theo nghĩa Tiếng Việt\n");
    printf("[0]. Quay lại\n");
    printf("Chọn [1-4]: ");
    scanf("%d", &option); getchar();

    if (option < 1 || option > 4) return;

    printf("Nhập từ cần tìm : ");
    inputString(keyword, 256);

    system("cls");
    printf("Kết quả tìm kiếm cho: %s\n", keyword);
    fuzzySearching(L, keyword, option); 
    printf("\n-------------------------------------------\n");
}

void caseNo1(KanjiList *L, HashTable *vHT, HashTableK *kHT) {
    int choice;
    while(1) {
        system("cls");
        printf("\n======== HỆ THỐNG TÌM KIẾM ========\n");
        printf("[1]. Exact Match (Tìm đúng 100%%)\n");
        printf("[2]. Pattern Match (KMP - Tìm chuỗi con)\n");
        printf("[3]. Fuzzy Match (Levenshtein - Tìm gần đúng)\n");
        printf("[0]. Thoát ra Menu chính\n");
        printf(">>> Chọn loại hình: ");
        if (scanf("%d", &choice) != 1) { while(getchar() != '\n'); continue; }
        getchar();

        if (choice == 0) break;
        switch (choice) {
            case 1: menuExactSearch(vHT, kHT); break;
            case 2: menuKMPSearch(L); break;
            case 3: menuFuzzySearch(L); break;
        }
        printf("Nhấn Enter để chọn lại loại tìm kiếm...");
        getchar();
    }
}

void caseNo2(KanjiList *L) {
    system("cls");
    displayAllLessons(L);
    selectAndDisplayLesson(L);
}

void caseNo3(KanjiList *L) {
    runFilterLearnedVocab(L);
}

void caseNo6(KanjiList *L) {
    runChoiceOption(L);
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
        system("cls");
        displayMenu();
        if (scanf("%d", &choice) != 1) {
            printf("Vui long nhap so hop le.\n");
            while (getchar() != '\n'); // Clear buffer
            continue;
        }
        getchar(); // Clear newline character

        switch (choice) {
            case 1: {
                caseNo1(&myData, vocabHT, kanjiHT);
                break;
            }
            case 2:
                caseNo2(&myData);
                pauseAndClear();
                break;
            case 3:
                caseNo3(&myData);
                pauseAndClear();
                break;
            case 4:
                printf("Chuc nang 'Loc theo Lesson' dang duoc phat trien.\n");
                break;
            case 5:
                printf("Chuc nang 'Loc tu vung Kanji thong minh' dang duoc phat trien.\n");
                break;
            case 6:
                caseNo6(&myData); //bai tap
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
                getchar(); // chờ người dùng nhấn phím
        }
    }
}