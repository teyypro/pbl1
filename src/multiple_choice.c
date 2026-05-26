// ==================== src/multiple_choice.c ====================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "multiple_choice.h"
#include "../data_structures.h"
#include "utils.h"

#define KANJI_PER_LESSON 16
#define MAX_INPUT_SIZE   65536
#define LINE_BUFFER_SIZE 4096
#define MIN_REQUIRED_QA  4

// Cấu trúc lưu dữ liệu thô nhập từ bàn phím
typedef struct {
    char **data;
    int count;
} RawDataList;

// ==========================================
// THÀNH PHẦN QUẢN LÝ BỘ NHỚ (MEMORY MANAGEMENT)
// ==========================================

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

void freeRawDataList(RawDataList *rawList) {
    if (!rawList) return;
    if (rawList->data) {
        for (int i = 0; i < rawList->count; i++) {
            free(rawList->data[i]);
        }
        free(rawList->data);
    }
    free(rawList);
}

// ==========================================
// THÀNH PHẦN CHUYỂN ĐỔI DỮ LIỆU (MAPPING & CONVERSION)
// ==========================================

KanjiMCList* mapToKanjiMCRange(KanjiList *L, int start, int end) {
    if (!L || start < 0 || end > L->kanjiCount || start >= end) return NULL;

    KanjiMCList *mcList = malloc(sizeof(KanjiMCList));
    if (!mcList) return NULL;

    mcList->kanjiCount = end - start;
    mcList->arr = malloc(mcList->kanjiCount * sizeof(KanjiMC));
    if (!mcList->arr) {
        free(mcList);
        return NULL;
    }

    for (int i = 0; i < mcList->kanjiCount; i++) {
        mcList->arr[i].kanji   = strdup(L->kanjis[start + i].kanji);
        mcList->arr[i].hanViet = strdup(L->kanjis[start + i].hanViet);
    }
    return mcList;
}

VocabMCList* mapToVocabMCRange(KanjiList *L, int start, int end) {
    if (!L || start < 0 || end > L->kanjiCount || start >= end) return NULL;

    VocabMCList *vList = malloc(sizeof(VocabMCList));
    if (!vList) return NULL;

    vList->vocabCount = 0;
    for (int i = start; i < end; i++) {
        vList->vocabCount += L->kanjis[i].vocabsCount;
    }

    vList->arr = malloc(vList->vocabCount * sizeof(VocabMC));
    if (!vList->arr) {
        free(vList);
        return NULL;
    }

    int index = 0;
    for (int i = start; i < end; i++) {
        for (int j = 0; j < L->kanjis[i].vocabsCount; j++) {
            Vocab *v = &L->kanjis[i].vocabs[j];
            vList->arr[index].vocab    = strdup(v->vocab);
            vList->arr[index].furigana = strdup(v->hiragana);
            vList->arr[index].romaji   = strdup(v->romaji);
            vList->arr[index].meaning  = strdup(v->meaning);
            index++;
        }
    }
    return vList;
}

char* getKanjiDataByType(KanjiMC *item, int type) {
    return (type == 1) ? item->kanji : item->hanViet;
}

char* getVocabDataByType(VocabMC *item, int type) {
    switch (type) {
        case 1:  return item->vocab;
        case 2:  return item->furigana;
        case 3:  return item->romaji;
        case 4:  return item->meaning;
        default: return item->vocab;
    }
}

// ==========================================
// TRÌNH ĐIỀU HƯỚNG NHẬP LIỆU AN TOÀN (SAFE INPUT)
// ==========================================

static int safeReadInt(int minVal, int maxVal, int *outValue) {
    char buffer[128];
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return 0;
    }
    buffer[strcspn(buffer, "\r\n")] = '\0';
    
    char *endptr;
    long val = strtol(buffer, &endptr, 10);
    if (endptr == buffer || *endptr != '\0') {
        return 0; 
    }
    if (val < minVal || val > maxVal) {
        return 0;
    }
    *outValue = (int)val;
    return 1;
}

