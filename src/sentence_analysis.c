// sentence_analysis.c
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

/* ====================== HASH TABLE CHO KANJI ====================== */

typedef struct KanjiNode {
    Kanji* kanjiData;
    struct KanjiNode* next;
} KanjiNode;

typedef struct {
    KanjiNode* buckets[512];        // HASH_SIZE = 512
} KanjiHashTable;


/* Hàm băm cho một ký tự Kanji */
unsigned int hashKanjiChar(wchar_t wc)
{
    return ((unsigned int)wc * 37) % 512;
}

/* Tạo HashTable từ toàn bộ dữ liệu Kanji */
KanjiHashTable* createKanjiHashTable(KanjiList *allData)
{
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

/* Tìm Kanji theo wchar_t */
Kanji* findKanji(KanjiHashTable* table, wchar_t wc)
{
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

void freeKanjiHashTable(KanjiHashTable* table)
{
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
        printf("Khong co thong tin kanji\n");
        return;
    }
    
    // Thông tin Kanji
    printf("▶ Kanji: %s\n", info->kanji);
    printf("  STT       : %d\n", info->stt);
    printf("  Hán Việt  : %s\n", info->hanViet ? info->hanViet : "?");
    printf("  Nghĩa     : %s\n", info->description ? info->description : "?");
    
    // Từ vựng liên quan
    if (info->vocabsCount > 0) {
        printf("  Từ vựng   :\n");
        int maxDisplay = info->vocabsCount;
        for (int j = 0; j < maxDisplay; j++) {
            Vocab* v = &info->vocabs[j];
            printf("    - %s (%s): %s\n", 
                   v->vocab ? v->vocab : "?", 
                   v->hiragana ? v->hiragana : "?", 
                   v->meaning ? v->meaning : "?");
        }
    }
    
    printf("----------------------------------------\n");
}

void analyzeJapaneseSentence(KanjiList *allData) {

    system("cls");
    if (!allData || allData->kanjiCount == 0) {
        printf("Chua co du lieu kanji!\n");
        return;
    }

    KanjiHashTable* kanjiTable = createKanjiHashTable(allData);
    if (!kanjiTable) {
        printf("Loi tao bang bam kanji!\n");
        return;
    }

    char sentence[MAX_SENTENCE];

    printf("\n=== Phân tích Câu JP here ===\n");
    printf(">>> Nhập câu bạn cần phân tích: \n");
    printf(">>> ");

    inputString(sentence, MAX_SENTENCE);
    if (strlen(sentence) == 0) return;

    /* Chuyển sang wchar_t */
    wchar_t* wsentence = convertToWchar(sentence);
    if (!wsentence) {
        printf("Loi chuyen doi UTF-8 sang wchar_t!\n");
        return;
    }


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
        printf("Trong cau nay khong tim thay kanji nao da hoc.\n");
    }

    free(wsentence);
    freeKanjiHashTable(kanjiTable);
}


