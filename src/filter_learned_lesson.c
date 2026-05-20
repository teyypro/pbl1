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
        printf("  " CL_ERROR "🚨 [LỖI] Không tìm thấy dữ liệu cấu trúc trong phân vùng.\n" RESET);
        return;
    }
    
    system("cls || clear");
    printf("\n  " BG_HIGHLIGHT BOLD " LỌC CÁC TỪ VỰNG CHỈ CHỨA CÁC KANJI ĐÃ HỌC " RESET"\n");
    printf(CL_BORDER "──────────────────────────────────────────────────────────────────\n" RESET);
    
    printf("  " CL_TEXT "Nhập các bài đã học cần trích xuất từ vựng (cách nhau bởi 1 space, kết thúc bằng số 0):\n" RESET);
    printf("   >> " CL_LOGO BOLD);
    
    int selected[50] = {0};
    int numSelected = 0, input, maxLesson = 0;
    
    while (scanf("%d", &input) == 1 && input != 0 && numSelected < 50) {
        selected[numSelected++] = input;
        if (input > maxLesson) maxLesson = input;
    }
    printf(RESET);
    
    if (numSelected == 0) {
        printf("\n  " CL_WARN "⚠ Không có phân đoạn bài học nào được ghi nhận.\n" RESET);
        printf("  " CL_DIM "Nhấn Enter để tiếp tục..." RESET);
        getchar(); getchar();
        return;
    }

    wchar_t** targetKanjiList = malloc((maxLesson * KANJI_PER_LESSON + 10) * sizeof(wchar_t*));
    LearnedHashSet* learnedHash = createLearnedHashSet();
    
    if (!targetKanjiList || !learnedHash) {
        printf("  " CL_ERROR "🚨 [LỖI] Phân bổ bộ nhớ thất bại (Heap Allocation Error).\n" RESET);
        free(targetKanjiList);
        freeLearnedHashSet(learnedHash);
        return;
    }
    
    int targetCount = 0;

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

    printf("\n  " BOLD "DANH SÁCH TỪ VỰNG THỎA MÃN ĐIỀU KIỆN LỌC\n" RESET);
    printf(CL_BORDER "──────────────────────────────────────────────────────────────────\n" RESET);

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
                
                // Tâm lý học phân phối dữ liệu:
                // STT mờ (CL_DIM) -> Từ gốc đậm nổi bật (CL_KANJI) -> Tab -> Âm đọc bản địa (CL_KANA) -> Tab -> Chuyển ngữ hệ Latinh (CL_ROMAJI) -> Tab -> Ý nghĩa chuyển ngữ đích (CL_MEANING)
                printf("  " CL_PRIMARY "┃ " CL_DIM "%0.3d." RESET " " CL_KANJI BOLD "%s" RESET " (" CL_KANA "%s" RESET " - " CL_ROMAJI "%s" RESET ") " CL_MEANING "%s\n",
                       foundCount,
                       vocab->vocab,
                       vocab->hiragana ? vocab->hiragana : "-",
                       vocab->romaji ? vocab->romaji : "-",
                       vocab->meaning ? vocab->meaning : "-");
            }
        }
    }

    printf(CL_BORDER "──────────────────────────────────────────────────────────────────\n" RESET);

    if (foundCount == 0) {
        printf("  " CL_WARN "⚠ Hệ thống không tìm thấy từ vựng nào giao thoa giữa các bài đã chọn.\n" RESET);
    } else {
        printf("  " CL_SUCCESS "✓ Tìm thấy tổng cộng %d từ vựng hợp lệ.\n" RESET, foundCount);
    }

    freeLearnedHashSet(learnedHash);
    free(targetKanjiList);
    
    getchar();
    printf("  " CL_DIM "➔ Nhấn Enter để quay lại..." RESET);
    getchar();
}