// ==========================================
// LÕI LOGIC TRẮC NGHIỆM (TEST CORE LOGIC)
// ==========================================

void runMultipleChoiceTestKanji(KanjiMCList *list) {
    if (!list || list->kanjiCount < MIN_REQUIRED_QA) {
        printf("\n  " CL_PRIMARY "┃ " CL_ERROR "LỖI HỆ THỐNG: Cần tối thiểu %d câu hỏi để khởi tạo.\n" RESET, MIN_REQUIRED_QA);
        return;
    }

    int qType = 1, aType = 2;
    clearScreen();
    
    printf("\n  " CL_PRIMARY "┃ " BG_HIGHLIGHT BOLD " CẤU HÌNH KIỂM TRA KANJI " RESET "\n");
    printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
    printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "[1]" RESET " Kanji\n");
    printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "[2]" RESET " Hán Việt\n");
    printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
    
    do {
        printf("  " CL_PRIMARY "┃ Chọn dạng CÂU HỎI [1-2]: ");
    } while (!safeReadInt(1, 2, &qType));

    do {
        printf("  " CL_PRIMARY "┃ Chọn dạng CÂU TRẢ LỜI [1-2]: ");
    } while (!safeReadInt(1, 2, &aType));

    srand((unsigned int)time(NULL));
    int *correctFlags = calloc(list->kanjiCount, sizeof(int));
    if (!correctFlags) return;

    int totalCorrect = 0, roundCount = 0, stop = 0;
    char tempBuf[32];
    
    printf("\n  " CL_PRIMARY "┃ " CL_SUCCESS "✔ Cấu hình hoàn tất! Nhập '0' ở bất kỳ câu nào để thoát." RESET);
    printf("\n  " CL_PRIMARY "┃ >> Nhấn Enter để bắt đầu bài thi...");
    fgets(tempBuf, sizeof(tempBuf), stdin);
    
    while (totalCorrect < list->kanjiCount && !stop) {
        roundCount++;
        int roundScore = 0;
        int questionsInRound = 0;
        
        for (int i = 0; i < list->kanjiCount && !stop; i++) {
            if (correctFlags[i] == 1) continue;
            
            questionsInRound++;
            clearScreen();
            
            int percent = (int)(((float)totalCorrect / list->kanjiCount) * 100);
            printf("\n  " CL_PRIMARY "┃ " BOLD "Vòng: #%02d" RESET "  │  " CL_SUCCESS "Số câu đúng: %d/%d (%d%%)" RESET "\n", roundCount, totalCorrect, list->kanjiCount, percent);
            printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");

            char *questionText = getKanjiDataByType(&list->arr[i], qType);
            char *correctAnswerText = getKanjiDataByType(&list->arr[i], aType);

            printf("  " CL_PRIMARY "┃ " CL_KANJI BOLD " %s " RESET , questionText);
            printf(CL_TEXT "tương ứng với ? \n" RESET);
            printf(CL_PRIMARY " ┃ \n");

            char *options[4];
            int correctIdx = rand() % 4;
            options[correctIdx] = correctAnswerText;

            for (int j = 0; j < 4; j++) {
                if (j == correctIdx) continue;
                int randIdx, isDuplicate;
                do {
                    isDuplicate = 0;
                    randIdx = rand() % list->kanjiCount;
                    if (randIdx == i) { isDuplicate = 1; continue; }
                    
                    char *tempAnswer = getKanjiDataByType(&list->arr[randIdx], aType);
                    if (strcmp(tempAnswer, correctAnswerText) == 0) { isDuplicate = 1; continue; }
                    
                    for (int k = 0; k < j; k++) {
                        if (k == correctIdx) continue;
                        if (options[k] && strcmp(tempAnswer, options[k]) == 0) {
                            isDuplicate = 1;
                            break;
                        }
                    }
                } while (isDuplicate);
                options[j] = getKanjiDataByType(&list->arr[randIdx], aType);
            }

            for (int j = 0; j < 4; j++) {
                printf("  " CL_PRIMARY "┃" RESET "    " CL_KEY BOLD "[%d]" RESET "  %s\n", j + 1, options[j]);
            }

            int choice = -1;
            while (1) {
                printf("\n  " CL_PRIMARY "┃ " CL_DIM ">> Bạn chọn [0-4]: " RESET);
                if (safeReadInt(0, 4, &choice)) break;
                printf("  " CL_PRIMARY "┃ " CL_ERROR "❌ Vui lòng chỉ nhập số thứ tự từ 1 đến 4!" RESET "\n");
            }

            if (choice == 0) {
                stop = 1;
                break;
            }
            
            printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
            if (choice - 1 == correctIdx) {
                printf("  " CL_PRIMARY "┃ " CL_SUCCESS "🎉 CHÍNH XÁC!" RESET "\n");
                correctFlags[i] = 1;
                totalCorrect++;
                roundScore++;
            } else {
                printf("  " CL_PRIMARY "┃ " CL_ERROR "❌ CHƯA ĐÚNG!" RESET "\n");
                printf("  " CL_PRIMARY "┃ >> Đáp án chính xác: " CL_SUCCESS BOLD "%s" RESET "\n", correctAnswerText);
            }
            printf("  " CL_PRIMARY "┃ >> " CL_KANJI "%s" RESET " - " CL_TEXT "%s" RESET "\n", list->arr[i].kanji, list->arr[i].hanViet);
            printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
            
            if (totalCorrect < list->kanjiCount && !stop) {
                printf("\n  >> Nhấn Enter để tiếp tục...");
                fgets(tempBuf, sizeof(tempBuf), stdin);
            }
        }
        
        if (!stop && totalCorrect < list->kanjiCount) {
            clearScreen();
            printf("\n  " CL_PRIMARY "┃ " BG_HIGHLIGHT BOLD " KẾT QUẢ VÒNG LUYỆN TẬP " RESET "\n");
            printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
            printf("  " CL_PRIMARY "┃ • Số câu xử lý trong lượt : %d\n", questionsInRound);
            printf("  " CL_PRIMARY "┃ • Số câu trả lời đúng     : " CL_SUCCESS BOLD "%d" RESET "\n", roundScore);
            printf("  " CL_PRIMARY "┃ • Số câu trả lời sai      : " CL_ERROR BOLD "%d" RESET "\n", questionsInRound - roundScore);
            printf("  " CL_PRIMARY "┃ • Tổng tiến độ hiện tại   : %d/%d\n", totalCorrect, list->kanjiCount);
            printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
            
            int reviewChoice = 0;
            printf("  " CL_PRIMARY "┃ Bạn có muốn làm lại các câu sai?\n");
            printf("  " CL_PRIMARY "┃ " CL_KEY "[1] Có, tiếp tục" RESET "  │  " CL_DIM "[0] Không, dừng lại" RESET "\n");
            printf("  " CL_PRIMARY "┃ >> Lựa chọn của bạn: ");
            if (!safeReadInt(0, 1, &reviewChoice) || reviewChoice == 0) {
                stop = 1;
            }
        }
    }
    
    clearScreen();
    printf("\n  " CL_PRIMARY "┃ " BG_HIGHLIGHT BOLD " BÁO CÁO KẾT THÚC BÀI HỌC " RESET "\n");
    printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
    if (stop && totalCorrect < list->kanjiCount) {
        printf("  " CL_PRIMARY "┃ " CL_WARN "🚨 ĐÃ TẠM DỪNG TIẾN TRÌNH" RESET "\n");
        printf("  " CL_PRIMARY "┃ • Số vòng đã thực hiện : %d\n", roundCount);
        printf("  " CL_PRIMARY "┃ • Kết quả hoàn thành   : " CL_PRIMARY "%d/%d" RESET " mục.\n", totalCorrect, list->kanjiCount);
    } else {
        printf("  " CL_PRIMARY "┃ " CL_SUCCESS "🎉 HOÀN THÀNH XUẤT SẮC!" RESET "\n");
        printf("  " CL_PRIMARY "┃ • Bạn đã trả lời đúng toàn bộ " CL_SUCCESS BOLD "%d/%d" RESET " câu hỏi Kanji.\n", totalCorrect, list->kanjiCount);
        printf("  " CL_PRIMARY "┃ • Tổng số lượt thực hiện: %d\n", roundCount);
    }
    printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
    
    free(correctFlags);
    printf("\n  >> Nhấn Enter để quay lại Menu.");
    fgets(tempBuf, sizeof(tempBuf), stdin);
}

