#ifndef SEARCH_FUZZY_H
#define SEARCH_FUZZY_H
#include "../data_structures.h"
#include <wchar.h>

int min3(int a, int b, int c);
int levenshteinDistance(const wchar_t *s1, const wchar_t *s2);
void printFuzzyResult(int gap, Vocab *v);
void fuzzySearching(KanjiList *L, const char *inputUTF8, int option);

#endif