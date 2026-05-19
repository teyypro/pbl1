// ==================== src/fuzzy_search.c ====================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#ifdef _WIN32
    #include <windows.h>
#endif
#include "utils.h"
#include "../data_structures.h"

#define MAX_GAP 2

int min3(int a, int b, int c) {
    int m = a;
    if (b < m) m = b;
    if (c < m) m = c;
    return m;
}

int levenshteinDistance(wchar_t *s1, wchar_t *s2) {
    int len1 = wcslen(s1);
    int len2 = wcslen(s2);
    int i, j;
    
    int **arr = malloc((len1+1) * sizeof(int*));
    for (i = 0; i <= len1; i++) {
        arr[i] = malloc((len2 + 1) * sizeof(int));
    }
    for (i = 0; i <= len1; i++) arr[i][0] = i;
    for (j = 0; j <= len2; j++) arr[0][j] = j;

    int cost;
    for (i = 1; i <= len1; i++) {
        for (j = 1; j <= len2; j++) {
            cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            arr[i][j] = min3(
                arr[i-1][j] + 1,
                arr[i][j-1] + 1,
                arr[i-1][j-1] + cost
            );
        }
    }
    int res = arr[len1][len2];
    for (i = 0; i <= len1; i++) free(arr[i]);
    free(arr);
    return res;
}

void printFuzzyResult(int gap, Vocab *v) {
    char gapStr[10];
    if (gap == 0) {
        strcpy(gapStr, "EXACT");
    } else {
        sprintf(gapStr, "%d", gap);
    }
    
    char tagStr[40];
    if (gap == 0) {
        sprintf(tagStr, CL_SUCCESS "[%s]" RESET, gapStr); 
    } else {
        sprintf(tagStr, CL_DIM "[%s]" RESET, gapStr);
    }
    
    printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
    printf(CL_BORDER "  │ " CL_LOGO "📖 %-40s " CL_BORDER "│\n" RESET, v->vocab);
    printf(CL_BORDER "  │ " CL_TEXT "💬 %-44s " CL_BORDER "│\n" RESET, v->hiragana);
    printf(CL_BORDER "  │ " CL_KEY "🔍 %-44s " CL_BORDER "│\n" RESET, v->meaning);
    printf(CL_BORDER "  └────────────────────────────────────────────────────────────┘\n" RESET);
}

void fuzzySearching(KanjiList *L, char *inputUTF8, int option) {
    wchar_t *wInput = convertToWchar(inputUTF8);
    int foundCount = 0;
    int i, j;
    int gap;
    size_t inputLen = wcslen(wInput);

    printf("  " CL_PRIMARY "📊 KẾT QUẢ TÌM KIẾM MỜ\n" RESET);
    printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);

    for (i = 0; i < L->kanjiCount; i++) {
        for (j = 0; j < L->kanjis[i].vocabsCount; j++) {
            Vocab *v = &L->kanjis[i].vocabs[j];
            gap = 256;
            wchar_t *target_w = NULL;
            
            switch (option) {
                case 1: target_w = v->vocab_w; break;
                case 2: target_w = v->hiragana_w; break;
                case 3: target_w = v->romaji_w; break;
                case 4: target_w = v->meaning_w; break;
            }

            if (target_w != NULL) {
                size_t targetLen = wcslen(target_w);
                if (abs((int)inputLen - (int)targetLen) < 3) {
                    gap = levenshteinDistance(wInput, target_w);
                }
            }

            if (gap <= MAX_GAP) {
                printFuzzyResult(gap, v);
                foundCount++;
            }
        }
    }
    
    printf(CL_BORDER "  └────────────────────────────────────────────────────────────┘\n" RESET);
    
    if (foundCount == 0) {
        printf("\n  " CL_WARN "⚠ Không tìm thấy kết quả nào phù hợp.\n" RESET);
    } else {
        printf("\n  " CL_SUCCESS "✓ Tìm thấy %d kết quả\n" RESET, foundCount);
    }
    
    free(wInput);
}