void runMultipleChoiceTestVocab(VocabMCList *list) {
    if (!list || list->vocabCount < MIN_REQUIRED_QA) {
        printf("\n  " CL_PRIMARY "┃ " CL_ERROR "LỖI HỆ THỐNG: Cần tối thiểu %d từ vựng để khởi tạo.\n" RESET, MIN_REQUIRED_QA);
        return;
    }

    int qType = 1, aType = 4;
    clearScreen();
    
    printf("\n  " CL_PRIMARY "┃ " BG_HIGHLIGHT BOLD " CẤU HÌNH KIỂM TRA TỪ VỰNG " RESET "\n");
    printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
    printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "[1]" RESET " Từ vựng gốc (Kanji/Kana)  " CL_KEY BOLD "[2]" RESET " Cách đọc Furigana\n");
    printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "[3]" RESET " Phiên âm Romaji           " CL_KEY BOLD "[4]" RESET " Nghĩa Tiếng Việt\n");
    printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
    
    do {
        printf("  " CL_PRIMARY "┃ Chọn dạng CÂU HỎI [1-4]: ");
    } while (!safeReadInt(1, 4, &qType));

    do {
        printf("  " CL_PRIMARY "┃ Chọn dạng CÂU TRẢ LỜI [1-4]: ");
    } while (!safeReadInt(1, 4, &aType));

    srand((unsigned int)time(NULL));
    int *correctFlags = calloc(list->vocabCount, sizeof(int));
    if (!correctFlags) return;

    int totalCorrect = 0, roundCount = 0, stop = 0;
    char tempBuf[32];
    
    printf("\n  " CL_PRIMARY "┃ " CL_SUCCESS "✔ Cấu hình hoàn tất!" RESET);
    printf("\n  " CL_PRIMARY "┃ >> Nhấn Enter để bắt đầu...");
    fgets(tempBuf, sizeof(tempBuf), stdin);
    
    while (totalCorrect < list->vocabCount && !stop) {
        roundCount++;
        int roundScore = 0;
        int questionsInRound = 0;
        
        for (int i = 0; i < list->vocabCount && !stop; i++) {
            if (correctFlags[i] == 1) continue;
            
            questionsInRound++;
            clearScreen();
            
            int percent = (int)(((float)totalCorrect / list->vocabCount) * 100);
            printf("\n  " CL_PRIMARY "┃ " BOLD "Vòng: #%02d" RESET "  │  " CL_SUCCESS "Số câu đúng: %d/%d (%d%%)" RESET "\n", roundCount, totalCorrect, list->vocabCount, percent);
            printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");

            char *questionText = getVocabDataByType(&list->arr[i], qType);
            char *correctAnswerText = getVocabDataByType(&list->arr[i], aType);

            printf("  " CL_PRIMARY "┃ " CL_KANA BOLD " %s " RESET , questionText);
            printf(CL_TEXT "tương ứng với ? \n " RESET );
            printf(CL_PRIMARY " ┃ \n");

            char *options[4];
            int correctIdx = rand() % 4;
            options[correctIdx] = correctAnswerText;

            for (int j = 0; j < 4; j++) {
                if (j == correctIdx) continue;
                int randIdx, isDuplicate;
                do {
                    isDuplicate = 0;
                    randIdx = rand() % list->vocabCount;
                    if (randIdx == i) { isDuplicate = 1; continue; }
                    
                    char *tempAnswer = getVocabDataByType(&list->arr[randIdx], aType);
                    if (strcmp(tempAnswer, correctAnswerText) == 0) { isDuplicate = 1; continue; }
                    
                    for (int k = 0; k < j; k++) {
                        if (k == correctIdx) continue;
                        if (options[k] && strcmp(tempAnswer, options[k]) == 0) {
                            isDuplicate = 1;
                            break;
                        }
                    }
                } while (isDuplicate);
                options[j] = getVocabDataByType(&list->arr[randIdx], aType);
            }

            for (int j = 0; j < 4; j++) {
                printf("  " CL_PRIMARY "┃" RESET "    " CL_KEY BOLD "[%d]" RESET "  %s\n", j + 1, options[j]);
            }

            int choice = -1;
            while (1) {
                printf("\n  " CL_PRIMARY "┃ " CL_DIM ">> Bạn chọn [0-4]: " RESET);
                if (safeReadInt(0, 4, &choice)) break;
                printf("  " CL_PRIMARY "┃ " CL_ERROR "❌ Vui lòng chỉ nhập số thứ tự từ 1 đến 4!" RESET "\n");
            }

            if (choice == 0) {
                stop = 1;
                break;
            }
            
            printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
            if (choice - 1 == correctIdx) {
                printf("  " CL_PRIMARY "┃ " CL_SUCCESS "🎉 CHÍNH XÁC!" RESET "\n");
                correctFlags[i] = 1;
                totalCorrect++;
                roundScore++;
            } else {
                printf("  " CL_PRIMARY "┃ " CL_ERROR "❌ CHƯA ĐÚNG!" RESET "\n");
                printf("  " CL_PRIMARY "┃ >> Đáp án chính xác: " CL_SUCCESS BOLD "%s" RESET "\n", correctAnswerText);
            }
            printf("  " CL_PRIMARY "┃ >> " CL_KANA "%s" RESET " (%s - %s) " CL_TEXT "%s" RESET "\n", 
                   list->arr[i].vocab, list->arr[i].furigana, list->arr[i].romaji, list->arr[i].meaning);
            printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
            
            if (totalCorrect < list->vocabCount && !stop) {
                printf("\n  >> Nhấn Enter để tiếp tục...");
                fgets(tempBuf, sizeof(tempBuf), stdin);
            }
        }
        
        if (!stop && totalCorrect < list->vocabCount) {
            clearScreen();
            printf("\n  " CL_PRIMARY "┃ " BG_HIGHLIGHT BOLD " KẾT QUẢ VÒNG LUYỆN TẬP " RESET "\n");
            printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
            printf("  " CL_PRIMARY "┃ • Đúng trong vòng này: " CL_SUCCESS BOLD "%d / %d" RESET " câu\n", roundScore, questionsInRound);
            printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
            
            int reviewChoice = 0;
            printf("  " CL_PRIMARY "┃ Bạn có muốn làm lại các câu đã sai?\n");
            printf("  " CL_PRIMARY "┃ " CL_KEY "[1] Đồng ý" RESET "  │  " CL_DIM "[0] Thoát" RESET "\n");
            printf("  " CL_PRIMARY "┃ >> Lựa chọn của bạn: ");
            if (!safeReadInt(0, 1, &reviewChoice) || reviewChoice == 0) {
                stop = 1;
            }
        }
    }
    
    clearScreen();
    printf("\n  " CL_PRIMARY "┃ " BG_HIGHLIGHT BOLD " BÁO CÁO KẾT THÚC BÀI HỌC " RESET "\n");
    printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
    if (stop && totalCorrect < list->vocabCount) {
        printf("  " CL_PRIMARY "┃ " CL_WARN "🚨 ĐÃ TẠM DỪNG TIẾN TRÌNH" RESET "\n");
        printf("  " CL_PRIMARY "┃ • Số vòng đã thực hiện : %d\n", roundCount);
        printf("  " CL_PRIMARY "┃ • Kết quả hoàn thành   : " CL_PRIMARY "%d/%d" RESET " mục.\n", totalCorrect, list->vocabCount);
    } else {
        printf("  " CL_PRIMARY "┃ " CL_SUCCESS "🎉 HOÀN THÀNH XUẤT SẮC!" RESET "\n");
        printf("  " CL_PRIMARY "┃ • Bạn đã trả lời đúng toàn bộ " CL_SUCCESS BOLD "%d/%d" RESET " câu hỏi.\n", totalCorrect, list->vocabCount);
        printf("  " CL_PRIMARY "┃ • Tổng số lượt thực hiện: %d\n", roundCount);
    }
    printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
    
    free(correctFlags);
    printf("\n  >> Nhấn Enter để quay lại Menu.");
    fgets(tempBuf, sizeof(tempBuf), stdin);
}

