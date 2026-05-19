// ==================== src/dashboard.c ====================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
#endif
#include "kanji_search_exact.h"
#include "vocab_search_exact.h"
#include "fuzzy_search.h"
#include "substring_search.h"
#include "multiple_choice.h"
#include "lessons_management.h"
#include "filter_learned_lesson.h"
#include "sentence_analysis.h"
#include "parse_json_to_struct.h"
#include "prefix_search.h"
#include "utils.h"
#include "../data_structures.h"
#include "dashboard.h"


void displayMenu(void) {
    printf(CL_LOGO BOLD);
    printf("    _  _              _ _   __ _      \n");
    printf("   | |/ /            (|) | _ \\(_)     \n");
    printf("   | ' <  _ _ _ _  _ _   |  _/ | |     \n");
    printf("   | |\\_\\/ ` | ' \\| | |  |_|   |_|     \n");
    printf("   |_| \\_\\__,_|_| |_| |_|" RESET "  Dictionary " CL_LOGO "v3.0\n" RESET);
    printf(CL_LOGO "                   |__/                  \n\n" RESET);

    printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
    printf(CL_BORDER "  │" BG_HIGHLIGHT " DASHBOARD " RESET "  History    Favorites    Settings    Help       " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  ├──────┬─────────────────────────────────────────────────────┤\n" RESET);

    printf(CL_BORDER "  │ " RESET BOLD "MENU" CL_BORDER " │" RESET "                                                     " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "01" CL_BORDER "  │" RESET " 🔍 Tìm kiếm (Chính xác / KMP / Tìm mờ / Prefix)              " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "02" CL_BORDER "  │" RESET " 📁 Quản lý từ điển cá nhân                          " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "03" CL_BORDER "  │" RESET " 📑 Lọc từ vựng đã học                               " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "04" CL_BORDER "  │" RESET " 🧠 Phân tích câu (Parser)                           " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "05" CL_BORDER "  │" RESET " 📝 Bài tập hàng ngày                                " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  ├──────┴─────────────────────────────────────────────────────┤\n" RESET);
    printf(CL_BORDER "  │" RESET "      " BOLD CL_ERROR "[0] THOÁT CHƯƠNG TRÌNH" RESET "                                " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  └────────────────────────────────────────────────────────────┘\n" RESET);

    printf("  " BG_MENU CL_BORDER " Shortcut: " RESET " " CL_KEY "1-5: Chọn" RESET " | " CL_KEY "0: Thoát" RESET "\n");
    printf("\n  " BOLD "Command" RESET " » " CL_LOGO);
}

void menuExactSearch(HashTable *vocabHT, HashTableK *kanjiHT) {
    int choice;
    char keyword[256];
    while (1) {
        printf("\x1b[2J\x1b[H");
        printf("\n  " CL_BORDER "Dashboard > " RESET BOLD "EXACT SEARCH" RESET "\n");
        printf(CL_BORDER "  ────────────────────────────────────────────────────────────\n" RESET);
        printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
        printf(CL_BORDER "  │" RESET "  " BOLD "VUI LÒNG CHỌN PHẠM VI TÌM KIẾM" RESET "                            " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  ├──────┬─────────────────────────────────────────────────────┤\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "01" CL_BORDER "  │" RESET " ⛩️   Tra cứu Kanji (Mặt chữ / Hán Việt)              " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "02" CL_BORDER "  │" RESET " 📖  Tra cứu Từ vựng (Nhật/Hira/Romaji/Việt)         " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "00" CL_BORDER "  │" RESET " ↩️   Quay lại Dashboard                              " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  └──────┴─────────────────────────────────────────────────────┘\n" RESET);
        printf("\n  " BOLD "Lựa chọn của bạn" RESET " » " CL_KEY);
        
        if (scanf("%d", &choice) != 1) { while(getchar() != '\n'); continue; }
        getchar();
        printf(RESET);
        if (choice == 0) break;
        
        printf("\n  " BG_HIGHLIGHT " SEARCH BAR " RESET "\n");
        printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
        printf(CL_BORDER "  │ " RESET "Nhập từ khóa chính xác " BOLD "🔍" RESET ": " CL_LOGO);
        inputString(keyword, 256);
        printf(RESET CL_BORDER "  └────────────────────────────────────────────────────────────┘\n" RESET);
        printf("  " CL_TEXT "Đang truy xuất dữ liệu... " RESET "\n");
        
        printf("\x1b[2J\x1b[H"); 
        printf("\n  " CL_BORDER "Dashboard > Exact Search > " RESET BOLD "RESULTS" RESET "\n");
        printf(CL_BORDER "  ────────────────────────────────────────────────────────────\n" RESET);
        printf("  " BOLD "Kết quả cho: " RESET "\"" CL_LOGO "%s" RESET "\"\n", keyword);
        printf(CL_BORDER "  ────────────────────────────────────────────────────────────\n\n" RESET);
        
        if (choice == 1) {
            exactlySearchingKanji(kanjiHT, keyword);
        } else if (choice == 2) {
            exactlySearching(vocabHT, keyword);
        } else {
            printf("  " CL_LOGO " [!] " RESET "Lựa chọn không hợp lệ.\n");
        }
        
        printf("\n  " CL_BORDER "────────────────────────────────────────────────────────────" RESET);
        printf("\n  " CL_KEY "Nhấn Enter để tiếp tục..." RESET);
        getchar();
    }
}

void menuKMPSearch(KanjiList *L) {
    int option;
    char keyword[256];
    while (1) {
        printf("\x1b[2J\x1b[H");
        printf("\n  " CL_BORDER "Dashboard > Search > " RESET BOLD "PATTERN MATCHING (KMP)" RESET "\n");
        printf(CL_BORDER "  ────────────────────────────────────────────────────────────\n" RESET);
        printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
        printf(CL_BORDER "  │" RESET "  " BOLD "CHẾ ĐỘ KHỚP MẪU THÔNG MINH (KMP)" RESET "                         " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  ├──────┬─────────────────────────────────────────────────────┤\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "01" CL_BORDER "  │" RESET " ⛩️   Khớp theo mặt chữ Kanji                         " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "02" CL_BORDER "  │" RESET " 🎋  Khớp theo cách đọc Hiragana                     " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "03" CL_BORDER "  │" RESET " 🔡  Khớp theo phiên âm Romaji                       " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "04" CL_BORDER "  │" RESET " 🇻🇳  Khớp theo nghĩa Tiếng Việt                      " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "00" CL_BORDER "  │" RESET " ↩️   Quay lại                                        " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  └──────┴─────────────────────────────────────────────────────┘\n" RESET);
        printf("\n  " BOLD "Chọn chế độ [0-4]" RESET " » " CL_KEY);
        
        if (scanf("%d", &option) != 1) { while(getchar() != '\n'); continue; }
        getchar();
        printf(RESET);
        if (option == 0) break;
        if (option < 1 || option > 4) continue;
        
        printf("\n  " BG_HIGHLIGHT " KMP INPUT " RESET "\n");
        printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
        printf(CL_BORDER "  │ " RESET "Nhập chuỗi cần khớp " BOLD "🧩" RESET ": " CL_LOGO);
        inputString(keyword, 256);
        printf(RESET CL_BORDER "  └────────────────────────────────────────────────────────────┘\n" RESET);
        
        printf("\x1b[2J\x1b[H"); 
        printf("\n  " CL_BORDER "Dashboard > KMP Search > " RESET BOLD "RESULTS" RESET "\n");
        printf(CL_BORDER "  ────────────────────────────────────────────────────────────\n" RESET);
        printf("  " BOLD "Kết quả cho mẫu: " RESET "\"" CL_LOGO "%s" RESET "\"\n", keyword);
        printf(CL_BORDER "  ────────────────────────────────────────────────────────────\n\n" RESET);
        
        substringSearching(L, keyword, option);
        
        printf("\n  " CL_BORDER "────────────────────────────────────────────────────────────" RESET);
        printf("\n  " CL_KEY "Nhấn Enter để tiếp tục..." RESET);
        getchar();
    }
}

void menuFuzzySearch(KanjiList *L) {
    int option;
    char keyword[256];
    while (1) {
        printf("\x1b[2J\x1b[H");
        printf("\n  " CL_BORDER "Dashboard > Search > " RESET BOLD "FUZZY SEARCH (TÌM MỜ)" RESET "\n");
        printf(CL_BORDER "  ────────────────────────────────────────────────────────────\n" RESET);
        printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
        printf(CL_BORDER "  │" RESET "  " BOLD "CHẾ ĐỘ TÌM KIẾM GẦN ĐÚNG (FUZZY)" RESET "                         " CL_BORDER " │\n" RESET);
        printf(CL_BORDER "  ├──────┬─────────────────────────────────────────────────────┤\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "01" CL_BORDER "  │" RESET " 🌫️   Khớp mờ mặt chữ Kanji                           " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "02" CL_BORDER "  │" RESET " 🍃  Khớp mờ cách đọc Hiragana                       " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "03" CL_BORDER "  │" RESET " 🔤  Khớp mờ phiên âm Romaji                         " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "04" CL_BORDER "  │" RESET " 💬  Khớp mờ nghĩa Tiếng Việt                        " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "00" CL_BORDER "  │" RESET " ↩️   Quay lại                                        " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  └──────┴─────────────────────────────────────────────────────┘\n" RESET);
        printf("\n  " BOLD "Chọn chế độ [0-4]" RESET " » " CL_KEY);
        
        if (scanf("%d", &option) != 1) { while(getchar() != '\n'); continue; }
        getchar();
        printf(RESET);
        if (option == 0) break;
        if (option < 1 || option > 4) continue;
        
        printf("\n  " BG_HIGHLIGHT " FUZZY INPUT " RESET "\n");
        printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
        printf(CL_BORDER "  │ " RESET "Nhập từ khóa (sai dấu/ký tự) " BOLD "☁️" RESET ": " CL_LOGO);
        inputString(keyword, 256);
        printf(RESET CL_BORDER "  └────────────────────────────────────────────────────────────┘\n" RESET);
        printf("  " CL_TEXT "Đang tính toán khoảng cách chỉnh sửa (Levenshtein)..." RESET "\n");
        
        printf("\x1b[2J\x1b[H"); 
        printf("\n  " CL_BORDER "Dashboard > Fuzzy Search > " RESET BOLD "RESULTS" RESET "\n");
        printf(CL_BORDER "  ────────────────────────────────────────────────────────────\n" RESET);
        printf("  " BOLD "Kết quả tìm mờ cho: " RESET "\"" CL_LOGO "%s" RESET "\"\n", keyword);
        printf(CL_BORDER "  ────────────────────────────────────────────────────────────\n\n" RESET);
        
        fuzzySearching(L, keyword, option);
        
        printf("\n  " CL_BORDER "────────────────────────────────────────────────────────────" RESET);
        printf("\n  " CL_KEY "Nhấn Enter để thực hiện lượt tìm kiếm mới..." RESET);
        getchar();
    }
}

void menuPrefixSearch(TrieNode *trieRoot) {
    char keyword[256];
    int choice;
    while (1) {
        printf("\x1b[2J\x1b[H");
        printf("\n  " CL_BORDER "Dashboard > Tra cứu > " RESET BOLD "PREFIX SEARCH (TÌM THEO TIỀN TỐ)" RESET "\n");
        printf(CL_BORDER "  ────────────────────────────────────────────────────────────\n" RESET);
        
        printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
        printf(CL_BORDER "  │" RESET "  " BOLD "VUI LÒNG CHỌN CHẾ ĐỘ" RESET "                                      " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  ├──────┬─────────────────────────────────────────────────────┤\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "01" CL_BORDER "  │" RESET " 🔍 Tìm kiếm theo tiền tố                            " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "00" CL_BORDER "  │" RESET " ↩️   Quay lại                                        " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  └──────┴─────────────────────────────────────────────────────┘\n" RESET);
        
        printf("\n  " BOLD "Lựa chọn của bạn" RESET " » " CL_KEY);
        
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n');
            continue;
        }
        getchar();
        printf(RESET);
        
        if (choice == 0) break;
        if (choice != 1) {
            printf("  " CL_WARN "(!) Lựa chọn không hợp lệ. Vui lòng chọn lại.\n" RESET);
            printf("\n  " CL_KEY "Nhấn Enter để tiếp tục..." RESET);
            getchar();
            continue;
        }
        
        printf("\n  " BG_HIGHLIGHT " PREFIX INPUT " RESET "\n");
        printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
        printf(CL_BORDER "  │ " RESET "Nhập chuỗi ký tự bắt đầu (romaji only) " BOLD "⚡" RESET ": " CL_LOGO);
        inputString(keyword, 256);
        printf(RESET CL_BORDER "  └────────────────────────────────────────────────────────────┘\n" RESET);
        printf("  " CL_TEXT "Cây Trie đang xử lý luồng dữ liệu..." RESET "\n");
        
        printf("\x1b[2J\x1b[H"); 
        printf("\n  " CL_BORDER "Dashboard > Prefix Search > " RESET BOLD "RESULTS" RESET "\n");
        printf(CL_BORDER "  ────────────────────────────────────────────────────────────\n" RESET);
        printf("  " BOLD "Danh sách từ vựng bắt đầu bằng: " RESET "\"" CL_LOGO "%s" RESET "\"\n", keyword);
        printf(CL_BORDER "  ────────────────────────────────────────────────────────────\n\n" RESET);
        
        TrieNode *matchNode = searchPrefixNode(trieRoot, keyword);
        if (!matchNode) {
            printf("  " CL_KEY "[!]" RESET " Không tồn tại từ vựng nào bắt đầu bằng từ khóa: \"" BG_HIGHLIGHT "%s" RESET "\"\n", keyword);
        } else {
            int resultCounter = 0;
            printAllWordsFromNode(matchNode, &resultCounter);
            if(resultCounter == 0) {
                printf("  " CL_KEY "[!]" RESET " Không có kết quả nào phù hợp hoàn toàn.\n");
            } else {
                printf("\n  " BG_HIGHLIGHT " THÀNH CÔNG " RESET " Tìm thấy tổng cộng " BOLD "%d" RESET " kết quả phù hợp.\n", resultCounter);
            }
        }
        
        printf("\n  " CL_BORDER "────────────────────────────────────────────────────────────" RESET);
        printf("\n  " CL_KEY "Nhấn Enter để thực hiện lượt tìm kiếm mới..." RESET);
        getchar();
    }
}

void caseNo1(KanjiList *L, HashTable *vHT, HashTableK *kHT, TrieNode *trieRoot) {
    int choice;
    while(1) {
        printf("\x1b[2J\x1b[H"); 
        printf("\n  " CL_BORDER "Dashboard > " RESET BOLD "HỆ THỐNG TÌM KIẾM" RESET "\n");
        printf(CL_BORDER "  ────────────────────────────────────────────────────────────\n" RESET);
        
        printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
        printf(CL_BORDER "  │" RESET "  " BOLD "CHỌN PHƯƠNG THỨC TRA CỨU" RESET "                                 " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  ├──────┬─────────────────────────────────────────────────────┤\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "01" CL_BORDER "  │" RESET " 🎯  Exact Match (Tìm chính xác 100%%)                " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "02" CL_BORDER "  │" RESET " 🔍  Pattern Match (KMP - Tìm chuỗi con)              " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "03" CL_BORDER "  │" RESET " 🌫️  Fuzzy Match (Tìm gần đúng Levenshtein)          " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "04" CL_BORDER "  │" RESET " ⚡  Prefix Search (Gợi ý tiền tố cây Trie)          " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "00" CL_BORDER "  │" RESET " ↩️  Quay lại Menu chính                              " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  └──────┴─────────────────────────────────────────────────────┘\n" RESET);
        
        printf("\n  " BOLD "Lựa chọn của bạn" RESET " » " CL_KEY);
        if (scanf("%d", &choice) != 1) { while(getchar() != '\n'); continue; }
        getchar();
        printf(RESET);
        
        if (choice == 0) break;
        switch (choice) {
            case 1: menuExactSearch(vHT, kHT); break;
            case 2: menuKMPSearch(L); break;
            case 3: menuFuzzySearch(L); break;
            case 4: menuPrefixSearch(trieRoot); break;
            default:
                printf("\n  " CL_ERROR "(!) Lựa chọn không hợp lệ. Vui lòng chọn lại." RESET);
                break;
        }
        printf("\n  " CL_BORDER "────────────────────────────────────────────────────────────" RESET);
        printf("\n  " CL_KEY "Nhấn Enter để quay lại danh sách tìm kiếm..." RESET);
        getchar();
    }
}

void caseNo2(KanjiList *L) {
    system("cls");
    displayAllLessons(L);
    selectAndDisplayLesson(L);
}

void caseNo3(KanjiList *L) {
    runFilterLearnedVocab(L);
}

void caseNo5(KanjiList *L) {
    runChoiceOption(L);
}

void caseNo4(KanjiList *L) {
    analyzeJapaneseSentence(L);
}


void handleMenuSelection(char *rawJson) {
    int choice;
    KanjiList myData = parseJsonToStruct(rawJson);
    HashTable *vocabHT = createHashTable(HASH_TABLE_SIZE);
    HashTableK *kanjiHT = createHashTableK(HASH_TABLE_SIZE);
    TrieNode *trieRoot = buildTrieFromKanjiList(&myData);

    buildHashTableForVocab(&myData, vocabHT);
    buildHashTableForKanji(&myData, kanjiHT);

    while (1) {
        system("cls");
        displayMenu();
        if (scanf("%d", &choice) != 1) {
            printf("Vui long nhap so hop le.\n");
            while (getchar() != '\n');
            continue;
        }
        getchar();

        switch (choice) {
            case 1: caseNo1(&myData, vocabHT, kanjiHT, trieRoot); break;
            case 2: caseNo2(&myData); break;
            case 3: caseNo3(&myData); break;
            case 4: caseNo4(&myData); break;
            case 5: caseNo5(&myData); break;
            case 0:
                printf("Dang thoat chuong trinh...\n");
                freeTrie(trieRoot);
                return;
            default:
                printf("Lua chon khong hop le. Vui long chon lai.\n");
                getchar();
        }
    }
}