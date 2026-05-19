// ==================== src/filter_learned_lesson.c ====================
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

void freeLearnedHashSet(LearnedHashSet* set) {
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
        printf(CL_ERROR "  ⚠ Không có dữ liệu!\n" RESET);
        return;
    }
    
    // Giao diện nhập bài
    printf("\x1b[2J\x1b[H");
    printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
    printf(CL_BORDER "  │" BG_HIGHLIGHT "                 LỌC TỪ VỰNG ĐÃ HỌC                   " RESET CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  └────────────────────────────────────────────────────────────┘\n\n" RESET);
    
    printf(CL_TEXT "  📌 Nhập các bài đã học (cách nhau bằng dấu cách, kết thúc = 0):\n" RESET);
    printf("  " CL_LOGO);
    
    int selected[50] = {0};
    int numSelected = 0, input, maxLesson = 0;
    
    while (scanf("%d", &input) == 1 && input != 0 && numSelected < 50) {
        selected[numSelected++] = input;
        if (input > maxLesson) maxLesson = input;
    }
    
    if (numSelected == 0) {
        printf(CL_WARN "\n  ⚠ Không có bài nào được chọn.\n" RESET);
        pauseAndClear();
        return;
    }

    // Khởi tạo cấu trúc dữ liệu
    wchar_t** targetKanjiList = malloc((maxLesson * KANJI_PER_LESSON + 10) * sizeof(wchar_t*));
    LearnedHashSet* learnedHash = createLearnedHashSet();
    
    if (!targetKanjiList || !learnedHash) {
        printf(CL_ERROR "  ⚠ Lỗi cấp phát bộ nhớ!\n" RESET);
        free(targetKanjiList);
        freeLearnedHashSet(learnedHash);
        return;
    }
    
    int targetCount = 0;

    // Thu thập learnedKanji (vào HashSet) và targetKanji (vào mảng)
    for (int lesson = 1; lesson <= maxLesson; lesson++) {
        int start = (lesson - 1) * KANJI_PER_LESSON;
        int end   = start + KANJI_PER_LESSON;
        if (end > allData->kanjiCount) end = allData->kanjiCount;
        
        for (int j = start; j < end; j++) {
            wchar_t* kanjiW = allData->kanjis[j].kanji_w;
            if (!kanjiW) continue;
            
            addLearnedKanji(learnedHash, kanjiW);
            
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
                     ? maxLesson * KANJI_PER_LESSON : allData->kanjiCount;

    // Giao diện bảng kết quả
    printf("\n  " CL_PRIMARY "📖 KẾT QUẢ TỪ VỰNG\n" RESET);
    printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────────────────────────────────────┐\n" RESET);
    printf(CL_BORDER "  │ " CL_HEADER "STT │ TỪ VỰNG                        │ CÁCH ĐỌC                    │ NGHĨA" RESET "                    │\n" RESET);
    printf(CL_BORDER "  ├─────┼───────────────────────────────┼─────────────────────────────┼─────────────────────────────┤\n" RESET);

    // Duyệt qua các từ vựng
    for (int i = 0; i < maxToCheck; i++) {
        Kanji *kanjiEntry = &allData->kanjis[i];

        for (int v = 0; v < kanjiEntry->vocabsCount; v++) {
            Vocab *vocab = &kanjiEntry->vocabs[v];
            if (!vocab->vocab || !vocab->vocab_w) continue;
            
            const wchar_t* text = vocab->vocab_w;
            int hasTargetKanji = 0;
            int allKanjiKnown = 1;

            for (int pos = 0; text[pos] != L'\0'; pos++) {
                wchar_t ch = text[pos];

                if (isKanjiWChar(ch)) {
                    wchar_t singleKanji[2] = {ch, L'\0'};

                    if (!hasTargetKanji) {
                        for (int t = 0; t < targetCount; t++) {
                            if (wcscmp(singleKanji, targetKanjiList[t]) == 0) {
                                hasTargetKanji = 1;
                                break;
                            }
                        }
                    }

                    if (!containsLearnedKanji(learnedHash, singleKanji)) {
                        allKanjiKnown = 0;
                        break;
                    }
                }
            }

            if (hasTargetKanji && allKanjiKnown) {
                foundCount++;
                printf(CL_BORDER "  │ " CL_KEY "%-3d" CL_BORDER " │ " CL_LOGO "%-29s" CL_BORDER " │ " CL_TEXT "%-27s" CL_BORDER " │ %-27s │\n" RESET,
                       foundCount,
                       vocab->vocab ? vocab->vocab : "-",
                       vocab->hiragana ? vocab->hiragana : "-",
                       vocab->meaning ? vocab->meaning : "-");
            }
        }
    }

    printf(CL_BORDER "  └─────┴───────────────────────────────┴─────────────────────────────┴─────────────────────────────┘\n" RESET);

    if (foundCount == 0) {
        printf("\n  " CL_WARN "⚠ Không tìm thấy từ vựng nào thỏa mãn điều kiện.\n" RESET);
    } else {
        printf("\n  " CL_SUCCESS "✓ Tổng số từ vựng tìm được: %d\n" RESET, foundCount);
    }

    // Giải phóng bộ nhớ
    freeLearnedHashSet(learnedHash);
    free(targetKanjiList);
    getchar();
    printf("\n  " CL_DIM "▶ Nhấn Enter để tiếp tục..." RESET);
    getchar();
}