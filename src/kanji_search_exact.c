#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
#endif
#include "kanji_search_exact.h"
#include "../data_structures.h"
#define CLR_PRIMARY "\x1b[38;5;75m"   // Xanh Blue
#define CLR_SUCCESS "\x1b[38;5;82m"   // Xanh lá
#define CLR_WARN    "\x1b[38;5;214m"  // Cam/Vàng
#define CLR_TEXT    "\x1b[38;5;253m"  // Trắng xám
#define CLR_RESET   "\x1b[0m"
#define BOLD        "\x1b[1m"
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
    
    // Chaining (Xử lý xung đột bằng danh sách liên kết)
    newNode->next = ht->table[index];
    ht->table[index] = newNode;
    ht->count++;
}

void buildHashTableForKanji(KanjiList *L, HashTableK *ht) {
    if (!L || !ht) return;
    for (int i = 0; i < L->kanjiCount; i++) {
        Kanji *k = &L->kanjis[i];
        
        // Hash theo mặt chữ Kanji
        if (k->kanji) insertKanjiToHT(ht, k->kanji, k);
        
        // Hash theo Hán Việt
        if (k->hanViet) insertKanjiToHT(ht, k->hanViet, k);
    }
}

void printOutKanji(Kanji *k) {
    if (k == NULL) return;

    // --- Header: Kanji chính ---
    printf(CLR_PRIMARY "╔══════════════════════════════════════════════════════════╗\n");
    printf("║ " CLR_RESET BOLD " KANJI: %-10s " CLR_PRIMARY "│" CLR_RESET BOLD " HÁN VIỆT: %-12s " CLR_PRIMARY "│" CLR_RESET " STT: %-4d " CLR_PRIMARY "║\n", 
            k->kanji ? k->kanji : "N/A", 
            k->hanViet ? k->hanViet : "N/A",
            k->stt);
    printf(CLR_PRIMARY "╠══════════════════════════════════════════════════════════╣\n" CLR_RESET);

    // --- Thông tin chi tiết ---
    printf(CLR_TEXT "  ➤ " BOLD "Bộ thủ: " CLR_RESET "%-15s" CLR_TEXT " ➤ " BOLD "Số nét: " CLR_RESET "%s\n", 
            k->radical ? k->radical : "N/A", 
            k->stroke ? k->stroke : "N/A");
    
    printf(CLR_TEXT "  ➤ " BOLD "Giải nghĩa: " CLR_RESET "%s\n", k->description ? k->description : "N/A");
    // --- Cách đọc (On-Kun) ---
    printf(CLR_PRIMARY "  ╟────────────────────────────────────────────────────────╢\n" CLR_RESET);
    printf(CLR_WARN "  [Âm On] : " CLR_RESET);
    for(int i = 0; i < k->onCount; i++) {
        printf(BOLD "%s" CLR_RESET " (%s)%s", k->on[i].jp, k->on[i].romaji, (i == k->onCount - 1) ? "" : ", ");
    }
    printf(CLR_WARN "\n  [Âm Kun]: " CLR_RESET);
    for(int i = 0; i < k->kunCount; i++) {
        printf(BOLD "%s" CLR_RESET " (%s)%s", k->kun[i].jp, k->kun[i].romaji, (i == k->kunCount - 1) ? "" : ", ");
    }
    printf("\n" CLR_PRIMARY "  ╟" BOLD "─── TỪ VỰNG LIÊN QUAN (%d từ) ───────────────────────────╢" CLR_RESET "\n", k->vocabsCount);
    
    if (k->vocabsCount > 0 && k->vocabs != NULL) {
        for (int i = 0; i < k->vocabsCount; i++) {
            Vocab *v = &k->vocabs[i];
            // Dòng từ vựng chính
            printf("    " CLR_SUCCESS "● " BOLD "%-15s" CLR_RESET " " CLR_TEXT "[%s - %s]" CLR_RESET "\n", 
                   v->vocab ? v->vocab : "N/A", 
                   v->hiragana ? v->hiragana : "N/A", 
                   v->romaji ? v->romaji : "N/A");
            
            // Ý nghĩa từ vựng
            printf("      " CLR_TEXT "└─ Ý nghĩa: " CLR_RESET "%s\n", v->meaning ? v->meaning : "N/A");
            
            // Ví dụ (Nếu có)
            if (v->samplesCount > 0) {
                for (int j = 0; j < v->samplesCount; j++) {
                    printf("         " CLR_WARN "↳ " CLR_TEXT "Ex: " CLR_RESET "%-25s " CLR_TEXT "→ %s" CLR_RESET "\n", 
                           v->samples[j].jp, v->samples[j].vn);
                }
            }
            if (i < k->vocabsCount - 1) printf("\n"); 
        }
    } else {
        printf(CLR_TEXT "    (Hiện không có từ vựng đi kèm)\n" CLR_RESET);
    }
    printf(CLR_PRIMARY "╚══════════════════════════════════════════════════════════╝\n" CLR_RESET);
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
        printf("Khong tim thay ket qua Kanji cho: %s\n", key);
    }
}