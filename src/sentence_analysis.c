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
        printf("\n  " CL_WARN "⚠ Không có thông tin Kanji.\n" RESET);
        return;
    }
    
    // Giao diện tiêu đề phẳng với các thông số phân bổ cột đối xứng
    printf("  " CL_PRIMARY "┃ " CL_DIM "Kanji      :" RESET " " CL_KANJI BOLD "%s" RESET "  " CL_DIM "—" RESET "  " CL_MEANING BOLD "%s" RESET "  " CL_DIM "(STT: %d)" RESET "\n", 
           info->kanji ? info->kanji : "N/A", 
           info->hanViet ? info->hanViet : "N/A", 
           info->stt);
    printf("  " CL_PRIMARY "┃ " CL_DIM "Bộ thủ     :" RESET " %-15s " CL_DIM "Số nét: %s\n" RESET, 
           info->radical ? info->radical : "N/A", 
           info->stroke ? info->stroke : "N/A");
    printf("  " CL_PRIMARY "┃ " CL_DIM "Giải nghĩa :" RESET " " CL_TEXT "%s\n", 
           info->description ? info->description : "Chưa có mô tả.");
    
    printf("  " CL_PRIMARY "┃\n");

    // Khối phân rã cấu trúc âm đọc ON/KUN trực quan bằng token màu sắc đồng bộ
    printf("  " CL_PRIMARY "┃ " CL_DIM "[Âm ON]  :" RESET " ");
    if (info->onCount > 0) {
        for (int i = 0; i < info->onCount; i++) {
            printf(CL_KANJI BOLD "%s" RESET " " CL_KANA "(%s)" RESET "%s", info->on[i].jp, info->on[i].romaji, (i == info->onCount - 1) ? "" : ", ");
        }
    } else printf("N/A");
    
    printf("\n  " CL_PRIMARY "┃ " CL_DIM "[Âm KUN] :" RESET " ");
    if (info->kunCount > 0) {
        for (int i = 0; i < info->kunCount; i++) {
            printf(CL_KANJI BOLD "%s" RESET " " CL_KANA "(%s)" RESET "%s", info->kun[i].jp, info->kun[i].romaji, (i == info->kunCount - 1) ? "" : ", ");
        }
    } else printf("N/A");
    
    printf("\n  " CL_PRIMARY "┃\n");
    printf("  " CL_PRIMARY "┃ " CL_DIM "Từ vựng liên quan (%d):\n" RESET, info->vocabsCount);
    
    // Đồng bộ cách sắp xếp cấu trúc phẳng cho các từ vựng và câu ví dụ liên đới
    if (info->vocabsCount > 0 && info->vocabs != NULL) {
        for (int j = 0; j < info->vocabsCount; j++) {
            Vocab* v = &info->vocabs[j];
            
            printf(
                "  " CL_PRIMARY "┃ " RESET
                CL_DIM "%02d." RESET " "
                CL_KANJI BOLD "%s" RESET
                "  " CL_DIM "(" RESET
                CL_KANA "%s" RESET
                CL_DIM " • " RESET
                CL_ROMAJI "%s" RESET
                CL_DIM ")" RESET
                "  " CL_MEANING "%s\n",
                j + 1,
                v->vocab ? v->vocab : "?", 
                v->hiragana ? v->hiragana : "?", 
                v->romaji ? v->romaji : "?",
                v->meaning ? v->meaning : "?"
            );

        }
    } else {
        printf("  " CL_PRIMARY "┃ " RESET CL_DIM "    (Hệ thống chưa nạp từ vựng liên đới)\n" RESET);
        printf("  " CL_PRIMARY "┃\n" RESET);
    }
    printf("\n");
}

void analyzeJapaneseSentence(KanjiList *allData) {
    clearScreen();
    
    if (!allData || allData->kanjiCount == 0) {
        printf("\n  " CL_ERROR "🚨 [HỆ THỐNG] Phân vùng bộ nhớ trống. Chưa nạp cơ sở dữ liệu Kanji.\n" RESET);
        waitForEnter();
        return;
    }

    KanjiHashTable* kanjiTable = createKanjiHashTable(allData);
    if (!kanjiTable) {
        printf("\n  " CL_ERROR "🚨 [HỆ THỐNG] Lỗi nghiêm trọng: Phân bổ thực thể bảng băm (Heap Allocation) thất bại.\n" RESET);
        return;
    }

    char sentence[MAX_SENTENCE];

    // Tiêu đề bảng đồng bộ với cấu trúc thiết kế của dashboard bài học tổng hợp
    printf("\n  " BG_HIGHLIGHT BOLD "  TRA CỨU KANJI TRONG CÂU " RESET "\n");
    printf(CL_BORDER "──────────────────────────────────────────────────────────────────\n" RESET);
    
    printf("  " CL_TEXT "Hãy nhập câu cần phân tích các Kanji trong đó:\n" RESET);
    printf(CL_LOGO BOLD "  " RESET CL_LOGO BOLD);
    
    inputString(sentence, MAX_SENTENCE);
    printf(RESET);
    
    if (strlen(sentence) == 0) {
        printf("\n  " CL_WARN "⚠ [CẢNH BÁO] Chuỗi rỗng. Tiến trình phân rã bị hủy bỏ bởi người dùng.\n" RESET);
        freeKanjiHashTable(kanjiTable);
        waitForEnter();
        return;
    }

    wchar_t* wsentence = convertToWchar(sentence);
    if (!wsentence) {
        printf("\n  " CL_ERROR "🚨 [LỖI MÃ HÓA] Không thể chuyển đổi UTF-8 sang bộ ký tự rộng wchar_t.\n" RESET);
        freeKanjiHashTable(kanjiTable);
        return;
    }
    
    // Giao diện hiển thị kết quả phân rã chuỗi đầu vào
    system("cls");
    printf("  " CL_PRIMARY "▶ " RESET BOLD "%s\n" RESET, sentence);
    printf(CL_BORDER "──────────────────────────────────────────────────────────────────\n" RESET);

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
        printf("  " CL_WARN "⚠ Không tìm thấy phần tử Kanji nào thuộc phạm vi dữ liệu đã học trong câu trên.\n" RESET);
    }

    printf(CL_BORDER "──────────────────────────────────────────────────────────────────\n" RESET);

    free(wsentence);
    freeKanjiHashTable(kanjiTable);
    waitForEnter();
}