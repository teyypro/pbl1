#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
#endif

#include "../data_structures.h"
#include "filter_learned_lesson.h"

#define KANJI_PER_LESSON 16

/* Thay thế bool bằng int (1 là true, 0 là false) */

int isKanaUTF8(const char* utf8_char) {
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
    int i;
    for (i = 0; kana_list[i] != NULL; i++) {
        if (strcmp(utf8_char, kana_list[i]) == 0) return 1;
    }
    return 0;
}

int getUTF8Char(const char* str, char* output) {
    if (!str || !*str) return 0;
    unsigned char c = (unsigned char)str[0];
    int len = (c < 0x80) ? 1 : ((c & 0xE0) == 0xC0) ? 2 : ((c & 0xF0) == 0xE0) ? 3 : 4;
    
    int i;
    for (i = 0; i < len && str[i]; i++) output[i] = str[i];
    output[len] = '\0';
    return len;
}

/* Kiểm tra nhanh một chuỗi có nằm trong danh sách không */
int isInArray(const char* target, char** array, int count) {
    int i;
    for (i = 0; i < count; i++) {
        if (strcmp(target, array[i]) == 0) return 1;
    }
    return 0;
}

void runFilterLearnedVocab(KanjiList *allData) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    if (!allData || allData->kanjiCount == 0) return;

    int selected[50], n = 0, input, maxLesson = 0;
    printf("Nhap cac bai can review (ket thuc bang 0): ");
    while (scanf("%d", &input) && input != 0 && n < 50) {
        selected[n++] = input;
        if (input > maxLesson) maxLesson = input;
    }

    /* Bước 1 & 2: Thu thập Kanji mục tiêu và Kanji đã biết */
    int limit = maxLesson * KANJI_PER_LESSON;
    char** learnedKanji = malloc(limit * sizeof(char*));
    char** targetKanji = malloc(limit * sizeof(char*));
    int learnedCount = 0, targetCount = 0;

    int i, j, k;
    for (i = 1; i <= maxLesson; i++) {
        int start = (i - 1) * KANJI_PER_LESSON;
        for (j = start; j < start + KANJI_PER_LESSON && j < allData->kanjiCount; j++) {
            char* kStr = allData->kanjis[j].kanji;
            
            // Thêm vào danh sách "đã học"
            learnedKanji[learnedCount++] = kStr;

            // Nếu thuộc bài được chọn, thêm vào danh sách "mục tiêu"
            for (k = 0; k < n; k++) {
                if (selected[k] == i) {
                    targetKanji[targetCount++] = kStr;
                    break;
                }
            }
        }
    }

    /* Bước 3: Lọc từ vựng */
    printf("\n--- KET QUA LOC ---\n");
    for (i = 0; i < learnedCount; i++) {
        Kanji *kj = &allData->kanjis[i];
        for (j = 0; j < kj->vocabsCount; j++) {
            Vocab *v = &kj->vocabs[j];
            const char* ptr = v->vocab;
            int hasTarget = 0, allKnown = 1;

            while (*ptr) {
                char utf8[5];
                int len = getUTF8Char(ptr, utf8);
                if (!isKanaUTF8(utf8) && (unsigned char)utf8[0] >= 0x80) {
                    if (isInArray(utf8, targetKanji, targetCount)) hasTarget = 1;
                    if (!isInArray(utf8, learnedKanji, learnedCount)) {
                        allKnown = 0;
                        break;
                    }
                }
                ptr += len;
            }

            if (hasTarget && allKnown) {
                printf("- %s (%s): %s\n", v->vocab, v->hiragana, v->meaning);
            }
        }
    }
    getchar();
    free(learnedKanji);
    free(targetKanji);
}