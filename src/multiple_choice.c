// ==================== src/multiple_choice.c ====================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "multiple_choice.h"
#include "../data_structures.h"
#include "utils.h"

#define KANJI_PER_LESSON 16
#define MAX_INPUT_SIZE 65536

// Cấu trúc lưu dữ liệu nhập từ bàn phím
typedef struct {
    char **data;
    int count;
} RawDataList;

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
        printf(CL_ERROR "  ⚠ Không đủ dữ liệu làm trắc nghiệm (cần ít nhất 4 câu).\n" RESET);
        return;
    }

    int qType, aType;
    clearScreen();
    
    printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
    printf(CL_BORDER "  │" BG_HIGHLIGHT "                   THIẾT LẬP CÂU HỎI                    " RESET CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  ├────────────────────────────────────────────────────────────┤\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "[1]" RESET " Kanji                                            " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "[2]" RESET " Han Viet                                        " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  └────────────────────────────────────────────────────────────┘\n" RESET);
    
    printf("\n  " BOLD "Chọn loại CÂU HỎI: " RESET);
    scanf("%d", &qType);
    printf("  " BOLD "Chọn loại CÂU TRẢ LỜI: " RESET);
    scanf("%d", &aType);
    getchar();

    srand(time(NULL));
    
    int *correctFlags = (int*)calloc(list->kanjiCount, sizeof(int));
    int totalCorrect = 0;
    int roundCount = 0;
    int stop = 0;
    
    printf(CL_SUCCESS "\n  === BAT DAU LUYEN TAP KANJI ===\n" RESET);
    printf(CL_TEXT "  Ban se duoc hoi cho den khi tra loi dung tat ca cac cau.\n" RESET);
    printf(CL_WARN "  Nhan 0 o bat ky cau hoi nao de DUNG lai.\n" RESET);
    printf("\n  " CL_KEY "Nhan Enter de bat dau..." RESET);
    getchar();
    
    while (totalCorrect < list->kanjiCount && !stop) {
        roundCount++;
        int roundScore = 0;
        int questionsInRound = 0;
        
        clearScreen();
        printf(CL_BORDER "  ╔════════════════════════════════════════════════════════════╗\n" RESET);
        printf(CL_BORDER "  ║" BG_HIGHLIGHT "                       LUOT %d                            " RESET CL_BORDER "║\n", roundCount);
        printf(CL_BORDER "  ╠════════════════════════════════════════════════════════════╣\n" RESET);
        printf(CL_BORDER "  ║  " CL_KEY "Tien do: %d/%d cau da dung" RESET "                                      ║\n", totalCorrect, list->kanjiCount);
        printf(CL_BORDER "  ╚════════════════════════════════════════════════════════════╝\n" RESET);
        
        int recent = list->kanjiCount - totalCorrect;
        for (int i = 0; i < list->kanjiCount && !stop; i++) {
            if (correctFlags[i] == 1) continue;
            
            questionsInRound++;
            clearScreen();
            
            printf(CL_BORDER "  ╔════════════════════════════════════════════════════════════╗\n" RESET);
            printf(CL_BORDER "  ║" BG_HIGHLIGHT "            LUOT %d - CAU %d/%d                     " RESET CL_BORDER "║\n", roundCount, questionsInRound, recent);
            printf(CL_BORDER "  ╠════════════════════════════════════════════════════════════╣\n" RESET);
            printf(CL_BORDER "  ║  " CL_KEY "Tien do: %d/%d cau da dung" RESET "                                      ║\n", totalCorrect, list->kanjiCount);
            printf(CL_BORDER "  ║  " CL_DIM "(Nhan 0 de dung luyen tap)" RESET "                                 ║\n" RESET);
            printf(CL_BORDER "  ╚════════════════════════════════════════════════════════════╝\n\n" RESET);
            
            char *questionText = getKanjiDataByType(&list->arr[i], qType);
            char *correctAnswerText = getKanjiDataByType(&list->arr[i], aType);
            
            printf(CL_PRIMARY "  ┌────────────────────────────────────────────────────────┐\n");
            printf("  │ " CL_TEXT "Câu hỏi: [%s] tương ứng với?" CL_PRIMARY "                     │\n", questionText);
            printf("  └────────────────────────────────────────────────────────┘\n\n" RESET);
            
            char *options[4];
            int correctIdx = rand() % 4;
            options[correctIdx] = correctAnswerText;
            
            for (int j = 0; j < 4; j++) {
                if (j == correctIdx) continue;
                
                int randIdx;
                int isDuplicate;
                do {
                    isDuplicate = 0;
                    randIdx = rand() % list->kanjiCount;
                    
                    if (randIdx == i) isDuplicate = 1;
                    
                    char *tempAnswer = getKanjiDataByType(&list->arr[randIdx], aType);
                    if (strcmp(tempAnswer, correctAnswerText) == 0) isDuplicate = 1;
                    
                    for (int k = 0; k < j; k++) {
                        if (k == correctIdx) continue;
                        if (options[k] && strcmp(tempAnswer, options[k]) == 0) {
                            isDuplicate = 1;
                            break;
                        }
                    }
                    
                } while (isDuplicate);
                
                options[j] = getKanjiDataByType(&list->arr[randIdx], aType);
            }
            
            for (int j = 0; j < 4; j++) {
                printf("    " CL_WARN "[%d]" RESET " %s\n", j + 1, options[j]);
            }
            
            int choice;
            int validChoice = 0;
            do {
                printf(CL_KEY "\n  >> Lựa chọn: " RESET);
                if (scanf("%d", &choice) != 1) {
                    while(getchar() != '\n');
                    printf(CL_ERROR "    Vui lòng nhập số!\n" RESET);
                    continue;
                }
                if (choice == 0) {
                    stop = 1;
                    validChoice = 1;
                    break;
                }
                if (choice >= 1 && choice <= 4) {
                    validChoice = 1;
                } else {
                    printf(CL_WARN "    Vui lòng chọn 1-4 hoặc 0 để dừng!\n" RESET);
                }
            } while (!validChoice);
            
            if (stop) break;
            
            if (choice - 1 == correctIdx) {
                printf(CL_SUCCESS "\n  ✓ CHÍNH XÁC!\n" RESET);
                correctFlags[i] = 1;
                totalCorrect++;
                roundScore++;
            } else {
                printf(CL_ERROR "\n  ✗ SAI! Đáp án đúng là: " CL_SUCCESS "%s\n" RESET, correctAnswerText);
            }
            
            printf(CL_DIM "\n  %s : %s\n" RESET, list->arr[i].kanji, list->arr[i].hanViet);
            
            if (totalCorrect < list->kanjiCount && !stop) {
                printf(CL_DIM "\n  Nhấn Enter để tiếp tục..." RESET);
                getchar(); getchar();
            }
        }
        
        if (!stop && totalCorrect < list->kanjiCount) {
            clearScreen();
            printf(CL_BORDER "  ╔════════════════════════════════════════════════════════════╗\n" RESET);
            printf(CL_BORDER "  ║" BG_HIGHLIGHT "               KET THUC LUOT %d                       " RESET CL_BORDER "║\n", roundCount);
            printf(CL_BORDER "  ╠════════════════════════════════════════════════════════════╣\n" RESET);
            printf(CL_BORDER "  ║  So cau dung trong luot nay: " CL_KEY "%-4d/%d" RESET "                           ║\n", roundScore, questionsInRound);
            printf(CL_BORDER "  ║  Tong tien do: " CL_KEY "%d/%d cau da dung" RESET "                               ║\n", totalCorrect, list->kanjiCount);
            printf(CL_BORDER "  ║  Con " CL_WARN "%d cau chua dung" RESET "                                        ║\n", list->kanjiCount - totalCorrect);
            printf(CL_BORDER "  ╚════════════════════════════════════════════════════════════╝\n" RESET);
            
            int reviewChoice;
            printf(CL_TEXT "\n  Ban co muon lam lai cac cau sai khong? " RESET);
            printf(CL_KEY "(1: Co / 0: Thoat): " RESET);
            if (scanf("%d", &reviewChoice) != 1) {
                reviewChoice = 0;
            }
            getchar();
            
            if (reviewChoice == 0) {
                stop = 1;
                printf(CL_WARN "\n  ⚠ Ban da chon thoat.\n" RESET);
            } else {
                printf(CL_TEXT "\n  Tiep tuc luyen tap cac cau sai...\n" RESET);
                printf(CL_DIM "  Nhan Enter de sang luot tiep theo..." RESET);
                getchar();
            }
        }
    }
    
    clearScreen();
    
    if (stop && totalCorrect < list->kanjiCount) {
        printf(CL_BORDER "  ╔════════════════════════════════════════════════════════════╗\n" RESET);
        printf(CL_BORDER "  ║" CL_WARN "                     ⏸️  DA DUNG LUYEN TAP ⏸️                    " RESET CL_BORDER "║\n" RESET);
        printf(CL_BORDER "  ╠════════════════════════════════════════════════════════════╣\n" RESET);
        printf(CL_BORDER "  ║  Ban da dung lai o luot %-4d                              ║\n", roundCount);
        printf(CL_BORDER "  ║  So cau da lam dung: " CL_KEY "%-4d/%d" RESET "                              ║\n", totalCorrect, list->kanjiCount);
        printf(CL_BORDER "  ║                                                              ║\n" RESET);
        printf(CL_BORDER "  ║  " CL_DIM "Hen gap lai ban o lan sau!" RESET "                                ║\n" RESET);
        printf(CL_BORDER "  ╚════════════════════════════════════════════════════════════╝\n" RESET);
    } else if (totalCorrect == list->kanjiCount) {
        printf(CL_BORDER "  ╔════════════════════════════════════════════════════════════╗\n" RESET);
        printf(CL_BORDER "  ║" CL_SUCCESS "                     🎉 CHÚC MỪNG! 🎉                     " RESET CL_BORDER "║\n" RESET);
        printf(CL_BORDER "  ╠════════════════════════════════════════════════════════════╣\n" RESET);
        printf(CL_BORDER "  ║  Ban da tra loi dung TAT CA %-4d cau!                        ║\n", list->kanjiCount);
        printf(CL_BORDER "  ║  So luot luyen tap: " CL_KEY "%d" RESET "                                            ║\n", roundCount);
        printf(CL_BORDER "  ║                                                              ║\n" RESET);
        printf(CL_BORDER "  ║" CL_SUCCESS "                     ★  HOAN HAO  ★                       " RESET CL_BORDER "║\n" RESET);
        printf(CL_BORDER "  ╚════════════════════════════════════════════════════════════╝\n" RESET);
    }
    
    free(correctFlags);
    printf(CL_DIM "\n  Nhan Enter de quay lai menu." RESET);
    getchar(); getchar();
}

