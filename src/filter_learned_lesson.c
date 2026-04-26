#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#ifdef _WIN32
    #include <windows.h>
#endif
#include "../data_structures.h"
#include "filter_learned_lesson.h"
#define KANJI_PER_LESSON 16

// Hàm kiểm tra một ký tự UTF-8 có phải là Kana không
bool isKanaUTF8(const char* utf8_char) {
    static const char* kana_list[] = {
        "あ", "い", "う", "え", "お", "か", "き", "く", "け", "こ",
        "さ", "し", "す", "せ", "そ", "た", "ち", "つ", "て", "と",
        "な", "に", "ぬ", "ね", "の", "は", "ひ", "ふ", "へ", "ほ",
        "ま", "み", "む", "め", "も", "や", "ゆ", "よ", "ら", "り",
        "る", "れ", "ろ", "わ", "を", "ん", "が", "ぎ", "ぐ", "げ",
        "ご", "ざ", "じ", "ず", "ぜ", "ぞ", "だ", "ぢ", "づ", "で",
        "ど", "ば", "び", "ぶ", "べ", "ぼ", "ぱ", "ぴ", "ぷ", "ぺ",
        "ぽ", "ゃ", "ゅ", "ょ", "っ", "ア", "イ", "ウ", "エ", "オ",
        "カ", "キ", "ク", "ケ", "コ", "サ", "シ", "ス", "セ", "ソ",
        "タ", "チ", "ツ", "テ", "ト", "ナ", "ニ", "ヌ", "ネ", "ノ",
        "ハ", "ヒ", "フ", "ヘ", "ホ", "マ", "ミ", "ム", "メ", "モ",
        "ヤ", "ユ", "ヨ", "ラ", "リ", "ル", "レ", "ロ", "ワ", "ヲ",
        "ン", "ガ", "ギ", "グ", "ゲ", "ゴ", "ザ", "ジ", "ズ", "ゼ",
        "ゾ", "ダ", "ヂ", "ヅ", "デ", "ド", "バ", "ビ", "ブ", "ベ",
        "ボ", "パ", "ピ", "プ", "ペ", "ポ", "ャ", "ュ", "ョ", "ッ",
        NULL
    };
    
    for (int i = 0; kana_list[i] != NULL; i++) {
        if (strcmp(utf8_char, kana_list[i]) == 0) {
            return true;
        }
    }
    return false;
}

// Hàm lấy ký tự UTF-8 tiếp theo từ chuỗi
int getUTF8Char(const char* str, char* output) {
    if (!str || !*str) return 0;
    
    unsigned char c = (unsigned char)str[0];
    int len = 0;
    
    if (c < 0x80) {
        len = 1;  // ASCII
    } else if ((c & 0xE0) == 0xC0) {
        len = 2;  // 2 bytes UTF-8
    } else if ((c & 0xF0) == 0xE0) {
        len = 3;  // 3 bytes UTF-8 (Hiragana, Katakana, Kanji)
    } else if ((c & 0xF8) == 0xF0) {
        len = 4;  // 4 bytes UTF-8
    } else {
        return 0;
    }
    
    for (int i = 0; i < len && str[i]; i++) {
        output[i] = str[i];
    }
    output[len] = '\0';
    
    return len;
}

// Hàm kiểm tra kanji có trong mảng không
bool isKanjiInArray(const char* kanji, char** array, int count) {
    for (int i = 0; i < count; i++) {
        if (strcmp(kanji, array[i]) == 0) {
            return true;
        }
    }
    return false;
}

LearnedVocabList* createLearnedList() {
    LearnedVocabList *L = malloc(sizeof(LearnedVocabList));
    L->head = NULL;
    L->count = 0;
    return L;
}

void addLearnedVocab(LearnedVocabList *L, Vocab *v) {
    LearnedVocab *newNode = malloc(sizeof(LearnedVocab));
    newNode->vocab = strdup(v->vocab);
    newNode->furigana = strdup(v->hiragana);
    newNode->romaji = strdup(v->romaji);
    newNode->meaning = strdup(v->meaning);
    newNode->next = NULL;
    if (L->head == NULL) {
        L->head = newNode;
    } else {
        LearnedVocab *temp = L->head;
        while (temp->next) temp = temp->next;
        temp->next = newNode;
    }
    L->count++;
}

void freeLearnedVocabList(LearnedVocabList *L) {
    if (!L) return;
    LearnedVocab *curr = L->head;
    while (curr) {
        LearnedVocab *next = curr->next;
        free(curr->vocab);
        free(curr->furigana);
        free(curr->romaji);
        free(curr->meaning);
        free(curr);
        curr = next;
    }
    free(L);
}