// ==========================================
// THÀNH PHẦN XỬ LÝ NHẬP LIỆU TỪ BÀN PHÍM (RAW BUFFER)
// ==========================================

RawDataList* readDataFromKeyboard(void) {
    RawDataList *rawList = malloc(sizeof(RawDataList));
    if (!rawList) return NULL;

    rawList->data = NULL;
    rawList->count = 0;

    clearScreen();
    printf("\n  " CL_PRIMARY "┃ " BG_HIGHLIGHT BOLD " TRÌNH NHẬP DỮ LIỆU THỦ CÔNG " RESET "\n");
    printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
    printf("  " CL_PRIMARY "┃ " CL_DIM "Hỗ trợ copy/paste trực tiếp từ Excel hoặc Google Sheets." RESET "\n");
    printf("  " CL_PRIMARY "┃ " CL_DIM "Định dạng: Các trường ngăn cách bằng ký tự [TAB]." RESET "\n");
    printf("  " CL_PRIMARY "┃ " CL_DIM "- Du lieu Kanji: Kanji    Hán Việt" RESET "\n" RESET);
    printf("  " CL_PRIMARY "┃ " CL_DIM "- Du lieu Tu vung: Kanji    Furigana    Romaji    Nghĩa" RESET "\n" RESET);
    printf("  " CL_PRIMARY "┃ " CL_DIM "⏹  Kết thúc lệnh: Nhấn [Enter] 2 lần liên tiếp." RESET "\n");
    printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
    printf("  " CL_PRIMARY "┃ 😏 Hãy paste dữ liệu của bạn vào bên dưới:\n\n");

    char lineBuffer[LINE_BUFFER_SIZE];
    int emptyLineCount = 0;

    while (1) {
        printf("  ");
        inputString(lineBuffer, sizeof(lineBuffer));

        char *trimmed = lineBuffer;
        while (*trimmed == ' ' || *trimmed == '\t') trimmed++;

        if (strlen(trimmed) == 0) {
            emptyLineCount++;
            if (emptyLineCount >= 2) {
                break;
            }
            continue;
        }

        emptyLineCount = 0;

        rawList->data = realloc(rawList->data, (rawList->count + 1) * sizeof(char*));
        if (!rawList->data) {
            freeRawDataList(rawList);
            return NULL;
        }

        rawList->data[rawList->count] = strdup(trimmed);
        rawList->count++;
    }

    if (rawList->count == 0) {
        printf(CL_WARN "\n  ⚠ Khong co du lieu nao duoc nhap!\n" RESET);
        free(rawList);
        return NULL;
    }

    printf(CL_SUCCESS "\n  ✓ Da nhan %d dong du lieu.\n" RESET, rawList->count);
    return rawList;
}

