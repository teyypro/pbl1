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

    printf("\n  " BG_HIGHLIGHT BOLD " DANH SÁCH BÀI HỌC TỔNG HỢP " RESET"\n");


    for (i = 0; i < totalLessons; i++) {
        int startIndex = i * KANJI_PER_LESSON;
        int endIndex = startIndex + KANJI_PER_LESSON - 1;
        if (endIndex >= L->kanjiCount) endIndex = L->kanjiCount - 1;
        
        printf("  " CL_PRIMARY "┃ " CL_DIM "Bài" CL_PRIMARY " %02d " CL_DIM "(%3d - %3d) : " RESET, i + 1, startIndex + 1, endIndex + 1);
        
        for (j = 0; j < KANJI_PER_LESSON; j++) {
            // Sử dụng CL_KANJI để đồng bộ nhận diện ký tự Hán tự hệ thống
            printf(CL_KANJI BOLD "%s" RESET "  ", L->kanjis[j + startIndex].kanji);
        }
        printf("\n");
    }
    
}

void displayDetailedKanji(Kanji *k) {
    if (k == NULL) return;

    system("cls || clear");
    
    printf("\n  " BG_HIGHLIGHT BOLD " THÔNG TIN CHI TIẾT KANJI " RESET"\n\n");

    printf("  " CL_PRIMARY "┃ " CL_DIM "Kanji:" RESET " " CL_KANJI BOLD "%s" RESET "\n", k->kanji ? k->kanji : "N/A");

    // Điểm nhấn thị giác chính: Chữ Hán Việt đậm màu CL_MEANING đặt cạnh STT để định vị cấu trúc bài học
    printf("  " CL_PRIMARY "┃ " CL_DIM "Hán Việt   :" RESET " " CL_MEANING BOLD "%-15s" RESET " " CL_DIM "STT: %d\n" RESET, k->hanViet ? k->hanViet : "N/A", k->stt);
    printf("  " CL_PRIMARY "┃ " CL_DIM "Bộ thủ     :" RESET " %-15s " CL_DIM "Số nét: %s\n" RESET, k->radical ? k->radical : "N/A", k->stroke ? k->stroke : "N/A");
    printf("  " CL_PRIMARY "┃ " CL_DIM "Giải nghĩa :" RESET " %s\n", k->description ? k->description : "N/A");
    
    printf("  " CL_PRIMARY "┃\n");

    // Khối âm đọc: Phân rã cấu trúc bằng màu sắc giúp não bộ dễ phân biệt JP và Romaji mà không bị rối mắt
    printf("  " CL_PRIMARY "┃ " CL_DIM "[Âm ON]  :" RESET " ");
    if (k->onCount > 0) {
        for (int i = 0; i < k->onCount; i++) {
            printf(CL_KANJI BOLD "%s" RESET " " CL_KANA "(%s)" RESET "%s", k->on[i].jp, k->on[i].romaji, (i == k->onCount - 1) ? "" : ", ");
        }
    } else printf("N/A");
    
    printf("\n  " CL_PRIMARY "┃ " CL_DIM "[Âm KUN] :" RESET " ");
    if (k->kunCount > 0) {
        for (int i = 0; i < k->kunCount; i++) {
            printf(CL_KANJI BOLD "%s" RESET " " CL_KANA "(%s)" RESET "%s", k->kun[i].jp, k->kun[i].romaji, (i == k->kunCount - 1) ? "" : ", ");
        }
    } else printf("N/A");
    
    printf("\n  " CL_PRIMARY "┃\n");
    printf("  " CL_PRIMARY "┃ " CL_DIM "Từ vựng liên quan (%d):\n" RESET, k->vocabsCount);
    
    if (k->vocabsCount > 0 && k->vocabs != NULL) {
        for (int i = 0; i < k->vocabsCount; i++) {
            Vocab *v = &k->vocabs[i];
            // Header vocab hiện đại
            printf(
                "  " CL_PRIMARY "┃ " RESET
                CL_DIM "%02d." RESET " "
                CL_KANJI BOLD "%s" RESET
                "  " CL_DIM "(" RESET
                CL_KANA "%s" RESET
                CL_DIM " • " RESET
                CL_ROMAJI "%s" RESET
                CL_DIM ")" RESET
                "  " CL_MEANING "%s\n",
                i + 1,
                v->vocab,
                v->hiragana ? v->hiragana : "-",
                v->romaji ? v->romaji : "-",
                v->meaning ? v->meaning : "-"
            );

            // Examples
            if (v->samplesCount > 0) {
                for (int j = 0; j < v->samplesCount; j++) {

                    printf(
                        "  " CL_PRIMARY "┃    " RESET
                        CL_DIM "┆ " RESET
                        CL_JP_SENT "%s\n" RESET,
                        v->samples[j].jp
                    );

                    printf(
                        "  " CL_PRIMARY "┃    " RESET
                        CL_DIM "└─ " RESET
                        CL_VN_SENT "%s\n" RESET,
                        v->samples[j].vn
                    );
                }
            }

            // spacing giữa các vocab
            printf("  " CL_PRIMARY "┃\n" RESET);
        }

    } else {

        printf(
            "  " CL_PRIMARY "│ " RESET
            CL_DIM "Không có dữ liệu từ vựng liên quan.\n" RESET
        );
    }


    printf("  " CL_DIM ">> Nhấn Enter để quay lại..." RESET);
    getchar();
}

