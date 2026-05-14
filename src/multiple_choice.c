#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "multiple_choice.h"
#include "../data_structures.h"

#define KANJI_PER_LESSON 16
#include <windows.h> // Để hỗ trợ hiển thị màu trên CMD Windows
// Mã màu ANSI
#define CLR_PRIMARY "\x1b[38;5;75m"   // Xanh Blue sáng
#define CLR_SUCCESS "\x1b[38;5;82m"   // Xanh Green
#define CLR_ERROR   "\x1b[38;5;196m"  // Đỏ
#define CLR_WARN    "\x1b[38;5;214m"  // Cam/Vàng
#define CLR_TEXT    "\x1b[38;5;253m"  // Trắng xám
#define CLR_RESET   "\x1b[0m"
#define BOLD        "\x1b[1m"

// Hàm vẽ đường kẻ ngang
void drawLine(int width, char* color) {
    printf("%s", color);
    for(int i=0; i<width; i++) printf("═");
    printf("%s\n", CLR_RESET);
}

void printAppHeader(const char* title) {
    system("cls");
    printf(CLR_PRIMARY BOLD);
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║ %-56s ║\n", title);
    printf("╚══════════════════════════════════════════════════════════╝\n" CLR_RESET);
}
void freeKanjiMCList(KanjiMCList *list) {
    if (!list) return;
    if (list->arr) {
        for (int i = 0; i < list->kanjiCount; i++) {
            free(list->arr[i].kanji);
            free(list->arr[i].hanViet);
        }
        free(list->arr);
    }
    free(list);
}

void freeVocabMCList(VocabMCList *list) {
    if (!list) return;
    if (list->arr) {
        for (int i = 0; i < list->vocabCount; i++) {
            free(list->arr[i].vocab);
            free(list->arr[i].furigana);
            free(list->arr[i].romaji);
            free(list->arr[i].meaning);
        }
        free(list->arr);
    }
    free(list);
}

KanjiMCList* mapToKanjiMCRange(KanjiList *L, int start, int end) {
    KanjiMCList *mcList = malloc(sizeof(KanjiMCList));
    if (!mcList) return NULL;

    mcList->kanjiCount = end - start;
    mcList->arr = malloc(mcList->kanjiCount * sizeof(KanjiMC));

    for (int i = 0; i < mcList->kanjiCount; i++) {
        mcList->arr[i].kanji = strdup(L->kanjis[start + i].kanji);
        mcList->arr[i].hanViet = strdup(L->kanjis[start + i].hanViet);
    }
    return mcList;
}

VocabMCList* mapToVocabMCRange(KanjiList *L, int start, int end) {
    VocabMCList *vList = malloc(sizeof(VocabMCList));
    if (!vList) return NULL;

    vList->vocabCount = 0;
    for (int i = start; i < end; i++) {
        vList->vocabCount += L->kanjis[i].vocabsCount;
    }

    vList->arr = malloc(vList->vocabCount * sizeof(VocabMC));
    int index = 0;
    for (int i = start; i < end; i++) {
        for (int j = 0; j < L->kanjis[i].vocabsCount; j++) {
            Vocab *v = &L->kanjis[i].vocabs[j];
            vList->arr[index].vocab = strdup(v->vocab);
            vList->arr[index].furigana = strdup(v->hiragana);
            vList->arr[index].romaji = strdup(v->romaji);
            vList->arr[index].meaning = strdup(v->meaning);
            index++;
        }
    }
    return vList;
}

char* getKanjiDataByType(KanjiMC *item, int type) {
    if (type == 1) return item->kanji;
    return item->hanViet;
}

