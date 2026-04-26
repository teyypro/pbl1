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

KanjiMCList* mapToKanjiMCRange(KanjiList *L, int start, int end);
VocabMCList* mapToVocabMCRange(KanjiList *L, int start, int end);
char* getKanjiDataByType(KanjiMC *item, int type);
void runMultipleChoiceTestKanji(KanjiMCList *list);
void runChoiceOption(KanjiList *L);
char* getVocabDataByType(VocabMC *item, int type);
void runMultipleChoiceTestVocab(VocabMCList *list);

#endif