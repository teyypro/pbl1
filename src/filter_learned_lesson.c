#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#ifdef _WIN32
    #include <windows.h>
#endif

#include "../data_structures.h"
#include "filter_learned_lesson.h"
#include "utils.h"

#define KANJI_PER_LESSON 16
#define HASH_SIZE        256


typedef struct HashNode {
    wchar_t* kanji;
    struct HashNode* next;
} HashNode;

typedef struct {
    HashNode* buckets[HASH_SIZE];
} LearnedHashSet;

unsigned int hashKanji(const wchar_t* str) {
    unsigned int hash = 5381;
    while (*str) {
        hash = ((hash << 5) + hash) + *str++;
    }
    return hash % HASH_SIZE;
}

LearnedHashSet* createLearnedHashSet() {
    return (LearnedHashSet*)calloc(1, sizeof(LearnedHashSet));
}

void addLearnedKanji(LearnedHashSet* set, wchar_t* kanji) {
    if (!set || !kanji) return;

    unsigned int idx = hashKanji(kanji);
    HashNode* node = (HashNode*)malloc(sizeof(HashNode));
    
    node->kanji = kanji;
    node->next = set->buckets[idx];
    set->buckets[idx] = node;
}

int containsLearnedKanji(LearnedHashSet* set, const wchar_t* kanji) {
    if (!set || !kanji) return 0;

    unsigned int idx = hashKanji(kanji);
    HashNode* curr = set->buckets[idx];

    while (curr) {
        if (wcscmp(curr->kanji, kanji) == 0)
            return 1;
        curr = curr->next;
    }
    return 0;
}

void freeLearnedHashSet(LearnedHashSet* set)
{
    if (!set) return;
    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode* curr = set->buckets[i];
        while (curr) {
            HashNode* temp = curr;
            curr = curr->next;
            free(temp);
        }
    }
    free(set);
}

void runFilterLearnedVocab(KanjiList *allData) {

    if (!allData || allData->kanjiCount == 0) {
        printf("Khong co du lieu.\n");
        return;
    }

    /* Nhập các bài muốn review */
    int selected[50] = {0};
    int numSelected = 0, input, maxLesson = 0;

    printf("Nhap cac bai can review (ket thuc bang 0): ");
    while (scanf("%d", &input) == 1 && input != 0 && numSelected < 50) {
        selected[numSelected++] = input;
        if (input > maxLesson) maxLesson = input;
    }

    if (numSelected == 0) {
        printf("Khong co bai nao duoc chon.\n");
        return;
    }

    /* Khởi tạo cấu trúc dữ liệu */
    wchar_t** targetKanjiList = malloc((maxLesson * KANJI_PER_LESSON + 10) * sizeof(wchar_t*));
    LearnedHashSet* learnedHash = createLearnedHashSet();

    if (!targetKanjiList || !learnedHash) {
        printf("Loi cap phat bo nho!\n");
        free(targetKanjiList);
        freeLearnedHashSet(learnedHash);
        return;
    }

    int targetCount = 0;

    /* Thu thập learnedKanji (vào HashSet) và targetKanji (vào mảng) */
    for (int lesson = 1; lesson <= maxLesson; lesson++) {
        int start = (lesson - 1) * KANJI_PER_LESSON;
        int end   = start + KANJI_PER_LESSON;
        if (end > allData->kanjiCount) end = allData->kanjiCount;

        for (int j = start; j < end; j++) {
            wchar_t* kanjiW = allData->kanjis[j].kanji_w;
            if (!kanjiW) continue;

            /* Thêm vào danh sách đã học (hash set) */
            addLearnedKanji(learnedHash, kanjiW);

            /* Nếu bài này được chọn → thêm vào danh sách mục tiêu targetKanjiList*/
            for (int k = 0; k < numSelected; k++) {
                if (selected[k] == lesson) {
                    targetKanjiList[targetCount++] = kanjiW;
                    break;
                }
            }
        }
    }


    int foundCount = 0;
    int maxToCheck = (maxLesson * KANJI_PER_LESSON < allData->kanjiCount) 
                     ? maxLesson * KANJI_PER_LESSON 
                     : allData->kanjiCount;

    /* Duyệt qua các từ vựng */
    for (int i = 0; i < maxToCheck; i++) {
        Kanji *kanjiEntry = &allData->kanjis[i];

        for (int v = 0; v < kanjiEntry->vocabsCount; v++) {
            Vocab *vocab = &kanjiEntry->vocabs[v];
            if (!vocab->vocab || !vocab->vocab_w) continue;

            const wchar_t* text = vocab->vocab_w;
            int hasTargetKanji = 0;
            int allKanjiKnown = 1;

            /* Duyệt từng ký tự trong từ vựng */
            for (int pos = 0; text[pos] != L'\0'; pos++) {
                wchar_t ch = text[pos];

                if (isKanjiWChar(ch)) {
                    wchar_t singleKanji[2] = {ch, L'\0'};

                    /* Kiểm tra Kanji này có chứa Kanji mục tiêu (thuộc các lessons đã chọn) không, duyệt for cơ bản */
                    if (!hasTargetKanji) {
                        for (int t = 0; t < targetCount; t++) {
                            if (wcscmp(singleKanji, targetKanjiList[t]) == 0) {
                                hasTargetKanji = 1;
                                break;
                            }
                        }
                    }

                    /* Kiểm tra Kanji này đã học chưa (dùng HashSet) */
                    if (!containsLearnedKanji(learnedHash, singleKanji)) {
                        allKanjiKnown = 0;
                        break;
                    }
                }
            }

            if (hasTargetKanji && allKanjiKnown) {
                foundCount++;
                printf("%d. %s (%s - %s): %s\n",
                       foundCount,
                       vocab->vocab,
                       vocab->hiragana ? vocab->hiragana : "-",
                       vocab->romaji   ? vocab->romaji   : "-",
                       vocab->meaning);
            }
        }
    }

    if (foundCount == 0) {
        printf("\nKhong tim thay tu vung nao thoa man dieu kien.\n");
    } else {
        printf("\nTong so tu vung tim duoc: %d\n", foundCount);
    }

    /* Giải phóng bộ nhớ */
    freeLearnedHashSet(learnedHash);
    free(targetKanjiList);

}