void runMultipleChoiceTestKanji(KanjiMCList *list) {
    if (!list || list->kanjiCount < 4) {
        printf(CLR_ERROR "Không đủ dữ liệu làm trắc nghiệm (cần ít nhất 4 câu).\n" CLR_RESET);
        return;
    }

    int qType, aType;
    system("cls");
    // --- Giao diện Thiết lập ---
    printf(CLR_PRIMARY BOLD "╔══════════════════════════════════════════╗\n");
    printf("║           THIẾT LẬP CÂU HỎI              ║\n");
    printf("╚══════════════════════════════════════════╝\n" CLR_RESET);
    printf(CLR_TEXT "  [1]. Kanji\n  [2]. Hán Việt\n\n" CLR_RESET);

    printf(BOLD " ➤ Chọn loại CÂU HỎI: " CLR_RESET);
    scanf("%d", &qType);
    printf(BOLD " ➤ Chọn loại CÂU TRẢ LỜI: " CLR_RESET);
    scanf("%d", &aType);
    getchar(); 

    srand(time(NULL));
    int score = 0;
    for (int i = 0; i < list->kanjiCount; i++) {
        system("cls");
        char *questionText = getKanjiDataByType(&list->arr[i], qType);
        char *correctAnswerText = getKanjiDataByType(&list->arr[i], aType);

        // --- Giao diện Câu hỏi ---
        printf(CLR_WARN "Câu %d/%d" CLR_RESET, i + 1, list->kanjiCount);
        // Thanh tiến trình nhỏ
        printf("  ");
        for(int p=0; p<15; p++) printf(p < (i+1)*15/list->kanjiCount ? "■" : "□");
        printf("\n");

        printf(CLR_PRIMARY "┌──────────────────────────────────────────┐\n");
        printf("│ %-40s │\n", ""); // Dòng trống tạo khoảng cách
        printf("│ " CLR_TEXT BOLD "  [%s] tương ứng với? " CLR_PRIMARY "%-13s │\n", questionText, "");
        printf("└──────────────────────────────────────────┘\n" CLR_RESET);

        char *options[4];
        int correctIdx = rand() % 4;
        options[correctIdx] = correctAnswerText;

        for (int j = 0; j < 4; j++) {
            if (j == correctIdx) continue;
            int randIdx;
            int isDuplicate;
            do {
                isDuplicate = 0;
                randIdx = rand() % list->kanjiCount;
                if (randIdx == i) isDuplicate = 1;
                if (strcmp(getKanjiDataByType(&list->arr[randIdx], aType), correctAnswerText) == 0) isDuplicate = 1;
            } while (isDuplicate);
            options[j] = getKanjiDataByType(&list->arr[randIdx], aType);
        }

        // --- Hiển thị tùy chọn ---
        for (int j = 0; j < 4; j++) {
            printf("  " CLR_WARN BOLD "[%d]" CLR_RESET " %-30s\n", j + 1, options[j]);
        }
        
        int choice;
        printf("\n" BOLD " ➤ Lựa chọn (0 thoát): " CLR_RESET);
        if (scanf("%d", &choice) != 1 || choice == 0) break;

        // --- Phản hồi Đúng/Sai ---
        if (choice - 1 == correctIdx) {
            printf("\n" CLR_SUCCESS BOLD "   ✔ CHÍNH XÁC!" CLR_RESET "\n");
            score++;
        } else {
            printf("\n" CLR_ERROR BOLD "   ✘ SAI RỒI!" CLR_RESET " Đáp án đúng: " CLR_SUCCESS "%s" CLR_RESET "\n", correctAnswerText);
        }
        
        // --- Hiển thị Flashcard giải thích ---
        printf("\n  " CLR_PRIMARY "┏━━━━━━━ THÔNG TIN ━━━━━━━┓" CLR_RESET);
        printf("\n    Kanji:    " BOLD "%s" CLR_RESET, list->arr[i].kanji);
        printf("\n    Hán Việt: " BOLD "%s" CLR_RESET, list->arr[i].hanViet);
        printf("\n  " CLR_PRIMARY "┗━━━━━━━━━━━━━━━━━━━━━━━━━┛" CLR_RESET);

        printf("\n\n" CLR_TEXT "-- Nhấn Enter để tiếp tục --" CLR_RESET);
        getchar(); getchar();
    }

    // --- Màn hình Kết quả ---
    system("cls");
    printf(CLR_WARN BOLD "╔══════════════════════════════════════════╗\n");
    printf("║             KẾT QUẢ BÀI THI              ║\n");
    printf("╚══════════════════════════════════════════╝\n" CLR_RESET);
    
    printf("\n  Điểm của bạn: " CLR_SUCCESS BOLD "%d/%d" CLR_RESET, score, list->kanjiCount);
    float rate = (float)score / list->kanjiCount;
    
    if (rate == 1.0) printf("\n  Đánh giá: " CLR_SUCCESS "Tuyệt vời! Hoàn hảo. ✨" CLR_RESET);
    else if (rate > 0.5) printf("\n  Đánh giá: " CLR_PRIMARY "Khá tốt! Cố gắng thêm nhé. 👍" CLR_RESET);
    else printf("\n  Đánh giá: " CLR_ERROR "Bạn cần ôn tập lại nhiều hơn. ✍️" CLR_RESET);

    printf("\n\n" CLR_TEXT "Nhấn Enter để quay lại menu chính." CLR_RESET);
    getchar();
}
char* getVocabDataByType(VocabMC *item, int type) {
    switch (type) {
        case 1: return item->vocab;     // Kanji
        case 2: return item->furigana;  // Furigana
        case 3: return item->romaji;    // Romaji
        case 4: return item->meaning;   // Meaning
        default: return item->vocab;
    }
}