void runFilterLearnedVocab(KanjiList *allData) {
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif
    
    if (!allData || allData->kanjiCount == 0) {
        printf("Không có dữ liệu!\n");
        return;
    }
    
    int selected[50];
    printf("Nhập các bài cần review (kết thúc bằng 0): ");
    int n = 0, input;
    while (scanf("%d", &input) && input != 0 && n < 50) {
        selected[n++] = input;
    }
    getchar();
    
    if (n == 0) {
        printf("Chưa chọn bài nào!\n");
        return;
    }
    
    int maxLesson = 0;
    for (int i = 0; i < n; i++) {
        if (selected[i] > maxLesson) maxLesson = selected[i];
    }
    
    // Bước 1: Gom tất cả Kanji đã học từ lesson 1 đến maxLesson
    int maxKanji = maxLesson * KANJI_PER_LESSON;
    if (maxKanji > allData->kanjiCount) maxKanji = allData->kanjiCount;
    
    char** allLearnedKanji = malloc(maxKanji * sizeof(char*));
    int allLearnedCount = 0;
    
    for (int lesson = 1; lesson <= maxLesson; lesson++) {
        int start = (lesson - 1) * KANJI_PER_LESSON;
        int end = start + KANJI_PER_LESSON;
        if (start >= allData->kanjiCount) continue;
        if (end > allData->kanjiCount) end = allData->kanjiCount;
        
        for (int j = start; j < end; j++) {
            if (allData->kanjis[j].kanji) {
                allLearnedKanji[allLearnedCount++] = strdup(allData->kanjis[j].kanji);
            }
        }
    }
    
    // Bước 2: Gom Kanji chỉ từ các bài được chọn
    char** selectedLessonKanji = malloc(maxKanji * sizeof(char*));
    int selectedCount = 0;
    
    for (int i = 0; i < n; i++) {
        int lesson = selected[i];
        int start = (lesson - 1) * KANJI_PER_LESSON;
        int end = start + KANJI_PER_LESSON;
        if (start >= allData->kanjiCount) continue;
        if (end > allData->kanjiCount) end = allData->kanjiCount;
        
        for (int j = start; j < end; j++) {
            if (allData->kanjis[j].kanji) {
                // Kiểm tra tránh trùng lặp
                bool exists = false;
                for (int k = 0; k < selectedCount; k++) {
                    if (strcmp(selectedLessonKanji[k], allData->kanjis[j].kanji) == 0) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    selectedLessonKanji[selectedCount++] = strdup(allData->kanjis[j].kanji);
                }
            }
        }
    }
    
    printf("Tổng số kanji đã học (lesson 1->%d): %d\n", maxLesson, allLearnedCount);
    printf("Tổng số kanji trong các bài được chọn: %d\n", selectedCount);
    
    // Bước 3: Duyệt vocab và kiểm tra
    LearnedVocabList *result = createLearnedList();
    int totalChecked = 0;
    int totalAdded = 0;
    
    for (int lesson = 1; lesson <= maxLesson; lesson++) {
        int start = (lesson - 1) * KANJI_PER_LESSON;
        int end = start + KANJI_PER_LESSON;
        if (start >= allData->kanjiCount) continue;
        if (end > allData->kanjiCount) end = allData->kanjiCount;
        
        for (int i = start; i < end; i++) {
            for (int j = 0; j < allData->kanjis[i].vocabsCount; j++) {
                Vocab *v = &allData->kanjis[i].vocabs[j];
                if (!v->vocab) continue;
                
                totalChecked++;
                
                bool hasKanjiFromSelected = false;  // Có kanji từ bài được chọn không?
                bool allKanjiLearned = true;         // Tất cả kanji đã học chưa?
                
                const char* ptr = v->vocab;
                
                while (*ptr) {
                    char utf8_char[5] = {0};
                    int len = getUTF8Char(ptr, utf8_char);
                    if (len == 0) break;
                    
                    // Kiểm tra nếu không phải Kana -> là Kanji
                    if (!isKanaUTF8(utf8_char)) {
                        // Kiểm tra kanji này có thuộc bài được chọn không
                        if (isKanjiInArray(utf8_char, selectedLessonKanji, selectedCount)) {
                            hasKanjiFromSelected = true;
                        }
                        
                        // Kiểm tra kanji này đã học chưa
                        if (!isKanjiInArray(utf8_char, allLearnedKanji, allLearnedCount)) {
                            allKanjiLearned = false;
                            break;
                        }
                    }
                    
                    ptr += len;
                }
                
                // Điều kiện: 
                // 1. Có ít nhất 1 kanji thuộc bài được chọn
                // 2. Tất cả kanji đều đã học (từ lesson 1->maxLesson)
                if (hasKanjiFromSelected && allKanjiLearned) {
                    addLearnedVocab(result, v);
                    totalAdded++;
                }
            }
        }
    }
    
    // In kết quả
    printf("\n=== KẾT QUẢ ===\n");
    printf("Tổng số từ đã kiểm tra: %d\n", totalChecked);
    printf("Tổng số từ hợp lệ: %d\n", totalAdded);
    printf("\n--- DANH SÁCH TỪ VỰNG (CÓ KANJI TỪ BÀI ĐƯỢC CHỌN VÀ CHỈ CHỨA KANJI ĐÃ HỌC) ---\n");
    
    LearnedVocab *curr = result->head;
    int stt = 1;
    while (curr) {
        printf("%d. %-20s | %-20s | %-20s | %-s\n", 
               stt++,
               curr->vocab, 
               curr->furigana, 
               curr->romaji, 
               curr->meaning);
        curr = curr->next;
    }
    
    if (result->count == 0) {
        printf("Không có từ vựng nào thỏa mãn!\n");
    }
    
    // Giải phóng bộ nhớ
    for (int i = 0; i < allLearnedCount; i++) {
        free(allLearnedKanji[i]);
    }
    free(allLearnedKanji);
    
    for (int i = 0; i < selectedCount; i++) {
        free(selectedLessonKanji[i]);
    }
    free(selectedLessonKanji);
    
    freeLearnedVocabList(result);
}