char* getVocabDataByType(VocabMC *item, int type) {
    switch (type) {
        case 1: return item->vocab;
        case 2: return item->furigana;
        case 3: return item->romaji;
        case 4: return item->meaning;
        default: return item->vocab;
    }
}

void runMultipleChoiceTestVocab(VocabMCList *list) {
    if (!list || list->vocabCount < 4) {
        printf(CL_ERROR "  ⚠ Không đủ dữ liệu làm trắc nghiệm (cần ít nhất 4 từ).\n" RESET);
        return;
    }

    int qType, aType;
    clearScreen();
    
    printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
    printf(CL_BORDER "  │" BG_HIGHLIGHT "                   THIẾT LẬP CÂU HỎI                    " RESET CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  ├────────────────────────────────────────────────────────────┤\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "[1]" RESET " Kanji    " CL_KEY "[2]" RESET " Furigana    " CL_KEY "[3]" RESET " Romaji    " CL_KEY "[4]" RESET " Meaning  " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  └────────────────────────────────────────────────────────────┘\n" RESET);
    
    printf("\n  " BOLD "Chọn loại CÂU HỎI: " RESET);
    scanf("%d", &qType);
    printf("  " BOLD "Chọn loại CÂU TRẢ LỜI: " RESET);
    scanf("%d", &aType);
    getchar();

    srand(time(NULL));
    
    int *correctFlags = (int*)calloc(list->vocabCount, sizeof(int));
    int totalCorrect = 0;
    int roundCount = 0;
    int stop = 0;
    
    printf(CL_SUCCESS "\n  === BAT DAU LUYEN TAP ===\n" RESET);
    printf(CL_TEXT "  Ban se duoc hoi cho den khi tra loi dung tat ca cac cau.\n" RESET);
    printf(CL_WARN "  Nhan 0 o bat ky cau hoi nao de DUNG lai.\n" RESET);
    printf("\n  " CL_KEY "Nhan Enter de bat dau..." RESET);
    getchar();
    
    while (totalCorrect < list->vocabCount && !stop) {
        roundCount++;
        int roundScore = 0;
        int questionsInRound = 0;
        
        clearScreen();
        printf(CL_BORDER "  ╔════════════════════════════════════════════════════════════╗\n" RESET);
        printf(CL_BORDER "  ║" BG_HIGHLIGHT "                       LUOT %d                            " RESET CL_BORDER "║\n", roundCount);
        printf(CL_BORDER "  ╠════════════════════════════════════════════════════════════╣\n" RESET);
        printf(CL_BORDER "  ║  " CL_KEY "Tien do: %d/%d cau da dung" RESET "                                      ║\n", totalCorrect, list->vocabCount);
        printf(CL_BORDER "  ╚════════════════════════════════════════════════════════════╝\n" RESET);
        
        int recent = list->vocabCount - totalCorrect;
        for (int i = 0; i < list->vocabCount && !stop; i++) {
            if (correctFlags[i] == 1) continue;
            
            questionsInRound++;
            clearScreen();
            
            printf(CL_BORDER "  ╔════════════════════════════════════════════════════════════╗\n" RESET);
            printf(CL_BORDER "  ║" BG_HIGHLIGHT "            LUOT %d - CAU %d/%d                     " RESET CL_BORDER "║\n", roundCount, questionsInRound, recent);
            printf(CL_BORDER "  ╠════════════════════════════════════════════════════════════╣\n" RESET);
            printf(CL_BORDER "  ║  " CL_KEY "Tien do: %d/%d cau da dung" RESET "                                      ║\n", totalCorrect, list->vocabCount);
            printf(CL_BORDER "  ║  " CL_DIM "(Nhan 0 de dung luyen tap)" RESET "                                 ║\n" RESET);
            printf(CL_BORDER "  ╚════════════════════════════════════════════════════════════╝\n\n" RESET);
            
            char *questionText = getVocabDataByType(&list->arr[i], qType);
            char *correctAnswerText = getVocabDataByType(&list->arr[i], aType);
            
            printf(CL_PRIMARY "  ┌────────────────────────────────────────────────────────┐\n");
            printf("  │ " CL_TEXT "Câu hỏi: [%s] tương ứng với?" CL_PRIMARY "                     │\n", questionText);
            printf("  └────────────────────────────────────────────────────────┘\n\n" RESET);
            
            char *options[4];
            int correctIdx = rand() % 4;
            options[correctIdx] = correctAnswerText;
            
            for (int j = 0; j < 4; j++) {
                if (j == correctIdx) continue;
                
                int randIdx;
                int isDuplicate;
                do {
                    isDuplicate = 0;
                    randIdx = rand() % list->vocabCount;
                    
                    if (randIdx == i) isDuplicate = 1;
                    
                    char *tempAnswer = getVocabDataByType(&list->arr[randIdx], aType);
                    if (strcmp(tempAnswer, correctAnswerText) == 0) isDuplicate = 1;
                    
                    for (int k = 0; k < j; k++) {
                        if (k == correctIdx) continue;
                        if (options[k] && strcmp(tempAnswer, options[k]) == 0) {
                            isDuplicate = 1;
                            break;
                        }
                    }
                    
                } while (isDuplicate);
                
                options[j] = getVocabDataByType(&list->arr[randIdx], aType);
            }
            
            for (int j = 0; j < 4; j++) {
                printf("    " CL_WARN "[%d]" RESET " %s\n", j + 1, options[j]);
            }
            
            int choice;
            int validChoice = 0;
            do {
                printf(CL_KEY "\n  >> Lựa chọn: " RESET);
                if (scanf("%d", &choice) != 1) {
                    while(getchar() != '\n');
                    printf(CL_ERROR "    Vui lòng nhập số!\n" RESET);
                    continue;
                }
                if (choice == 0) {
                    stop = 1;
                    validChoice = 1;
                    break;
                }
                if (choice >= 1 && choice <= 4) {
                    validChoice = 1;
                } else {
                    printf(CL_WARN "    Vui lòng chọn 1-4 hoặc 0 để dừng!\n" RESET);
                }
            } while (!validChoice);
            
            if (stop) break;
            
            if (choice - 1 == correctIdx) {
                printf(CL_SUCCESS "\n  ✓ CHÍNH XÁC!\n" RESET);
                correctFlags[i] = 1;
                totalCorrect++;
                roundScore++;
            } else {
                printf(CL_ERROR "\n  ✗ SAI! Đáp án đúng là: " CL_SUCCESS "%s\n" RESET, correctAnswerText);
            }
            
            printf(CL_DIM "\n  %s (%s - %s) : %s\n" RESET, 
                   list->arr[i].vocab, 
                   list->arr[i].furigana, 
                   list->arr[i].romaji, 
                   list->arr[i].meaning);
            
            if (totalCorrect < list->vocabCount && !stop) {
                printf(CL_DIM "\n  Nhấn Enter để tiếp tục..." RESET);
                getchar(); getchar();
            }
        }
        
        if (!stop && totalCorrect < list->vocabCount) {
            clearScreen();
            printf(CL_BORDER "  ╔════════════════════════════════════════════════════════════╗\n" RESET);
            printf(CL_BORDER "  ║" BG_HIGHLIGHT "               KET THUC LUOT %d                       " RESET CL_BORDER "║\n", roundCount);
            printf(CL_BORDER "  ╠════════════════════════════════════════════════════════════╣\n" RESET);
            printf(CL_BORDER "  ║  So cau dung trong luot nay: " CL_KEY "%-4d/%d" RESET "                           ║\n", roundScore, questionsInRound);
            printf(CL_BORDER "  ║  Tong tien do: " CL_KEY "%d/%d cau da dung" RESET "                               ║\n", totalCorrect, list->vocabCount);
            printf(CL_BORDER "  ║  Con " CL_WARN "%d cau chua dung" RESET "                                        ║\n", list->vocabCount - totalCorrect);
            printf(CL_BORDER "  ╚════════════════════════════════════════════════════════════╝\n" RESET);
            
            int reviewChoice;
            printf(CL_TEXT "\n  Ban co muon lam lai cac cau sai khong? " RESET);
            printf(CL_KEY "(1: Co / 0: Thoat): " RESET);
            if (scanf("%d", &reviewChoice) != 1) {
                reviewChoice = 0;
            }
            getchar();
            
            if (reviewChoice == 0) {
                stop = 1;
                printf(CL_WARN "\n  ⚠ Ban da chon thoat.\n" RESET);
            } else {
                printf(CL_TEXT "\n  Tiep tuc luyen tap cac cau sai...\n" RESET);
                printf(CL_DIM "  Nhan Enter de sang luot tiep theo..." RESET);
                getchar();
            }
        }
    }
    
    clearScreen();
    
    if (stop && totalCorrect < list->vocabCount) {
        printf(CL_BORDER "  ╔════════════════════════════════════════════════════════════╗\n" RESET);
        printf(CL_BORDER "  ║" CL_WARN "                     ⏸️  DA DUNG LUYEN TAP ⏸️                    " RESET CL_BORDER "║\n" RESET);
        printf(CL_BORDER "  ╠════════════════════════════════════════════════════════════╣\n" RESET);
        printf(CL_BORDER "  ║  Ban da dung lai o luot %-4d                              ║\n", roundCount);
        printf(CL_BORDER "  ║  So cau da lam dung: " CL_KEY "%-4d/%d" RESET "                              ║\n", totalCorrect, list->vocabCount);
        printf(CL_BORDER "  ║                                                              ║\n" RESET);
        printf(CL_BORDER "  ║  " CL_DIM "Hen gap lai ban o lan sau!" RESET "                                ║\n" RESET);
        printf(CL_BORDER "  ╚════════════════════════════════════════════════════════════╝\n" RESET);
    } else if (totalCorrect == list->vocabCount) {
        printf(CL_BORDER "  ╔════════════════════════════════════════════════════════════╗\n" RESET);
        printf(CL_BORDER "  ║" CL_SUCCESS "                     🎉 CHÚC MỪNG! 🎉                     " RESET CL_BORDER "║\n" RESET);
        printf(CL_BORDER "  ╠════════════════════════════════════════════════════════════╣\n" RESET);
        printf(CL_BORDER "  ║  Ban da tra loi dung TAT CA %-4d cau!                        ║\n", list->vocabCount);
        printf(CL_BORDER "  ║  So luot luyen tap: " CL_KEY "%d" RESET "                                            ║\n", roundCount);
        printf(CL_BORDER "  ║                                                              ║\n" RESET);
        printf(CL_BORDER "  ║" CL_SUCCESS "                     ★  HOAN HAO  ★                       " RESET CL_BORDER "║\n" RESET);
        printf(CL_BORDER "  ╚════════════════════════════════════════════════════════════╝\n" RESET);
    }
    
    free(correctFlags);
    printf(CL_DIM "\n  Nhan Enter de quay lai menu." RESET);
    getchar(); getchar();
}

// Giải phóng RawDataList
void freeRawDataList(RawDataList *rawList) {
    if (!rawList) return;
    for (int i = 0; i < rawList->count; i++) {
        free(rawList->data[i]);
    }
    free(rawList->data);
    free(rawList);
}

// Hàm đọc dữ liệu từ bàn phím
RawDataList* readDataFromKeyboard() {
    RawDataList *rawList = malloc(sizeof(RawDataList));
    if (!rawList) return NULL;

    rawList->data = NULL;
    rawList->count = 0;

    clearScreen();
    printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
    printf(CL_BORDER "  │" BG_HIGHLIGHT "               NHẬP DỮ LIỆU TỪ BÀN PHÍM                 " RESET CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  ├────────────────────────────────────────────────────────────┤\n" RESET);
    printf(CL_BORDER "  │  " CL_DIM "- Paste du lieu tu Excel/Google Sheets" RESET "                         │\n" RESET);
    printf(CL_BORDER "  │  " CL_DIM "- Moi dong la 1 muc (cach nhau bang TAB)" RESET "                     │\n" RESET);
    printf(CL_BORDER "  │  " CL_DIM "- Du lieu Kanji: Kanji\tHan Viet" RESET "                           │\n" RESET);
    printf(CL_BORDER "  │  " CL_DIM "- Du lieu Tu vung: Kanji\tFurigana\tRomaji\tNghia" RESET "            │\n" RESET);
    printf(CL_BORDER "  │  " CL_DIM "- Nhan Enter 2 lan lien tiep de ket thuc" RESET "                 │\n" RESET);
    printf(CL_BORDER "  └────────────────────────────────────────────────────────────┘\n" RESET);
    printf(CL_KEY "\n  👉 Bat dau paste du lieu:\n\n" RESET);

    char lineBuffer[4096];
    int emptyLineCount = 0;

    while (1) {
        printf("  ");
        inputString(lineBuffer, sizeof(lineBuffer));

        char *trimmed = lineBuffer;
        while (*trimmed == ' ' || *trimmed == '\t') trimmed++;

        if (strlen(trimmed) == 0) {
            emptyLineCount++;
            if (emptyLineCount >= 2) {
                break;
            }
            continue;
        }

        emptyLineCount = 0;

        rawList->data = realloc(rawList->data, (rawList->count + 1) * sizeof(char*));
        if (!rawList->data) {
            freeRawDataList(rawList);
            return NULL;
        }

        rawList->data[rawList->count] = strdup(trimmed);
        rawList->count++;
    }

    if (rawList->count == 0) {
        printf(CL_WARN "\n  ⚠ Khong co du lieu nao duoc nhap!\n" RESET);
        free(rawList);
        return NULL;
    }

    printf(CL_SUCCESS "\n  ✓ Da nhan %d dong du lieu.\n" RESET, rawList->count);
    return rawList;
}

// Chuyển đổi thành KanjiMCList
KanjiMCList* convertToKanjiMCList(RawDataList *rawList) {
    if (!rawList || rawList->count == 0) return NULL;
    
    KanjiMCList *mcList = malloc(sizeof(KanjiMCList));
    if (!mcList) return NULL;
    
    mcList->kanjiCount = rawList->count;
    mcList->arr = malloc(mcList->kanjiCount * sizeof(KanjiMC));
    
    for (int i = 0; i < rawList->count; i++) {
        char *lineCopy = strdup(rawList->data[i]);
        
        mcList->arr[i].kanji = strdup("");
        mcList->arr[i].hanViet = strdup("");
        
        char *token = strtok(lineCopy, "\t");
        if (token) {
            free(mcList->arr[i].kanji);
            mcList->arr[i].kanji = strdup(token);
            token = strtok(NULL, "\t");
            if (token) {
                free(mcList->arr[i].hanViet);
                mcList->arr[i].hanViet = strdup(token);
            }
        }
        
        free(lineCopy);
    }
    
    return mcList;
}

// Chuyển đổi thành VocabMCList
VocabMCList* convertToVocabMCList(RawDataList *rawList) {
    if (!rawList || rawList->count == 0) return NULL;
    
    VocabMCList *vList = malloc(sizeof(VocabMCList));
    if (!vList) return NULL;
    
    vList->vocabCount = rawList->count;
    vList->arr = malloc(vList->vocabCount * sizeof(VocabMC));
    
    for (int i = 0; i < rawList->count; i++) {
        char *lineCopy = strdup(rawList->data[i]);
        
        vList->arr[i].vocab = strdup("");
        vList->arr[i].furigana = strdup("");
        vList->arr[i].romaji = strdup("");
        vList->arr[i].meaning = strdup("");
        
        char *token = strtok(lineCopy, "\t");
        int fieldIndex = 0;
        
        while (token != NULL && fieldIndex < 4) {
            switch (fieldIndex) {
                case 0:
                    free(vList->arr[i].vocab);
                    vList->arr[i].vocab = strdup(token);
                    break;
                case 1:
                    free(vList->arr[i].furigana);
                    vList->arr[i].furigana = strdup(token);
                    break;
                case 2:
                    free(vList->arr[i].romaji);
                    vList->arr[i].romaji = strdup(token);
                    break;
                case 3:
                    free(vList->arr[i].meaning);
                    vList->arr[i].meaning = strdup(token);
                    break;
            }
            token = strtok(NULL, "\t");
            fieldIndex++;
        }
        
        free(lineCopy);
    }
    
    return vList;
}

// Hàm chính cho nhập dữ liệu từ bàn phím
void runMultipleChoiceWithKeyboardInput() {
    RawDataList *rawList = readDataFromKeyboard();
    if (!rawList) {
        printf(CL_DIM "\n  Nhan Enter de quay lai..." RESET);
        getchar();
        return;
    }
    
    clearScreen();
    printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
    printf(CL_BORDER "  │" BG_HIGHLIGHT "                  CHẾ ĐỘ BÀI TẬP                       " RESET CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  ├────────────────────────────────────────────────────────────┤\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "[1]" RESET " Trac nghiem Kanji                                  " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "[2]" RESET " Trac nghiem Tu vung                                " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "[0]" RESET " Quay lai                                         " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  └────────────────────────────────────────────────────────────┘\n" RESET);
    printf("\n  " BOLD "Chon [0-2]: " RESET);
    
    int mode;
    if (scanf("%d", &mode) != 1 || mode == 0) {
        freeRawDataList(rawList);
        while (getchar() != '\n');
        return;
    }
    getchar();
    
    if (mode == 1) {
        KanjiMCList *mcList = convertToKanjiMCList(rawList);
        if (mcList && mcList->kanjiCount >= 4) {
            printf(CL_SUCCESS "\n  ✓ Da tao danh sach %d Kanji.\n" RESET, mcList->kanjiCount);
            printf(CL_DIM "  Nhan Enter de bat dau trac nghiem..." RESET);
            getchar();
            runMultipleChoiceTestKanji(mcList);
            freeKanjiMCList(mcList);
        } else {
            printf(CL_WARN "\n  ⚠ Khong du du lieu (can it nhat 4 Kanji).\n" RESET);
            printf(CL_DIM "  Nhan Enter de tiep tuc..." RESET);
            getchar();
        }
    } else if (mode == 2) {
        VocabMCList *vList = convertToVocabMCList(rawList);
        if (vList && vList->vocabCount >= 4) {
            printf(CL_SUCCESS "\n  ✓ Da tao danh sach %d tu vung.\n" RESET, vList->vocabCount);
            printf(CL_DIM "  Nhan Enter de bat dau trac nghiem..." RESET);
            getchar();
            runMultipleChoiceTestVocab(vList);
            freeVocabMCList(vList);
        } else {
            printf(CL_WARN "\n  ⚠ Khong du du lieu (can it nhat 4 tu vung).\n" RESET);
            printf(CL_DIM "  Nhan Enter de tiep tuc..." RESET);
            getchar();
        }
    }
    
    freeRawDataList(rawList);
}

void runChoiceOption(KanjiList *L) {
    if (!L || L->kanjiCount == 0) return;
    
    int totalLessons = (L->kanjiCount + KANJI_PER_LESSON - 1) / KANJI_PER_LESSON;
    int lessonChoice;
    
    clearScreen();
    printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
    printf(CL_BORDER "  │" BG_HIGHLIGHT "                     MENU BÀI TẬP                       " RESET CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  ├────────────────────────────────────────────────────────────┤\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "[1]" RESET " Chon bai hoc [1-%d]                                 " CL_BORDER "│\n", totalLessons);
    printf(CL_BORDER "  │  " CL_KEY "[2]" RESET " Nhap du lieu tu ban phim                           " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "[0]" RESET " Quay lai                                         " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  └────────────────────────────────────────────────────────────┘\n" RESET);
    printf("\n  " BOLD "Chon: " RESET);
    
    int mainChoice;
    if (scanf("%d", &mainChoice) != 1 || mainChoice == 0) {
        while (getchar() != '\n');
        return;
    }
    getchar();
    
    if (mainChoice == 2) {
        runMultipleChoiceWithKeyboardInput();
        return;
    }
    
    printf(CL_TEXT "\n  Chon bai [1-%d]: " RESET, totalLessons);
    scanf("%d", &lessonChoice); getchar();

    if (lessonChoice < 1 || lessonChoice > totalLessons) {
        printf(CL_ERROR "  ⚠ Bai hoc khong hop le!\n" RESET);
        printf(CL_DIM "  Nhan Enter de quay lai..." RESET);
        getchar();
        return;
    }

    int start, end;
    start = (lessonChoice - 1) * KANJI_PER_LESSON;
    end   = start + KANJI_PER_LESSON;
    if (end > L->kanjiCount) end = L->kanjiCount;

    clearScreen();
    printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
    printf(CL_BORDER "  │" BG_HIGHLIGHT "                  CHẾ ĐỘ BÀI TẬP                       " RESET CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  ├────────────────────────────────────────────────────────────┤\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "[1]" RESET " Trac nghiem Kanji                                  " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "[2]" RESET " Trac nghiem Tu vung                                " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "[0]" RESET " Quay lai                                         " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  └────────────────────────────────────────────────────────────┘\n" RESET);
    printf("\n  " BOLD "Chon: " RESET);

    int mode;
    if (scanf("%d", &mode) != 1 || mode == 0) {
        while (getchar() != '\n');
        return;
    }

    if (mode == 1) {
        KanjiMCList *mcList = mapToKanjiMCRange(L, start, end);
        runMultipleChoiceTestKanji(mcList);
        freeKanjiMCList(mcList);
    } else if (mode == 2) {
        VocabMCList *vList = mapToVocabMCRange(L, start, end);
        runMultipleChoiceTestVocab(vList); 
        freeVocabMCList(vList);
    }
}