void runMultipleChoiceTestVocab(VocabMCList *list) {
    if (!list || list->vocabCount < 4) {
        printf(CLR_ERROR "Không đủ dữ liệu!\n" CLR_RESET);
        return;
    }

    int qType, aType;
    printAppHeader(" THIẾT LẬP BÀI THI TỪ VỰNG ");
    printf(CLR_TEXT " [1]. Kanji    [2]. Furigana\n [3]. Romaji   [4]. Ý nghĩa\n\n" CLR_RESET);
    printf(BOLD " ➤ Chọn loại CÂU HỎI: " CLR_RESET); scanf("%d", &qType);
    printf(BOLD " ➤ Chọn loại TRẢ LỜI: " CLR_RESET); scanf("%d", &aType);
    getchar();

    srand(time(NULL));
    int score = 0;

    for (int i = 0; i < list->vocabCount; i++) {
        printAppHeader(" ĐANG LÀM BÀI TRẮC NGHIỆM ");
        
        // Thanh tiến trình (Progress Bar) giả
        printf(CLR_WARN " Tiến trình: %d/%d ", i + 1, list->vocabCount);
        for(int p=0; p<20; p++) printf(p < (i+1)*20/list->vocabCount ? "■" : "□");
        printf("\n\n" CLR_RESET);

        char *questionText = getVocabDataByType(&list->arr[i], qType);
        char *correctAnswerText = getVocabDataByType(&list->arr[i], aType);

        // Hiển thị câu hỏi trong khung
        printf(" ┌────────────────────────────────────────────────────────┐\n");
        printf(" │ " BOLD "CÂU HỎI: %-45s" CLR_RESET " │\n", questionText);
        printf(" └────────────────────────────────────────────────────────┘\n\n");

        char *options[4];
        int correctIdx = rand() % 4;
        options[correctIdx] = correctAnswerText;
        for (int j = 0; j < 4; j++) {
            if (j == correctIdx) continue;
            int randIdx;
            do {
                randIdx = rand() % list->vocabCount;
            } while (randIdx == i || strcmp(getVocabDataByType(&list->arr[randIdx], aType), correctAnswerText) == 0);
            options[j] = getVocabDataByType(&list->arr[randIdx], aType);
        }

        for (int j = 0; j < 4; j++) 
            printf("   " CLR_PRIMARY BOLD "[%d]" CLR_RESET " %s\n", j + 1, options[j]);

        int choice;
        printf("\n " BOLD "➤ Lựa chọn của bạn (0 để thoát): " CLR_RESET);
        if (scanf("%d", &choice) != 1 || choice == 0) break;

        if (choice - 1 == correctIdx) {
            printf("\n   " CLR_SUCCESS BOLD "✔ CHÍNH XÁC!" CLR_RESET "\n");
            score++;
        } else {
            printf("\n   " CLR_ERROR BOLD "✘ SAI RỒI!" CLR_RESET " Đáp án đúng: " CLR_SUCCESS "%s" CLR_RESET "\n", correctAnswerText);
        }

        // Card hiển thị thông tin chi tiết
        printf("\n " CLR_PRIMARY "ℹ Chi tiết: " CLR_RESET BOLD "%s " CLR_RESET "(%s) : %s\n", 
               list->arr[i].vocab, list->arr[i].furigana, list->arr[i].meaning);
        
        printf(CLR_WARN "\n -- Nhấn Enter để sang câu tiếp theo --" CLR_RESET);
        getchar(); getchar();
    }

    // Màn hình kết quả
    printAppHeader(" KẾT QUẢ BÀI THI ");
    printf("\n" BOLD "   ĐIỂM CỦA BẠN: " CLR_SUCCESS "%d/%d" CLR_RESET "\n", score, list->vocabCount);
    float rate = (float)score/list->vocabCount;
    if(rate >= 0.8) printf("   ĐÁNH GIÁ: " CLR_SUCCESS "XUẤT SẮC! ✨" CLR_RESET "\n");
    else if(rate >= 0.5) printf("   ĐÁNH GIÁ: " CLR_WARN "TỐT, CẦN CỐ GẮNG! 👍" CLR_RESET "\n");
    else printf("   ĐÁNH GIÁ: " CLR_ERROR "CẦN HỌC LẠI BÀI! ✍️" CLR_RESET "\n");
    
    printf("\n Nhấn Enter để quay lại menu.");
    getchar();
}

