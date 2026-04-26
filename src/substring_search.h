#ifndef SUBSTRING_SEARCH_H
#define SUBSTRING_SEARCH_H
#include "../data_structures.h"
#include <wchar.h>

void computeLPSArray(wchar_t *pat, int M, int *lps);
int KMPsearch(wchar_t *pat, wchar_t *txt);
void printSubstringResult(Vocab *v);
void substringSearching(KanjiList *L, char *inputUTF8, int option);

#endif