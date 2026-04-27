#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#ifdef _WIN32
    #include <windows.h>
#endif
#include "utils.h"
#include "../data_structures.h"

// Tạo mảng LPS cho mẫu
void computeLPSArray(wchar_t *pat, int M, int *lps) {
    int len = 0;
    lps[0] = 0;
    int i = 1;

    while (i < M) {
        if (pat[i] == pat[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

// Hàm KMP: trả về 1 nếu tìm thấy, 0 nếu không
int KMPsearch(wchar_t *pat, wchar_t *txt) {
    int M = wcslen(pat);
    int N = wcslen(txt);

    int *lps = malloc(M * sizeof(int));
    computeLPSArray(pat, M, lps);

    int i = 0; // chỉ số cho txt
    int j = 0; // chỉ số cho pat
    while (i < N) {
        if (pat[j] == txt[i]) {
            i++;
            j++;
        }

        if (j == M) {
            free(lps);
            return 1; // tìm thấy
        } else if (i < N && pat[j] != txt[i]) {
            if (j != 0)
                j = lps[j - 1];
            else
                i++;
        }
    }
    free(lps);
    return 0; // không tìm thấy
}


void printSubstringResult(Vocab *v) {

    printf("%-20s %-20s %-20s %-30s\n",  
        v->vocab, 
        v->hiragana, 
        v->romaji, 
        v->meaning);

}

void substringSearching(KanjiList *L, char *inputUTF8, int option) {
    wchar_t *wInput = convertToWchar(inputUTF8);
    int foundCount = 0;
    int i, j;
    int gap;
    for (i = 0; i < L->kanjiCount; i++) {
        for (j = 0; j < L->kanjis[i].vocabsCount; j++) {
            Vocab *v = &L->kanjis[i].vocabs[j];
            gap = 256;
            wchar_t *target_w = NULL;
            char *target_label = NULL;
            switch (option) {
                case 1: {
                    target_w = v->vocab_w;
                    target_label = v->vocab;
                    break;
                }
                case 2: {
                    target_w = v->hiragana_w;
                    target_label = v->hiragana;
                    break;
                }
                case 3: {
                    target_w = v->romaji_w;
                    target_label = v->romaji;
                    break;
                }
                case 4: {
                    target_w = v->meaning_w;
                    target_label = v->meaning;
                    break;
                }
            }

            if (KMPsearch(wInput, target_w)) {
                printSubstringResult(v); 
                foundCount++;
            }

        }
    }
    if (foundCount == 0) {
        printf("Not Found!\n");
    }

}