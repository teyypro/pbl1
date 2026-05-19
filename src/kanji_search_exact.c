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

    printf(CL_PRIMARY "╔════════════════════════════════════════════════════════════════════╗\n");
    printf("║ " CL_RESET BOLD " KANJI: %-10s " CL_PRIMARY "│" CL_RESET BOLD " HÁN VIỆT: %-12s " CL_PRIMARY "│" CL_RESET " STT: %-4d " CL_PRIMARY "║\n", 
            k->kanji ? k->kanji : "N/A", 
            k->hanViet ? k->hanViet : "N/A",
            k->stt);
    printf(CL_PRIMARY "╠════════════════════════════════════════════════════════════════════╣\n" CL_RESET);

    printf(CL_TEXT "  ➤ " BOLD "Bộ thủ: " CL_RESET "%-15s" CL_TEXT " ➤ " BOLD "Số nét: " CL_RESET "%s\n", 
            k->radical ? k->radical : "N/A", 
            k->stroke ? k->stroke : "N/A");
    
    printf(CL_TEXT "  ➤ " BOLD "Giải nghĩa: " CL_RESET "%s\n", k->description ? k->description : "N/A");
    
    printf(CL_PRIMARY "  ╟────────────────────────────────────────────────────────────────────╢\n" CL_RESET);
    
    printf(CL_WARN "  [Âm On] : " CL_RESET);
    for(int i = 0; i < k->onCount; i++) {
        printf(BOLD "%s" CL_RESET " (%s)%s", k->on[i].jp, k->on[i].romaji, (i == k->onCount - 1) ? "" : ", ");
    }
    printf(CL_WARN "\n  [Âm Kun]: " CL_RESET);
    for(int i = 0; i < k->kunCount; i++) {
        printf(BOLD "%s" CL_RESET " (%s)%s", k->kun[i].jp, k->kun[i].romaji, (i == k->kunCount - 1) ? "" : ", ");
    }
    
    printf("\n" CL_PRIMARY "  ╟" BOLD "─── TỪ VỰNG LIÊN QUAN (%d từ) ─────────────────────────────────────╢" CL_RESET "\n", k->vocabsCount);
    
    if (k->vocabsCount > 0 && k->vocabs != NULL) {
        for (int i = 0; i < k->vocabsCount; i++) {
            Vocab *v = &k->vocabs[i];
            printf("    " CL_SUCCESS "● " BOLD "%-15s" CL_RESET " " CL_TEXT "[%s - %s]" CL_RESET "\n", 
                   v->vocab ? v->vocab : "N/A", 
                   v->hiragana ? v->hiragana : "N/A", 
                   v->romaji ? v->romaji : "N/A");
            printf("      " CL_TEXT "└─ Ý nghĩa: " CL_RESET "%s\n", v->meaning ? v->meaning : "N/A");
            
            if (v->samplesCount > 0) {
                for (int j = 0; j < v->samplesCount; j++) {
                    printf("         " CL_WARN "↳ " CL_TEXT "Ex: " CL_RESET "%-25s " CL_TEXT "→ %s" CL_RESET "\n", 
                           v->samples[j].jp, v->samples[j].vn);
                }
            }
            if (i < k->vocabsCount - 1) printf("\n");
        }
    } else {
        printf(CL_TEXT "    (Hiện không có từ vựng đi kèm)\n" CL_RESET);
    }
    printf(CL_PRIMARY "╚════════════════════════════════════════════════════════════════════╝\n" CL_RESET);
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
        printf(CL_WARN "  ⚠ Không tìm thấy kết quả Kanji cho: " CL_LOGO "%s\n" RESET, key);
    }
}