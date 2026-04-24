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

// Prototypes cho Kanji
HashTableK* createHashTableK(int size);
void insertKanjiToHT(HashTableK *ht, char *key, Kanji *kanji);
void buildHashTableForKanji(KanjiList *L, HashTableK *ht);
static int hashGetIndex(char *str, int tableSize);
void insertKanjiToHT(HashTableK *ht, char *key, Kanji *kanji);
void printOutKanji(Kanji *k);
void exactlySearchingKanji(HashTableK *ht, char *key);

#endif