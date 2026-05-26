#ifndef vocab_search_exact_H
#define vocab_search_exact_H
#include "../data_structures.h"

#define HASH_TABLE_SIZE 10007

struct HashNodeForVocab {
    char *key;
    Vocab *vocab;
    struct HashNodeForVocab *next;
};

struct HashTableForVocab {
    struct HashNodeForVocab **table;
    int size;
    int count;
};

typedef struct HashNodeForVocab HashNode;
typedef struct HashTableForVocab HashTable;

HashTable* createHashTable(int size);
void buildHashTableForVocab(KanjiList *L, HashTable *ht);
void exactlySearchingVocab(HashTable *ht, char *key);

#endif