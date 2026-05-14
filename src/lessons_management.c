#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
#endif
#include "lessons_management.h"
#include "../data_structures.h"

#define KANJI_PER_LESSON 16
#define CLR_PRIMARY "\x1b[38;5;75m"   // Xanh Blue
#define CLR_SUCCESS "\x1b[38;5;82m"   // Xanh lá
#define CLR_WARN    "\x1b[38;5;214m"  // Cam/Vàng
#define CLR_HEADER  "\x1b[38;5;225m"  // Hồng nhạt/Trắng sáng
#define CLR_RESET   "\x1b[0m"
#define BOLD        "\x1b[1m"
void displayAllLessons(KanjiList *L) {
    int totalLessons = L->kanjiCount / KANJI_PER_LESSON;

    int i, j;
    for (i = 0; i < totalLessons; i++) {
        int startIndex = i * KANJI_PER_LESSON;
        int endIndex = startIndex + KANJI_PER_LESSON - 1;
        if (endIndex >= L->kanjiCount) endIndex = L->kanjiCount - 1;
        printf("[Bài %-2d] : (%-3d - %-3d) : ", i + 1, startIndex + 1, endIndex + 1);
        // for (j = 0; j < KANJI_PER_LESSON/2; j++) {
        //     printf("[%d. %s - %s ]", j + startIndex + 1, L->kanjis[j + startIndex].kanji, L->kanjis[j + startIndex].hanViet);
        // }
        // printf("\n");
        // for (j = KANJI_PER_LESSON/2; j < KANJI_PER_LESSON; j++) {
        //     printf("[%d. %s - %s ]", j + startIndex + 1, L->kanjis[j + startIndex].kanji, L->kanjis[j + startIndex].hanViet);
        // }
        for (j = 0; j < KANJI_PER_LESSON; j++) {
            printf("%s   ", L->kanjis[j + startIndex].kanji);
        }
        printf("\n");
    }
}

void displayDetailedKanji(Kanji *k) {
    if (k == NULL) return;

    system("cls || clear");
    
    // --- Header ---
    printf(CLR_PRIMARY "╔══════════════════════════════════════════════════╗\n");
    printf("║ " CLR_HEADER BOLD "        THÔNG TIN CHI TIẾT KANJI: [ %-3s ]" CLR_PRIMARY "      ║\n", k->kanji ? k->kanji : "N/A");
    printf("╚══════════════════════════════════════════════════╝\n" CLR_RESET);

    // --- Thông tin cơ bản ---
    printf(BOLD "  %-12s " CLR_RESET ": %-15s " BOLD "STT" CLR_RESET ": %d\n", "Hán Việt", k->hanViet ? k->hanViet : "N/A", k->stt);
    printf(BOLD "  %-12s " CLR_RESET ": %-15s " BOLD "Số nét" CLR_RESET ": %s\n", "Bộ thủ", k->radical ? k->radical : "N/A", k->stroke ? k->stroke : "N/A");
    printf(BOLD "  %-12s " CLR_RESET ": %s\n", "Giải nghĩa", k->description ? k->description : "N/A");
    printf(CLR_PRIMARY "  ╟────────────────────────────────────────────────╢\n" CLR_RESET);

    // --- Âm On / Kun ---
    printf(CLR_WARN "  [Âm ON]  : " CLR_RESET);
    if (k->onCount > 0) {
        for (int i = 0; i < k->onCount; i++) {
            printf(BOLD "%s" CLR_RESET " (%s)%s", k->on[i].jp, k->on[i].romaji, (i == k->onCount - 1) ? "" : ", ");
        }
    } else printf("N/A");
    
    printf("\n" CLR_WARN "  [Âm KUN] : " CLR_RESET);
    if (k->kunCount > 0) {
        for (int i = 0; i < k->kunCount; i++) {
            printf(BOLD "%s" CLR_RESET " (%s)%s", k->kun[i].jp, k->kun[i].romaji, (i == k->kunCount - 1) ? "" : ", ");
        }
    } else printf("N/A");
    printf("\n" CLR_PRIMARY "  ╟────────────────────────────────────────────────╢\n" CLR_RESET);

    // --- Từ vựng liên quan ---
    printf(BOLD "  TỪ VỰNG LIÊN QUAN (%d)\n" CLR_RESET, k->vocabsCount);
    if (k->vocabsCount > 0 && k->vocabs != NULL) {
        for (int i = 0; i < k->vocabsCount; i++) {
            Vocab *v = &k->vocabs[i];
            printf("\n    " CLR_SUCCESS "%d. %-10s " CLR_RESET "[%s / %s]\n", i + 1, v->vocab, v->hiragana, v->romaji);
            printf("       └─ Nghĩa: %s\n", v->meaning);
            
            if (v->samplesCount > 0) {
                for (int j = 0; j < v->samplesCount; j++) {
                    printf("          " CLR_WARN "↳ " CLR_RESET "Ví dụ: %s\n", v->samples[j].jp);
                    printf("            Dịch : %s\n", v->samples[j].vn);
                }
            }
        }
    } else {
        printf("    (Không có dữ liệu từ vựng)\n");
    }

    printf(CLR_PRIMARY "\n╚══════════════════════════════════════════════════╝\n" CLR_RESET);
    printf(CLR_WARN " >> Nhấn Enter để quay lại..." CLR_RESET);
    
    getchar();
}

void selectAndDisplayLesson(KanjiList *L) {
    int lessonChoice;
    int totalLessons = (L->kanjiCount + KANJI_PER_LESSON - 1) / KANJI_PER_LESSON;

    printf("\nTra cứu Lesson: ");
    scanf("%d", &lessonChoice);
    getchar(); // Clear buffer

    if (lessonChoice < 1 || lessonChoice > totalLessons) {
        printf("Bài học không hợp lệ.\n");
        return;
    }

    int startIndex = (lessonChoice - 1) * KANJI_PER_LESSON;
    int endIndex = startIndex + KANJI_PER_LESSON;
    if (endIndex > L->kanjiCount) endIndex = L->kanjiCount;

    while (1) {
        system("cls || clear");
        printf("BÀI %d : DANH SÁCH KANJI\n", lessonChoice);
        printf("STT\t| Kanji\t| Hán Việt\n");

        for (int i = startIndex; i < endIndex; i++) {
            printf("%d\t| %s\t| %s\n", 
                   i + 1, 
                   L->kanjis[i].kanji, 
                   L->kanjis[i].hanViet);
        }

        int kanjiIdx;
        printf("\nNhập số thứ tự Kanji để xem chi tiết (0 to exit): ");
        scanf("%d", &kanjiIdx);
        getchar(); // Clear buffer

        if (kanjiIdx == 0) break;
        
        if (kanjiIdx < startIndex + 1 || kanjiIdx > endIndex) {
            printf("Lựa chọn không hợp lệ. Vui lòng chọn lại.\n");
            continue;
        }
        displayDetailedKanji(&L->kanjis[kanjiIdx - 1]);
    }
}