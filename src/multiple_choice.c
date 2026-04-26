#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "multiple_choice.h"
#include "../data_structures.h"

#define KANJI_PER_LESSON 16

void freeKanjiMCList(KanjiMCList *list) {
    if (!list) return;
    if (list->arr) {
        for (int i = 0; i < list->kanjiCount; i++) {
            free(list->arr[i].kanji);
            free(list->arr[i].hanViet);
        }
        free(list->arr);
    }
    free(list);
}

void freeVocabMCList(VocabMCList *list) {
    if (!list) return;
    if (list->arr) {
        for (int i = 0; i < list->vocabCount; i++) {
            free(list->arr[i].vocab);
            free(list->arr[i].furigana);
            free(list->arr[i].romaji);
            free(list->arr[i].meaning);
        }
        free(list->arr);
    }
    free(list);
}

KanjiMCList* mapToKanjiMCRange(KanjiList *L, int start, int end) {
    KanjiMCList *mcList = malloc(sizeof(KanjiMCList));
    if (!mcList) return NULL;

    mcList->kanjiCount = end - start;
    mcList->arr = malloc(mcList->kanjiCount * sizeof(KanjiMC));

    for (int i = 0; i < mcList->kanjiCount; i++) {
        mcList->arr[i].kanji = strdup(L->kanjis[start + i].kanji);
        mcList->arr[i].hanViet = strdup(L->kanjis[start + i].hanViet);
    }
    return mcList;
}

VocabMCList* mapToVocabMCRange(KanjiList *L, int start, int end) {
    VocabMCList *vList = malloc(sizeof(VocabMCList));
    if (!vList) return NULL;

    vList->vocabCount = 0;
    for (int i = start; i < end; i++) {
        vList->vocabCount += L->kanjis[i].vocabsCount;
    }

    vList->arr = malloc(vList->vocabCount * sizeof(VocabMC));
    int index = 0;
    for (int i = start; i < end; i++) {
        for (int j = 0; j < L->kanjis[i].vocabsCount; j++) {
            Vocab *v = &L->kanjis[i].vocabs[j];
            vList->arr[index].vocab = strdup(v->vocab);
            vList->arr[index].furigana = strdup(v->hiragana);
            vList->arr[index].romaji = strdup(v->romaji);
            vList->arr[index].meaning = strdup(v->meaning);
            index++;
        }
    }
    return vList;
}

char* getKanjiDataByType(KanjiMC *item, int type) {
    if (type == 1) return item->kanji;
    return item->hanViet;
}

void runMultipleChoiceTestKanji(KanjiMCList *list) {
    if (!list || list->kanjiCount < 4) {
        printf("Không đủ dữ liệu làm trắc nghiệm (cần ít nhất 4 câu).\n");
        return;
    }

    int qType, aType;
    system("cls");
    printf("--- THIẾT LẬP CÂU HỎI ---\n");
    printf("[1]. Kanji\n");
    printf("[2]. Hán Việt\n");

    printf("Chọn loại CÂU HỎI: ");
    scanf("%d", &qType);
    printf("Chọn loại CÂU TRẢ LỜI: ");
    scanf("%d", &aType);
    getchar(); 

    srand(time(NULL));
    int score = 0;
    for (int i = 0; i < list->kanjiCount; i++) {
        system("cls");
        char *questionText = getKanjiDataByType(&list->arr[i], qType);
        char *correctAnswerText = getKanjiDataByType(&list->arr[i], aType);

        printf("Câu %d/%d: [%s] tương ứng với?\n\n", i + 1, list->kanjiCount, questionText);
        char *options[4];
        int correctIdx = rand() % 4;
        options[correctIdx] = correctAnswerText;

        int usedIndices[4] = {-1, -1, -1, -1}; 
        for (int j = 0; j < 4; j++) {
            if (j == correctIdx) continue;
            int randIdx;
            int isDuplicate;
            do {
                isDuplicate = 0;
                randIdx = rand() % list->kanjiCount;
                if (randIdx == i) isDuplicate = 1;
                if (strcmp(getKanjiDataByType(&list->arr[randIdx], aType), correctAnswerText) == 0) isDuplicate = 1;

            } while (isDuplicate);
            options[j] = getKanjiDataByType(&list->arr[randIdx], aType);
        }

        for (int j = 0; j < 4; j++) printf("[%d]. %s\n", j + 1, options[j]);
        int choice;
        printf("\nLựa chọn (0 để thoát): ");
        if (scanf("%d", &choice) != 1 || choice == 0) break;

        if (choice - 1 == correctIdx) {
            printf("Chính xác!\n");
            score++;
        } else {
            printf("Sai! Đáp án đúng là: %s\n", correctAnswerText);
        }
        printf("%s : %s\n", list->arr[i].kanji, list->arr[i].hanViet);
        printf("\nNhấn Enter để tiếp tục...");
        getchar(); getchar();
    }

    system("cls");
    printf("\n======== KẾT QUẢ ========\n");
    printf("Điểm của bạn: %d/%d\n", score, list->kanjiCount);
    printf("=========================\n");
    printf("Nhấn Enter để quay lại menu.");
    getchar();
}