void runChoiceOption(KanjiList *L) {
    if (!L || L->kanjiCount == 0) return;
    
    printAppHeader(" CHỌN BÀI HỌC ");
    int totalLessons = (L->kanjiCount + KANJI_PER_LESSON - 1) / KANJI_PER_LESSON;
    printf(CLR_TEXT " Hiện có " BOLD "%d" CLR_RESET CLR_TEXT " bài học trong dữ liệu.\n", totalLessons);
    printf(BOLD " ➤ Nhập số bài [1-%d]: " CLR_RESET, totalLessons);
    int lessonChoice;
    scanf("%d", &lessonChoice); getchar();

    int start = (lessonChoice - 1) * KANJI_PER_LESSON;
    int end = start + KANJI_PER_LESSON;
    if (end > L->kanjiCount) end = L->kanjiCount;

    printAppHeader(" CHẾ ĐỘ LUYỆN TẬP ");
    printf("   " CLR_PRIMARY BOLD "[1]." CLR_RESET " Trắc nghiệm Kanji (Hán Việt)\n");
    printf("   " CLR_PRIMARY BOLD "[2]." CLR_RESET " Trắc nghiệm Từ vựng\n");
    printf("   " CLR_ERROR BOLD "[0]." CLR_RESET " Quay lại\n");
    printf("\n " BOLD "➤ Lựa chọn của bạn: " CLR_RESET);

    int mode;
    if (scanf("%d", &mode) != 1 || mode == 0) return;

    if (mode == 1) {
        KanjiMCList *mcList = mapToKanjiMCRange(L, start, end);
        runMultipleChoiceTestKanji(mcList);
        freeKanjiMCList(mcList);
    } else if (mode == 2) {
        VocabMCList *vList = mapToVocabMCRange(L, start, end);
        runMultipleChoiceTestVocab(vList); 
        freeVocabMCList(vList);
    }
}