KanjiMCList* convertToKanjiMCList(RawDataList *rawList) {
    if (!rawList || rawList->count == 0) return NULL;
    
    KanjiMCList *mcList = malloc(sizeof(KanjiMCList));
    if (!mcList) return NULL;
    
    mcList->kanjiCount = rawList->count;
    mcList->arr = malloc(mcList->kanjiCount * sizeof(KanjiMC));
    if (!mcList->arr) {
        free(mcList);
        return NULL;
    }
    
    for (int i = 0; i < rawList->count; i++) {
        char *lineCopy = strdup(rawList->data[i]);
        mcList->arr[i].kanji = NULL;
        mcList->arr[i].hanViet = NULL;
        
        char *token = strtok(lineCopy, "\t");
        if (token) {
            mcList->arr[i].kanji = strdup(token);
            token = strtok(NULL, "\t");
            if (token) {
                mcList->arr[i].hanViet = strdup(token);
            }
        }
        
        if (!mcList->arr[i].kanji)   mcList->arr[i].kanji = strdup("");
        if (!mcList->arr[i].hanViet) mcList->arr[i].hanViet = strdup("");
        
        free(lineCopy);
    }
    return mcList;
}

VocabMCList* convertToVocabMCList(RawDataList *rawList) {
    if (!rawList || rawList->count == 0) return NULL;
    
    VocabMCList *vList = malloc(sizeof(VocabMCList));
    if (!vList) return NULL;
    
    vList->vocabCount = rawList->count;
    vList->arr = malloc(vList->vocabCount * sizeof(VocabMC));
    if (!vList->arr) {
        free(vList);
        return NULL;
    }
    
    for (int i = 0; i < rawList->count; i++) {
        char *lineCopy = strdup(rawList->data[i]);
        vList->arr[i].vocab    = NULL;
        vList->arr[i].furigana = NULL;
        vList->arr[i].romaji   = NULL;
        vList->arr[i].meaning  = NULL;
        
        char *token = strtok(lineCopy, "\t");
        int fieldIndex = 0;
        
        while (token != NULL && fieldIndex < 4) {
            switch (fieldIndex) {
                case 0: vList->arr[i].vocab    = strdup(token); break;
                case 1: vList->arr[i].furigana = strdup(token); break;
                case 2: vList->arr[i].romaji   = strdup(token); break;
                case 3: vList->arr[i].meaning  = strdup(token); break;
            }
            token = strtok(NULL, "\t");
            fieldIndex++;
        }
        
        if (!vList->arr[i].vocab)    vList->arr[i].vocab    = strdup("");
        if (!vList->arr[i].furigana) vList->arr[i].furigana = strdup("");
        if (!vList->arr[i].romaji)   vList->arr[i].romaji   = strdup("");
        if (!vList->arr[i].meaning)  vList->arr[i].meaning  = strdup("");
        
        free(lineCopy);
    }
    return vList;
}

