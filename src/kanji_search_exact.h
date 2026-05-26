#ifndef kanji_search_exact_H
#define kanji_search_exact_H
#include "../data_structures.h"

#define HASH_TABLE_SIZE 10007

struct HashNodeForKanji {
    char *key;
    Kanji *kanji;
    struct HashNodeForKanji *next;
};

struct HashTableForKanji {
    struct HashNodeForKanji **table;
    int size;
    int count;
};

typedef struct HashNodeForKanji HashNodeK;
typedef struct HashTableForKanji HashTableK;

HashTableK* createHashTableK(int size);
void buildHashTableForKanji(KanjiList *L, HashTableK *ht);
void exactlySearchingKanji(HashTableK *ht, char *key);

#endif