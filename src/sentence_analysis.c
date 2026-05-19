// ==================== src/sentence_analysis.c ====================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#ifdef _WIN32
    #include <windows.h>
#endif

#include "../data_structures.h"
#include "filter_learned_lesson.h"  
#include "utils.h"

#define MAX_SENTENCE 1024

typedef struct KanjiNode {
    Kanji* kanjiData;
    struct KanjiNode* next;
} KanjiNode;

typedef struct {
    KanjiNode* buckets[512];
} KanjiHashTable;

unsigned int hashKanjiChar(wchar_t wc) {
    return ((unsigned int)wc * 37) % 512;
}

KanjiHashTable* createKanjiHashTable(KanjiList *allData) {
    KanjiHashTable* table = (KanjiHashTable*)calloc(1, sizeof(KanjiHashTable));
    if (!table) return NULL;

    for (int i = 0; i < allData->kanjiCount; i++) {
        wchar_t* kw = allData->kanjis[i].kanji_w;
        if (!kw || !kw[0]) continue;

        unsigned int idx = hashKanjiChar(kw[0]);

        KanjiNode* node = (KanjiNode*)malloc(sizeof(KanjiNode));
        if (node) {
            node->kanjiData = &allData->kanjis[i];
            node->next = table->buckets[idx];
            table->buckets[idx] = node;
        }
    }
    return table;
}

Kanji* findKanji(KanjiHashTable* table, wchar_t wc) {
    if (!table) return NULL;

    unsigned int idx = hashKanjiChar(wc);
    KanjiNode* curr = table->buckets[idx];

    while (curr) {
        if (curr->kanjiData->kanji_w[0] == wc)
            return curr->kanjiData;
        curr = curr->next;
    }
    return NULL;
}

void freeKanjiHashTable(KanjiHashTable* table) {
    if (!table) return;
    for (int i = 0; i < 512; i++) {
        KanjiNode* curr = table->buckets[i];
        while (curr) {
            KanjiNode* temp = curr;
            curr = curr->next;
            free(temp);
        }
    }
    free(table);
}

void printOutKanjiInfo(Kanji* info) {
    if (!info) {
        printf(CL_WARN "\n  ⚠ Không có thông tin Kanji.\n" RESET);
        return;
    }
    
    printf("\n  " BG_HIGHLIGHT " KANJI INFO " RESET "\n");
    printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
    printf(CL_BORDER "  │ " RESET "Chữ: " CL_LOGO BOLD "%-10s" RESET 
           " Hán Việt: " CL_KEY BOLD "%-28s" RESET CL_BORDER "│\n" RESET, 
           info->kanji, (info->hanViet ? info->hanViet : "?"));
    printf(CL_BORDER "  │ " RESET "Số thứ tự: " CL_TEXT "%-49d" RESET CL_BORDER "│\n" RESET, info->stt);
    printf(CL_BORDER "  ├────────────────────────────────────────────────────────────┤\n" RESET);
    printf(CL_BORDER "  │ " RESET BOLD "Giải nghĩa: " RESET "                                               " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  │ " RESET CL_TEXT "%-58s" RESET CL_BORDER " │\n" RESET, 
           (info->description ? info->description : "Chưa có mô tả."));
    printf(CL_BORDER "  ├────────────────────────────────────────────────────────────┤\n" RESET);
    
    if (info->vocabsCount > 0) {
        printf(CL_BORDER "  │ " RESET BOLD "Từ vựng liên quan (" CL_KEY "%d" RESET BOLD "):" RESET "                                │\n" RESET, info->vocabsCount);
        for (int j = 0; j < info->vocabsCount && j < 4; j++) {
            Vocab* v = &info->vocabs[j];
            printf(CL_BORDER "  │ " RESET "  " CL_LOGO "• " RESET BOLD "%-12s" RESET 
                   CL_DIM "(" RESET "%-14s" CL_DIM ")" RESET " : %-18s " CL_BORDER "│\n" RESET,
                   (v->vocab ? v->vocab : "?"), 
                   (v->hiragana ? v->hiragana : "?"), 
                   (v->meaning ? v->meaning : "?"));
        }
        if (info->vocabsCount > 4) {
            printf(CL_BORDER "  │ " RESET "  " CL_DIM "... và %d từ vựng khác" RESET "                              " CL_BORDER "│\n" RESET, info->vocabsCount - 4);
        }
    } else {
        printf(CL_BORDER "  │ " RESET "  " CL_DIM "(Không có từ vựng liên quan)" RESET "                        " CL_BORDER "│\n" RESET);
    }
    printf(CL_BORDER "  └────────────────────────────────────────────────────────────┘\n" RESET);
}

void analyzeJapaneseSentence(KanjiList *allData) {
    clearScreen();
    
    if (!allData || allData->kanjiCount == 0) {
        printf(CL_ERROR "  ⚠ Chưa có dữ liệu kanji!\n" RESET);
        waitForEnter();
        return;
    }

    KanjiHashTable* kanjiTable = createKanjiHashTable(allData);
    if (!kanjiTable) {
        printf(CL_ERROR "  ⚠ Lỗi tạo bảng băm kanji!\n" RESET);
        return;
    }

    char sentence[MAX_SENTENCE];

    printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
    printf(CL_BORDER "  │" BG_HIGHLIGHT "                    PHÂN TÍCH CÂU TIẾNG NHẬT                    " RESET CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  └────────────────────────────────────────────────────────────┘\n\n" RESET);
    
    printf(CL_TEXT "  📝 Nhập câu tiếng Nhật cần phân tích:\n" RESET);
    printf("  " CL_LOGO);
    
    inputString(sentence, MAX_SENTENCE);
    
    if (strlen(sentence) == 0) {
        printf(CL_WARN "\n  ⚠ Bạn chưa nhập câu nào!\n" RESET);
        freeKanjiHashTable(kanjiTable);
        waitForEnter();
        return;
    }

    wchar_t* wsentence = convertToWchar(sentence);
    if (!wsentence) {
        printf(CL_ERROR "  ⚠ Lỗi chuyển đổi UTF-8 sang wchar_t!\n" RESET);
        freeKanjiHashTable(kanjiTable);
        return;
    }

    printf("\n  " CL_PRIMARY "📖 KẾT QUẢ PHÂN TÍCH\n" RESET);
    printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
    printf(CL_BORDER "  │ " CL_HEADER "Câu gốc:" CL_RESET " %-53s " CL_BORDER "│\n" RESET, sentence);
    printf(CL_BORDER "  └────────────────────────────────────────────────────────────┘\n" RESET);

    int found = 0;
    for (int i = 0; wsentence[i] != L'\0'; i++) {
        wchar_t ch = wsentence[i];
        if (isKanjiWChar(ch)) {
            Kanji* info = findKanji(kanjiTable, ch);
            if (info) {
                found = 1;
                printOutKanjiInfo(info);
            }
        }
    }

    if (!found) {
        printf(CL_WARN "\n  ⚠ Trong câu này không tìm thấy Kanji nào trong dữ liệu.\n" RESET);
    }

    free(wsentence);
    freeKanjiHashTable(kanjiTable);
    waitForEnter();
}