// ==========================================
// GIAO DIỆN ĐIỀU HƯỚNG CHÍNH (MAIN MENU CONTEXT)
// ==========================================

void runMultipleChoiceWithKeyboardInput(void) {
    RawDataList *rawList = readDataFromKeyboard();
    char menuBuf[32];
    if (!rawList) {
        printf("\n  >> Nhấn Enter để quay về...");
        fgets(menuBuf, sizeof(menuBuf), stdin);
        return;
    }
    
    while (1) {
        clearScreen();
        printf("\n  " CL_PRIMARY "┃ " BG_HIGHLIGHT BOLD " LỰA CHỌN CHẾ ĐỘ KIỂM TRA " RESET "\n");
        printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
        printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "[1]" RESET " Trắc nghiệm Kanji (Kanji / Hán Việt) \n");
        printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "[2]" RESET " Trắc nghiệm Từ vựng (Kanji / Kana / Romaji / Tiếng Việt)\n");
        printf("  " CL_PRIMARY "┃ " CL_DIM BOLD "[0]" RESET " Quay lại\n");
        printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
        
        int mode = 0;
        printf("\n  " CL_PRIMARY "┃ >> Chọn [0-2]: ");
        if (!safeReadInt(0, 2, &mode) || mode == 0) {
            freeRawDataList(rawList);
            return;
        }
        
        if (mode == 1) {
            KanjiMCList *mcList = convertToKanjiMCList(rawList);
            if (mcList && mcList->kanjiCount >= MIN_REQUIRED_QA) {
                runMultipleChoiceTestKanji(mcList);
            } else {
                printf("\n  " CL_PRIMARY "┃ " CL_ERROR "❌ THẤT BẠI: Dữ liệu chứa %d mục (Yêu cầu ≥ %d)." RESET "\n", mcList ? mcList->kanjiCount : 0, MIN_REQUIRED_QA);
                fgets(menuBuf, sizeof(menuBuf), stdin);
            }
            freeKanjiMCList(mcList);
        } else if (mode == 2) {
            VocabMCList *vList = convertToVocabMCList(rawList);
            if (vList && vList->vocabCount >= MIN_REQUIRED_QA) {
                runMultipleChoiceTestVocab(vList);
            } else {
                printf("\n  " CL_PRIMARY "┃ " CL_ERROR "❌ THẤT BẠI: Dữ liệu chứa %d mục (Yêu cầu ≥ %d)." RESET "\n", vList ? vList->vocabCount : 0, MIN_REQUIRED_QA);
                fgets(menuBuf, sizeof(menuBuf), stdin);
            }
            freeVocabMCList(vList);
        }
    }
}

