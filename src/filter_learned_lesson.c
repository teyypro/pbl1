#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#ifdef _WIN32
    #include <windows.h>
#endif

#include "../data_structures.h"
#include "filter_learned_lesson.h"

#define KANJI_PER_LESSON 16

/* ====================== HÀM HỖ TRỢ wchar_t ====================== */

int isKanjiWChar(wchar_t wc)
{
    return (wc >= 0x4E00 && wc <= 0x9FFF) ||   /* Kanji thông dụng */
           (wc >= 0x3400 && wc <= 0x4DBF);
}

int isInWArray(const wchar_t* target, wchar_t** array, int count)
{
    for (int i = 0; i < count; i++) {
        if (wcscmp(target, array[i]) == 0)
            return 1;
    }
    return 0;
}


/* ====================== HÀM CHÍNH ====================== */
void runFilterLearnedVocab(KanjiList *allData)
{

    if (!allData || allData->kanjiCount == 0) {
        printf("Khong co du lieu.\n");
        return;
    }

    int selected[50] = {0};
    int n = 0, input, maxLesson = 0;

    printf("Nhap cac bai can review (ket thuc bang 0): ");
    while (scanf("%d", &input) == 1 && input != 0 && n < 50) {
        selected[n++] = input;
        if (input > maxLesson) maxLesson = input;
    }

    if (n == 0) {
        printf("Khong co bai nao duoc chon.\n");
        return;
    }


    int limit = maxLesson * KANJI_PER_LESSON + 10;

    wchar_t** learnedKanji = (wchar_t**)malloc(limit * sizeof(wchar_t*));
    wchar_t** targetKanji  = (wchar_t**)malloc(limit * sizeof(wchar_t*));

    if (!learnedKanji || !targetKanji) {
        printf("Loi cap phat bo nho!\n");
        free(learnedKanji);
        free(targetKanji);
        return;
    }

    int learnedCount = 0, targetCount = 0;

    /* Thu thập kanji đã học và kanji mục tiêu (dùng wchar_t) */
    for (int lesson = 1; lesson <= maxLesson; lesson++) {
        int start = (lesson - 1) * KANJI_PER_LESSON;
        int end = start + KANJI_PER_LESSON;
        if (end > allData->kanjiCount) end = allData->kanjiCount;

        for (int j = start; j < end; j++) {
            char* k8 = allData->kanjis[j].kanji;           // UTF-8
            if (!k8) continue;

            //wchar_t* kStr = utf8_to_wchar_temp(k8);
            wchar_t* kStr = allData->kanjis[j].kanji_w;
            if (!kStr) continue;

            learnedKanji[learnedCount++] = kStr;

            for (int k = 0; k < n; k++) {
                if (selected[k] == lesson) {
                    targetKanji[targetCount++] = kStr;
                    break;
                }
            }
        }
    }


    int foundVocabs = 0;
    int maxVocab = maxLesson * KANJI_PER_LESSON;
    /* Duyệt tất cả từ vựng */
    for (int i = 0; i < maxVocab; i++) {
        Kanji *kj = &allData->kanjis[i];
        
        for (int j = 0; j < kj->vocabsCount; j++) {
            Vocab *v = &kj->vocabs[j];
            if (!v->vocab || !v->vocab_w) continue;  // Kiểm tra cả hai
            
            const wchar_t* ptr = v->vocab_w;
            int hasTarget = 0;
            int allKnown = 1;
            
            for (int k = 0; ptr[k] != L'\0'; k++) {  // Dùng for thay vì while
                wchar_t wc = ptr[k];
                
                if (isKanjiWChar(wc)) {
                    wchar_t single[2] = {wc, L'\0'};
                    
                    if (isInWArray(single, targetKanji, targetCount)) {
                        hasTarget = 1;
                    }
                    
                    if (!isInWArray(single, learnedKanji, learnedCount)) {
                        allKnown = 0;
                        break;
                    }
                }
            }
            
            if (hasTarget && allKnown) {
                foundVocabs++;
                printf("%d. %s (%s - %s): %s\n",
                    foundVocabs,
                    v->vocab,
                    v->hiragana,
                    v->romaji,
                    v->meaning);
                
            }
        }
    }

    if (foundVocabs == 0) {
        printf("Khong tim thay tu vung nao thoa man dieu kien.\n");
    } else {
        printf("\nTong so tu vung tim duoc: %d\n", foundVocabs);
    }

    /* Giải phóng tất cả wchar_t tạm */
    for (int i = 0; i < learnedCount; i++) free(learnedKanji[i]);
    free(learnedKanji);
    free(targetKanji);

    printf("\nNhan Enter de tiep tuc...");
    getchar();
}