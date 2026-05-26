#ifndef MULTIPLE_CHOICE_H
#define MULTIPLE_CHOICE_H
#include "../data_structures.h"


typedef struct {
    char *kanji;
    char *hanViet;
} KanjiMC;

typedef struct {
    char *vocab;
    char *furigana;
    char *romaji;   
    char *meaning;
} VocabMC;

typedef struct {
    int kanjiCount;
    KanjiMC *arr;
} KanjiMCList;

typedef struct {
    int vocabCount;
    VocabMC *arr;
} VocabMCList;


void runChoiceOption(KanjiList *L);

#endif