void runChoiceOption(KanjiList *L) {
    if (!L || L->kanjiCount == 0) return;
    int totalLessons = (L->kanjiCount + KANJI_PER_LESSON - 1) / KANJI_PER_LESSON;
    
    clearScreen();
    printf("\n  " CL_PRIMARY "┃ " BG_HIGHLIGHT BOLD " HỆ THỐNG TẠO TRẮC NGHIỆM " RESET "\n");
    printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
    printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "[1]" RESET " Chọn bài học trong sách [Bài 1 - %d]\n", totalLessons);
    printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "[2]" RESET " Nạp dữ liệu thủ công (Keyboard Paste)\n");
    printf("  " CL_PRIMARY "┃ " CL_DIM BOLD "[0]" RESET " Quay lại\n");
    printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
    
    int mainChoice = 0;
    printf("\n  " CL_PRIMARY "┃ >> Chọn phương thức: ");
    if (!safeReadInt(0, 2, &mainChoice) || mainChoice == 0) {
        return;
    }
    
    if (mainChoice == 2) {
        runMultipleChoiceWithKeyboardInput();
        return;
    }
    
    int lessonChoice = 0;
    while (1) {
        printf("  " CL_PRIMARY "┃ >> Chọn bài học [1-%d]: ", totalLessons);
        if (safeReadInt(1, totalLessons, &lessonChoice)) break;
        printf("  " CL_PRIMARY "┃ " CL_ERROR "❌ Bài học không tồn tại. Vui lòng chọn lại!\" RESET \"\\n\");\n");
    }

    int start = (lessonChoice - 1) * KANJI_PER_LESSON;
    int end   = start + KANJI_PER_LESSON;
    if (end > L->kanjiCount) end = L->kanjiCount;

    clearScreen();
    printf("\n  " CL_PRIMARY "┃ " BG_HIGHLIGHT BOLD " LỰA CHỌN PHÂN HỆ KIỂM TRA " RESET "\n");
    printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
    printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "[1]" RESET " Trắc nghiệm Kanji (Kanji / Hán Việt) \n");
    printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "[2]" RESET " Trắc nghiệm Từ vựng (Kanji / Kana / Romaji / Tiếng Việt)\n");
    printf("  " CL_PRIMARY "┃ " CL_DIM BOLD "[0]" RESET " Quay lại\n");
    printf("  " CL_PRIMARY "┃ ──────────────────────────────────────────────────────────────\n");
    
    int mode = 0;
    printf("\n  " CL_PRIMARY "┃ >> Chọn chế độ: ");
    if (!safeReadInt(0, 2, &mode) || mode == 0) return;

    if (mode == 1) {
        KanjiMCList *mcList = mapToKanjiMCRange(L, start, end);
        if (mcList) {
            runMultipleChoiceTestKanji(mcList);
            freeKanjiMCList(mcList);
        }
    } else if (mode == 2) {
        VocabMCList *vList = mapToVocabMCRange(L, start, end);
        if (vList) {
            runMultipleChoiceTestVocab(vList); 
            freeVocabMCList(vList);
        }
    }
}