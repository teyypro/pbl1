// ==================== src/lessons_management.c ====================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
#endif
#include "lessons_management.h"
#include "../data_structures.h"
#include "utils.h"

#define KANJI_PER_LESSON 16

void displayAllLessons(KanjiList *L) {
    int totalLessons = L->kanjiCount / KANJI_PER_LESSON;
    int i, j;

    printf("\n" CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
    printf(CL_BORDER "  │" BG_HIGHLIGHT "                     DANH SÁCH BÀI HỌC                      " RESET CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  └────────────────────────────────────────────────────────────┘\n\n" RESET);

    for (i = 0; i < totalLessons; i++) {
        int startIndex = i * KANJI_PER_LESSON;
        int endIndex = startIndex + KANJI_PER_LESSON - 1;
        if (endIndex >= L->kanjiCount) endIndex = L->kanjiCount - 1;
        
        printf(CL_KEY "  Bài %2d:" RESET " (%3d - %3d) : ", i + 1, startIndex + 1, endIndex + 1);
        
        for (j = 0; j < KANJI_PER_LESSON; j++) {
            printf(CL_LOGO "%s" RESET "  ", L->kanjis[j + startIndex].kanji);
        }
        printf("\n\n");
    }
}

void displayDetailedKanji(Kanji *k) {
    if (k == NULL) return;

    system("cls || clear");
    
    printf(CL_PRIMARY "╔════════════════════════════════════════════════════════════════════╗\n");
    printf("║ " CL_HEADER BOLD "                    THÔNG TIN CHI TIẾT KANJI                    " CL_PRIMARY "║\n");
    printf("╚════════════════════════════════════════════════════════════════════╝\n" CL_RESET);

    printf(BOLD "  %-12s " CL_RESET ": %-15s " BOLD "STT" CL_RESET ": %d\n", 
           "Hán Việt", k->hanViet ? k->hanViet : "N/A", k->stt);
    printf(BOLD "  %-12s " CL_RESET ": %-15s " BOLD "Số nét" CL_RESET ": %s\n", 
           "Bộ thủ", k->radical ? k->radical : "N/A", k->stroke ? k->stroke : "N/A");
    printf(BOLD "  %-12s " CL_RESET ": %s\n", 
           "Giải nghĩa", k->description ? k->description : "N/A");
    
    printf(CL_PRIMARY "  ╟────────────────────────────────────────────────────────────────────╢\n" CL_RESET);

    printf(CL_WARN "  [Âm ON]  : " CL_RESET);
    if (k->onCount > 0) {
        for (int i = 0; i < k->onCount; i++) {
            printf(BOLD "%s" CL_RESET " (%s)%s", k->on[i].jp, k->on[i].romaji, (i == k->onCount - 1) ? "" : ", ");
        }
    } else printf("N/A");
    
    printf("\n" CL_WARN "  [Âm KUN] : " CL_RESET);
    if (k->kunCount > 0) {
        for (int i = 0; i < k->kunCount; i++) {
            printf(BOLD "%s" CL_RESET " (%s)%s", k->kun[i].jp, k->kun[i].romaji, (i == k->kunCount - 1) ? "" : ", ");
        }
    } else printf("N/A");
    
    printf("\n" CL_PRIMARY "  ╟────────────────────────────────────────────────────────────────────╢\n" CL_RESET);
    printf(BOLD "  TỪ VỰNG LIÊN QUAN (%d)\n" CL_RESET, k->vocabsCount);
    
    if (k->vocabsCount > 0 && k->vocabs != NULL) {
        for (int i = 0; i < k->vocabsCount; i++) {
            Vocab *v = &k->vocabs[i];
            printf("\n    " CL_SUCCESS "%d. %-10s " CL_RESET "[%s / %s]\n", 
                   i + 1, v->vocab, v->hiragana, v->romaji);
            printf("       └─ Nghĩa: %s\n", v->meaning);
            
            if (v->samplesCount > 0) {
                for (int j = 0; j < v->samplesCount; j++) {
                    printf("          " CL_WARN "↳ " CL_RESET "Ví dụ: %s\n", v->samples[j].jp);
                    printf("            Dịch : %s\n", v->samples[j].vn);
                }
            }
        }
    } else {
        printf("    (Không có dữ liệu từ vựng)\n");
    }

    printf(CL_PRIMARY "\n╚════════════════════════════════════════════════════════════════════╝\n" CL_RESET);
    printf(CL_DIM "  ▶ Nhấn Enter để quay lại..." CL_RESET);
    getchar();
}

void selectAndDisplayLesson(KanjiList *L) {
    int lessonChoice;
    int totalLessons = (L->kanjiCount + KANJI_PER_LESSON - 1) / KANJI_PER_LESSON;

    printf("\n" CL_TEXT "  Tra cứu Lesson: " CL_LOGO);
    scanf("%d", &lessonChoice);
    getchar();

    if (lessonChoice < 1 || lessonChoice > totalLessons) {
        printf(CL_ERROR "  ⚠ Bài học không hợp lệ.\n" RESET);
        return;
    }

    int startIndex = (lessonChoice - 1) * KANJI_PER_LESSON;
    int endIndex = startIndex + KANJI_PER_LESSON;
    if (endIndex > L->kanjiCount) endIndex = L->kanjiCount;

    while (1) {
        system("cls || clear");
        
        printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
        printf(CL_BORDER "  │" BG_HIGHLIGHT "                     BÀI %d : DANH SÁCH KANJI                    " RESET CL_BORDER "│\n" RESET, lessonChoice);
        printf(CL_BORDER "  ├───────┬─────────────────────────────┬───────────────────────────────┤\n" RESET);
        printf(CL_BORDER "  │ " CL_HEADER "STT" CL_BORDER "   │ " CL_HEADER "KANJI" CL_BORDER "                         │ " CL_HEADER "HÁN VIỆT" CL_BORDER "                     │\n" RESET);
        printf(CL_BORDER "  ├───────┼─────────────────────────────┼───────────────────────────────┤\n" RESET);

        for (int i = startIndex; i < endIndex; i++) {
            printf(CL_BORDER "  │ " CL_KEY "%-5d" CL_BORDER " │ " CL_LOGO "%-27s" CL_BORDER " │ " CL_TEXT "%-29s" CL_BORDER " │\n" RESET,
                   i + 1, L->kanjis[i].kanji, L->kanjis[i].hanViet);
        }
        
        printf(CL_BORDER "  └───────┴─────────────────────────────┴───────────────────────────────┘\n" RESET);

        int kanjiIdx;
        printf("\n" CL_TEXT "  Nhập số thứ tự Kanji để xem chi tiết " CL_DIM "(0 để thoát)" CL_TEXT ": " CL_LOGO);
        scanf("%d", &kanjiIdx);
        getchar();

        if (kanjiIdx == 0) break;
        
        if (kanjiIdx < startIndex + 1 || kanjiIdx > endIndex) {
            printf(CL_ERROR "  ⚠ Lựa chọn không hợp lệ. Vui lòng chọn lại.\n" RESET);
            continue;
        }
        displayDetailedKanji(&L->kanjis[kanjiIdx - 1]);
    }
}