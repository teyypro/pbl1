#ifndef FILTER_LEARNED_LESSON_H
#define FILTER_LEARNED_LESSON_H
#include "../data_structures.h"

typedef struct LearnedVocab {
    char *vocab;
    char *furigana;
    char *romaji;
    char *meaning;
    struct LearnedVocab *next; // hoặc chỉ cần LearnedVocab *next;
} LearnedVocab;


typedef struct LearnedVocabList {
    struct LearnedVocab *head; // Node đầu tiên
    int count;          // Tổng số từ vựng tìm thấy
} LearnedVocabList;

LearnedVocabList* createLearnedList();
void addLearnedVocab(LearnedVocabList *L, Vocab *v);
void runFilterLearnedVocab(KanjiList *allData);

#endif