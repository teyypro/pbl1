// ==================== src/substring_search.c ====================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#ifdef _WIN32
    #include <windows.h>
#endif
#include "utils.h"
#include "../data_structures.h"

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

int KMPsearch(wchar_t *pat, wchar_t *txt) {
    int M = wcslen(pat);
    int N = wcslen(txt);

    int *lps = malloc(M * sizeof(int));
    computeLPSArray(pat, M, lps);

    int i = 0, j = 0;
    while (i < N) {
        if (pat[j] == txt[i]) {
            i++;
            j++;
        }

        if (j == M) {
            free(lps);
            return 1;
        } else if (i < N && pat[j] != txt[i]) {
            if (j != 0)
                j = lps[j - 1];
            else
                i++;
        }
    }
    free(lps);
    return 0;
}

void printSubstringResult(Vocab *v) {
    printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────────────────────┐\n" RESET);
    printf(CL_BORDER "  │ " CL_LOGO "%-20s" CL_BORDER " │ " CL_TEXT "%-20s" CL_BORDER " │ " CL_TEXT "%-20s" CL_BORDER " │ " CL_KEY "%-30s" CL_BORDER " │\n" RESET,
           v->vocab, v->hiragana, v->romaji, v->meaning);
    printf(CL_BORDER "  └────────────────────────────────────────────────────────────────────────────┘\n" RESET);
}

void substringSearching(KanjiList *L, char *inputUTF8, int option) {
    wchar_t *wInput = convertToWchar(inputUTF8);
    int foundCount = 0;
    size_t inputLen = wcslen(wInput);
    
    printf("  " CL_PRIMARY "📊 KẾT QUẢ TÌM KIẾM MẪU (KMP)\n" RESET);
    printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────────────────────┐\n" RESET);
    printf(CL_BORDER "  │ " CL_HEADER "TỪ VỰNG              " CL_BORDER "│ " CL_HEADER "HIRAGANA             " CL_BORDER "│ " CL_HEADER "ROMAJI               " CL_BORDER "│ " CL_HEADER "NGHĨA" CL_BORDER "                       │\n" RESET);
    printf(CL_BORDER "  ├────────────────────────────────────────────────────────────────────────────┤\n" RESET);

    for (int i = 0; i < L->kanjiCount; i++) {
        for (int j = 0; j < L->kanjis[i].vocabsCount; j++) {
            Vocab *v = &L->kanjis[i].vocabs[j];
            wchar_t *target_w = NULL;
            
            switch (option) {
                case 1: target_w = v->vocab_w; break;
                case 2: target_w = v->hiragana_w; break;
                case 3: target_w = v->romaji_w; break;
                case 4: target_w = v->meaning_w; break;
            }

            if (target_w && KMPsearch(wInput, target_w)) {
                printSubstringResult(v);
                foundCount++;
            }
        }
    }
    
    printf(CL_BORDER "  └────────────────────────────────────────────────────────────────────────────┘\n" RESET);
    
    if (foundCount == 0) {
        printf("\n  " CL_WARN "⚠ Không tìm thấy kết quả nào phù hợp với mẫu: " CL_LOGO "%s\n" RESET, inputUTF8);
    } else {
        printf("\n  " CL_SUCCESS "✓ Tìm thấy %d kết quả\n" RESET, foundCount);
    }
    
    free(wInput);
}