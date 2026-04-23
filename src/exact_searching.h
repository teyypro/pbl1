#ifndef EXACT_SEARCHING_H
#define EXACT_SEARCHING_H
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
HashNode* createHashNode(char *key, Vocab* vocab);
int hashGetIndex(char *str, int tableSize);
void insertVocabToHT(HashTable *ht, char *key, Vocab *vocab);
void buildHashTableForVocab(KanjiList *L, HashTable *ht);
void printOutVocab(Vocab *v);
void exactlySearching(HashTable *ht, char *key);

#endif