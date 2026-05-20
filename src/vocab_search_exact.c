// ==================== src/vocab_search_exact.c ====================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
#endif
#include "vocab_search_exact.h"
#include "../data_structures.h"
#include "utils.h"

HashTable* createHashTable(int size) {
    HashTable *ht = malloc(sizeof(HashTable));
    if (ht == NULL) return NULL;
    ht->size = size;
    ht->count = 0;
    ht->table = malloc(sizeof(HashNode*) * size);
    int i;
    for (i = 0; i < ht->size; i++) {
        ht->table[i] = NULL;
    }
    return ht;
}

HashNode* createHashNode(char *key, Vocab* vocab) {
    HashNode *newNode = malloc(sizeof(HashNode));
    newNode->key = strdup(key);
    newNode->vocab = vocab;
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

void insertVocabToHT(HashTable *ht, char *key, Vocab *vocab) {
    if (!ht || !key || !vocab) return;
    
    int indexHT = hashGetIndex(key, ht->size);
    HashNode *newNode = createHashNode(key, vocab);
    newNode->next = ht->table[indexHT];
    ht->table[indexHT] = newNode;
    ht->count++;
}

void buildHashTableForVocab(KanjiList *L, HashTable *ht) {
    int i, j;
    for (i = 0; i < L->kanjiCount; i++) {
        Kanji *k = &L->kanjis[i];
        for (j = 0; j < k->vocabsCount; j++) {
            Vocab *v = &k->vocabs[j];
            
            insertVocabToHT(ht, v->vocab, v);
            insertVocabToHT(ht, v->romaji, v);
            insertVocabToHT(ht, v->hiragana, v);
            insertVocabToHT(ht, v->meaning, v);
        }
    }
}


void printOutVocab(Vocab *v) {
    if (v == NULL) return;
    
    // Header vocab hiện đại, sử dụng cấu trúc phẳng kết hợp các trường dữ liệu màu sắc đồng bộ
    printf(
        "  " CL_PRIMARY "┃ " RESET
        CL_KANJI BOLD "%s" RESET
        "  " CL_DIM "(" RESET
        CL_KANA "%s" RESET
        CL_DIM " • " RESET
        CL_ROMAJI "%s" RESET
        CL_DIM ")" RESET
        "  " CL_MEANING "%s\n",
        v->vocab ? v->vocab : "-",
        v->hiragana ? v->hiragana : "-",
        v->romaji ? v->romaji : "-",
        v->meaning ? v->meaning : "-"
    );
    
    // Khối câu ví dụ (Examples) sử dụng đường dẫn hướng mờ góc màn hình
    if (v->samplesCount > 0 && v->samples != NULL) {
        for (int i = 0; i < v->samplesCount; i++) {
            printf(
                "  " CL_PRIMARY "┃    " RESET
                CL_DIM "┆ " RESET
                CL_JP_SENT "%s\n" RESET,
                v->samples[i].jp ? v->samples[i].jp : ""
            );

            printf(
                "  " CL_PRIMARY "┃    " RESET
                CL_DIM "└─ " RESET
                CL_VN_SENT "%s\n" RESET,
                v->samples[i].vn ? v->samples[i].vn : ""
            );
        }
    }
    
    // Khoảng trắng dòng mờ phân cách giữa các mục từ vựng kết quả
    printf("  " CL_PRIMARY "┃\n" RESET);
}

void exactlySearching(HashTable *ht, char *key) {
    if (!ht || !key) return;
    
    int index = hashGetIndex(key, ht->size);
    HashNode *current = ht->table[index];
    
    int found = 0;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            printOutVocab(current->vocab);
            found = 1;
        }
        current = current->next;
    }
    
    if (!found) {
        printf("  " CL_WARN "⚠ Không tìm thấy kết quả cho từ khóa: " CL_LOGO "%s\n" RESET, key);
    }
    

}