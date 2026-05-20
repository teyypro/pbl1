// ==================== src/kanji_search_exact.c ====================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
#endif
#include "kanji_search_exact.h"
#include "../data_structures.h"
#include "utils.h"

HashTableK* createHashTableK(int size) {
    HashTableK *ht = malloc(sizeof(HashTableK));
    if (ht == NULL) return NULL;
    ht->size = size;
    ht->count = 0;
    ht->table = malloc(sizeof(HashNodeK*) * size);
    for (int i = 0; i < ht->size; i++) {
        ht->table[i] = NULL;
    }
    return ht;
}

HashNodeK* createHashNodeK(char *key, Kanji* kanji) {
    HashNodeK *newNode = malloc(sizeof(HashNodeK));
    if (newNode == NULL) return NULL;
    
    newNode->key = strdup(key);
    newNode->kanji = kanji;    
    newNode->next = NULL;
    return newNode;
}

int hashGetIndex(char *str, int tableSize) {
    unsigned long hash = 5381; 
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; 
    }
    return (int)(hash % tableSize);
}

void insertKanjiToHT(HashTableK *ht, char *key, Kanji *kanji) {
    if (!ht || !key || !kanji) return;
    
    int index = hashGetIndex(key, ht->size);
    HashNodeK *newNode = createHashNodeK(key, kanji);
    
    newNode->next = ht->table[index];
    ht->table[index] = newNode;
    ht->count++;
}

void buildHashTableForKanji(KanjiList *L, HashTableK *ht) {
    if (!L || !ht) return;
    for (int i = 0; i < L->kanjiCount; i++) {
        Kanji *k = &L->kanjis[i];
        
        if (k->kanji) insertKanjiToHT(ht, k->kanji, k);
        if (k->hanViet) insertKanjiToHT(ht, k->hanViet, k);
    }
}


void printOutKanji(Kanji *k) {
    if (k == NULL) return;

    // Giao diện tiêu đề phẳng với thanh định vị trực giác màu hệ thống
    printf("  " CL_PRIMARY "┃ " CL_DIM "Kanji      :" RESET " " CL_KANJI BOLD "%s" RESET "\n", k->kanji ? k->kanji : "N/A");
    printf("  " CL_PRIMARY "┃ " CL_DIM "Hán Việt   :" RESET " " CL_MEANING BOLD "%-15s" RESET " " CL_DIM "STT: %d\n" RESET, k->hanViet ? k->hanViet : "N/A", k->stt);
    printf("  " CL_PRIMARY "┃ " CL_DIM "Bộ thủ     :" RESET " %-15s " CL_DIM "Số nét: %s\n" RESET, k->radical ? k->radical : "N/A", k->stroke ? k->stroke : "N/A");
    printf("  " CL_PRIMARY "┃ " CL_DIM "Giải nghĩa :" RESET " %s\n", k->description ? k->description : "N/A");
    
    printf("  " CL_PRIMARY "┃\n");

    // Khối phân rã cấu trúc âm đọc ON/KUN trực quan bằng token màu, tránh rối mắt
    printf("  " CL_PRIMARY "┃ " CL_DIM "[Âm ON]  :" RESET " ");
    if (k->onCount > 0) {
        for (int i = 0; i < k->onCount; i++) {
            printf(CL_KANJI BOLD "%s" RESET " " CL_KANA "(%s)" RESET "%s", k->on[i].jp, k->on[i].romaji, (i == k->onCount - 1) ? "" : ", ");
        }
    } else printf("N/A");
    
    printf("\n  " CL_PRIMARY "┃ " CL_DIM "[Âm KUN] :" RESET " ");
    if (k->kunCount > 0) {
        for (int i = 0; i < k->kunCount; i++) {
            printf(CL_KANJI BOLD "%s" RESET " " CL_KANA "(%s)" RESET "%s", k->kun[i].jp, k->kun[i].romaji, (i == k->kunCount - 1) ? "" : ", ");
        }
    } else printf("N/A");
    
    printf("\n  " CL_PRIMARY "┃\n");
    printf("  " CL_PRIMARY "┃ " CL_DIM "Từ vựng liên quan (%d):\n" RESET, k->vocabsCount);
    
    // Đồng bộ cách sắp đặt cột đối xứng phẳng cho mục từ vựng liên quan và ví dụ mẫu
    if (k->vocabsCount > 0 && k->vocabs != NULL) {
        for (int i = 0; i < k->vocabsCount; i++) {
            Vocab *v = &k->vocabs[i];
            
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
                i + 1,
                v->vocab,
                v->hiragana ? v->hiragana : "-",
                v->romaji ? v->romaji : "-",
                v->meaning ? v->meaning : "-"
            );

            // In khối ví dụ tương đương đường trục phụ
            if (v->samplesCount > 0) {
                for (int j = 0; j < v->samplesCount; j++) {
                    printf(
                        "  " CL_PRIMARY "┃    " RESET
                        CL_DIM "┆ " RESET
                        CL_JP_SENT "%s\n" RESET,
                        v->samples[j].jp
                    );
                    printf(
                        "  " CL_PRIMARY "┃    " RESET
                        CL_DIM "└─ " RESET
                        CL_VN_SENT "%s\n" RESET,
                        v->samples[j].vn
                    );
                }
            }
            printf("\n");
        }
    } else {
        printf("  " CL_PRIMARY "┃ " RESET CL_DIM "(Hiện không có từ vựng đi kèm)\n" RESET);
    }
}

void exactlySearchingKanji(HashTableK *ht, char *key) {
    if (!ht || !key) return;
    
    int index = hashGetIndex(key, ht->size);
    HashNodeK *current = ht->table[index];
    
    
    int found = 0;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            printOutKanji(current->kanji);
            found = 1;
            
        }
        current = current->next;
    }
    
    if (!found) {
        printf("  " CL_WARN "⚠ Không tìm thấy kết quả Kanji cho: " CL_LOGO "%s\n" RESET, key);
    }
    
}