void selectAndDisplayLesson(KanjiList *L) {
    int lessonChoice;
    int totalLessons = (L->kanjiCount + KANJI_PER_LESSON - 1) / KANJI_PER_LESSON;

    printf("\n  " CL_DIM ">> Bạn cần xem bài số:" RESET " " CL_LOGO BOLD);
    scanf("%d", &lessonChoice);
    printf(RESET);
    getchar();

    if (lessonChoice < 1 || lessonChoice > totalLessons) {
        printf("  " CL_ERROR "🚨 [LỖI] Chỉ số bài học không tồn tại.\n" RESET);
        return;
    }

    int startIndex = (lessonChoice - 1) * KANJI_PER_LESSON;
    int endIndex = startIndex + KANJI_PER_LESSON;
    if (endIndex > L->kanjiCount) endIndex = L->kanjiCount;

    while (1) {
        system("cls || clear");
        
        printf("\n  " BG_HIGHLIGHT BOLD " BÀI %02d : DANH SÁCH KANJI " RESET "\n", lessonChoice);
    

        for (int i = startIndex; i < endIndex; i++) {
            // Sắp đặt cột đối xứng: Cột chữ gốc (Hồng Neon) và Cột nghĩa chuyển ngữ (Tím) phân cách bằng vạch dọc mờ
            printf("  " CL_PRIMARY "┃ " CL_DIM "%02d  │   " RESET CL_KANJI BOLD "%-6s" RESET CL_DIM "│" RESET "  " CL_MEANING "%-30s" RESET "\n",
                   i + 1, L->kanjis[i].kanji, L->kanjis[i].hanViet);
        }
        

        int kanjiIdx;
        printf(CL_DIM ">> Nhập số thứ tự để xem chi tiết Kanji (0 để thoát):" RESET " " CL_LOGO BOLD);
        scanf("%d", &kanjiIdx);
        printf(RESET);
        getchar();

        if (kanjiIdx == 0) break;
        
        if (kanjiIdx < startIndex + 1 || kanjiIdx > endIndex) {
            printf("  " CL_ERROR "🚨 [LỖI] Lựa chọn không hợp lệ. Vui lòng nhập lại.\n" RESET);
            printf("  " CL_DIM "Nhấn Enter để tiếp tục..." RESET);
            getchar();
            continue;
        }
        displayDetailedKanji(&L->kanjis[kanjiIdx - 1]);
    }
}