#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
#endif
#include "exact_searching_kanji.h"
#include "../data_structures.h"

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
    printf("============================================================\n");
    printf("KANJI: %s  |  HAN VIET: %s || STT: %d\n", 
            k->kanji ? k->kanji : "N/A", 
            k->hanViet ? k->hanViet : "N/A",
            k->stt);
    printf("Bo thu: %s  |  So net: %s\n", 
            k->radical ? k->radical : "N/A", 
            k->stroke ? k->stroke : "N/A");
    printf("Giai nghia: %s\n", k->description ? k->description : "N/A");
    
    // In On-yomi
    printf("Am On: ");
    for(int i = 0; i < k->onCount; i++) {
        printf("%s (%s)%s", k->on[i].jp, k->on[i].romaji, (i == k->onCount - 1) ? "" : ", ");
    }
    
    // In Kun-yomi
    printf("\nAm Kun: ");
    for(int i = 0; i < k->kunCount; i++) {
        printf("%s (%s)%s", k->kun[i].jp, k->kun[i].romaji, (i == k->kunCount - 1) ? "" : ", ");
    }

    // In danh sach tu vung lien quan
    printf("\n\n--- Tu vung lien quan (%d tu) ---\n", k->vocabsCount);
    if (k->vocabsCount > 0 && k->vocabs != NULL) {
        for (int i = 0; i < k->vocabsCount; i++) {
            Vocab *v = &k->vocabs[i];
            printf("  %d. %s [%s - %s]\n", 
                   i + 1, 
                   v->vocab ? v->vocab : "N/A", 
                   v->hiragana ? v->hiragana : "N/A", 
                   v->romaji ? v->romaji : "N/A");
            printf("     Nghia: %s\n", v->meaning ? v->meaning : "N/A");
            
            // In them vi du neu co (Optionally)
            if (v->samplesCount > 0) {
                for (int j = 0; j < v->samplesCount; j++) {
                    printf("      +  %s (%s)\n", v->samples[j].jp, v->samples[j].vn);
                }
            }
        }
    } else {
        printf("  (Khong co tu vung di kem)\n");
    }
    printf("============================================================\n");
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