char* getVocabDataByType(VocabMC *item, int type) {
    switch (type) {
        case 1: return item->vocab;     // Kanji
        case 2: return item->furigana;  // Furigana
        case 3: return item->romaji;    // Romaji
        case 4: return item->meaning;   // Meaning
        default: return item->vocab;
    }
}


void runMultipleChoiceTestVocab(VocabMCList *list) {
    if (!list || list->vocabCount < 4) {
        printf("Không đủ dữ liệu làm trắc nghiệm (cần ít nhất 4 từ).\n");
        return;
    }

    int qType, aType;
    system("cls");
    printf("--- THIẾT LẬP CÂU HỎI ---\n");
    printf("[1]. Kanji\n");
    printf("[2]. Furigana\n");
    printf("[3]. Romaji\n");
    printf("[4]. Meaning\n");
    printf("\nChọn loại CÂU HỎI: ");
    scanf("%d", &qType);
    printf("Chọn loại CÂU TRẢ LỜI: ");
    scanf("%d", &aType);
    getchar();

    srand(time(NULL));
    int score = 0;

    for (int i = 0; i < list->vocabCount; i++) {
        system("cls");
        char *questionText = getVocabDataByType(&list->arr[i], qType);
        char *correctAnswerText = getVocabDataByType(&list->arr[i], aType);

        printf("Câu %d/%d: [%s] tương ứng với?\n\n", i + 1, list->vocabCount, questionText);
        char *options[4];
        int correctIdx = rand() % 4;
        options[correctIdx] = correctAnswerText;
        for (int j = 0; j < 4; j++) {
            if (j == correctIdx) continue;
            int randIdx;
            do {
                randIdx = rand() % list->vocabCount;
            } while (randIdx == i ||
                     strcmp(getVocabDataByType(&list->arr[randIdx], aType), correctAnswerText) == 0);
            options[j] = getVocabDataByType(&list->arr[randIdx], aType);
        }

        for (int j = 0; j < 4; j++) printf("[%d]. %s\n", j + 1, options[j]);

        int choice;
        printf("\nLựa chọn (0 để thoát): ");
        if (scanf("%d", &choice) != 1 || choice == 0) break;

        if (choice - 1 == correctIdx) {
            printf("Chính xác!\n");
            score++;
        } else {
            printf("Sai! Đáp án đúng là: %s\n", correctAnswerText);
        }
        printf("%s (%s - %s) : %s \n", list->arr[i].vocab, list->arr[i].furigana, list->arr[i].romaji, list->arr[i].meaning );
        printf("\nNhấn Enter để tiếp tục...");
        getchar(); getchar();
    }

    system("cls");
    printf("\n======== KẾT QUẢ ========\n");
    printf("Điểm của bạn: %d/%d\n", score, list->vocabCount);
    printf("=========================\n");
    printf("Nhấn Enter để quay lại menu.");
    getchar();
}

void runChoiceOption(KanjiList *L) {
    if (!L || L->kanjiCount == 0) return;
    
    int totalLessons = (L->kanjiCount + KANJI_PER_LESSON - 1) / KANJI_PER_LESSON;
    int lessonChoice;
    printf("Chọn bài [1-%d]: ", totalLessons);
    scanf("%d", &lessonChoice); getchar();

    int start, end;
    start = (lessonChoice - 1) * KANJI_PER_LESSON;
    end   = start + KANJI_PER_LESSON;

    int mode;
    system("cls");
    printf("\n======== CHẾ ĐỘ BÀI TẬP ========\n");
    printf("[1]. Trắc nghiệm Kanji\n");
    printf("[2]. Trắc nghiệm Từ vựng\n");
    printf("[0]. Quay lại\n");
    printf("Chọn [0-2]: ");

    if (scanf("%d", &mode) != 1 || mode == 0) {
        while (getchar() != '\n');
        return;
    }

    if (mode == 1) {
        KanjiMCList *mcList = mapToKanjiMCRange(L, start, end);
        runMultipleChoiceTestKanji(mcList);
        freeKanjiMCList(mcList);
    } else 
    if (mode == 2) {
        VocabMCList *vList = mapToVocabMCRange(L, start, end);
        runMultipleChoiceTestVocab(vList); 
        freeVocabMCList(vList);
    }
}