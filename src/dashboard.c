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
#include "radical_graph.h"
// --- Định nghĩa Bảng màu (Đã được phối lại hài hòa hơn)
#define CL_LOGO      "\x1b[38;5;208m" // Màu cam đậm (Thương hiệu)
#define CL_BORDER    "\x1b[38;5;239m" // Màu xám tối (Giảm chói, tôn nội dung)
#define CL_TEXT      "\x1b[38;5;253m" // Trắng xám
#define CL_KEY       "\x1b[38;5;111m" // Xanh lơ dịu (Soft Blue)
#define CL_HIGHLIGHT "\x1b[48;5;236m\x1b[38;5;208m" // Nền xám đậm, chữ cam (Đồng bộ logo)
#define RESET        "\x1b[0m"
#define BOLD         "\x1b[1m"
void pauseAndClear() {
    printf("\n\n  " CL_BORDER "────────────────────────────────────────────────────────────" RESET);
    printf("\n  " BOLD "  [!] " RESET "Hệ thống đã sẵn sàng. Nhấn " CL_HIGHLIGHT " Enter " RESET " để tiếp tục...");
    getchar();
    printf("\x1b[2J\x1b[H"); // Dùng ANSI clear screen mượt hơn system("cls")
}
void displayMenu() {
    // 1. ASCII ART LOGO - Căn chỉnh lại padding cho cân đối
    printf(CL_LOGO BOLD);
    printf("    _  _              _ _   __ _      \n");
    printf("   | |/ /            (|) | _ \\(_)     \n");
    printf("   | ' <  _ _ _ _  _ _   |  _/ | |     \n");
    printf("   | |\\_\\/ ` | ' \\| | |  |_|   |_|     \n");
    printf("   |_| \\_\\__,_|_| |_| |_|" RESET "  Dictionary " CL_LOGO "v3.0\n" RESET);
    printf(CL_LOGO "                   |__/                  \n\n" RESET);
#define BG_MENU "\x1b[48;5;236m"
    // 2. MAIN INTERFACE (Khung được fix cứng 60 cột bên trong)
    printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
    
    // Header giả lập tab đang chọn
    printf(CL_BORDER "  │" BG_MENU CL_TEXT "" RESET CL_HIGHLIGHT " DASHBOARD " RESET CL_TEXT "  History    Favorites    Settings    Help       " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  ├──────┬─────────────────────────────────────────────────────┤\n" RESET);

    // Nội dung Menu kèm Icon (Padding đã được tính toán chính xác để thẳng hàng)
    printf(CL_BORDER "  │ " RESET BOLD "MENU" CL_BORDER " │" RESET "                                                     " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "01" CL_BORDER "  │" RESET " 🔍 Tìm kiếm (Chính xác / KMP / Tìm mờ)              " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "02" CL_BORDER "  │" RESET " 📁 Quản lý từ điển cá nhân                          " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "03" CL_BORDER "  │" RESET " 📑 Lọc từ vựng đã học                               " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "04" CL_BORDER "  │" RESET " 🧠 Phân tích câu (Parser)                           " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "05" CL_BORDER "  │" RESET " 📝 Bài tập hàng ngày                                " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  │  " CL_KEY "06" CL_BORDER "  │" RESET " ⛩️  Kanji họ hàng (Bộ thủ)                           " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  ├──────┴─────────────────────────────────────────────────────┤\n" RESET);
    
    // Nút thoát nổi bật
    printf(CL_BORDER "  │" RESET "      " BOLD "\x1b[31m[0] THOÁT CHƯƠNG TRÌNH\x1b[0m" RESET "                                " CL_BORDER "│\n" RESET);
    printf(CL_BORDER "  └────────────────────────────────────────────────────────────┘\n" RESET);

    // 3. FOOTER - Thanh trạng thái
    printf("  " "\x1b[48;5;236m" CL_BORDER " Shortcut: " RESET " " CL_KEY "1-6: Chọn" RESET " | " CL_KEY "0: Thoát" RESET"\n");
    
    // Prompt nhập liệu
    printf("\n  " BOLD "Command" RESET " » " CL_LOGO);
}
// --- MENU CẤP 2: TÌM KIẾM CHÍNH XÁC (EXACT SEARCH) ---
void menuExactSearch(HashTable *vocabHT, HashTableK *kanjiHT) {
    int choice;
    char keyword[256];
    while (1) {
        printf("\x1b[2J\x1b[H"); // Clear screen mượt mà
        // 1. HEADER - BREADCRUMB (Chỉ hướng)
        printf("\n  " CL_BORDER "Dashboard > " RESET BOLD "EXACT SEARCH" RESET "\n");
        printf(CL_BORDER "  " "────────────────────────────────────────────────────────────" "\n" RESET);
        // 2. GIAO DIỆN CHỌN CHẾ ĐỘ
        printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
        printf(CL_BORDER "  │" RESET "  " BOLD "VUI LÒNG CHỌN PHẠM VI TÌM KIẾM" RESET "                            " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  ├──────┬─────────────────────────────────────────────────────┤\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "01" CL_BORDER "  │" RESET " ⛩️   Tra cứu Kanji (Mặt chữ / Hán Việt)              " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "02" CL_BORDER "  │" RESET " 📖  Tra cứu Từ vựng (Nhật/Hira/Romaji/Việt)         " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "00" CL_BORDER "  │" RESET " ↩️   Quay lại Dashboard                              " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  └──────┴─────────────────────────────────────────────────────┘\n" RESET);
        printf("\n  " BOLD "Lựa chọn của bạn" RESET " » " CL_KEY);
        if (scanf("%d", &choice) != 1) { while(getchar() != '\n'); continue; }
        getchar(); // Clear buffer
        printf(RESET);
        if (choice == 0) break;
        // 3. GIAO DIỆN NHẬP TỪ KHÓA (Thiết kế thanh Input)
        printf("\n  " CL_HIGHLIGHT " SEARCH BAR " RESET "                                             \n");
        printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
        printf(CL_BORDER "  │ " RESET "Nhập từ khóa chính xác " BOLD "🔍" RESET ": " CL_LOGO);
        // Giả sử inputString là hàm bạn đã viết để lấy chuỗi
        fgets(keyword, sizeof(keyword), stdin);
        keyword[strcspn(keyword, "\n")] = 0; // Xóa ký tự xuống dòng
        printf(RESET CL_BORDER "  └────────────────────────────────────────────────────────────┘\n" RESET);
        // Hiệu ứng giả lập đang tìm kiếm
        printf("  " CL_TEXT "Đang truy xuất dữ liệu... " RESET "\n");
        // 4. HIỂN THỊ KẾT QUẢ
        printf("\x1b[2J\x1b[H"); 
        printf("\n  " CL_BORDER "Dashboard > Exact Search > " RESET BOLD "RESULTS" RESET "\n");
        printf(CL_BORDER "  " "────────────────────────────────────────────────────────────" "\n" RESET);
        printf("  " BOLD "Kết quả cho: " RESET "\"" CL_LOGO "%s" RESET "\"\n", keyword);
        printf(CL_BORDER "  " "────────────────────────────────────────────────────────────" "\n\n" RESET);
        // Khối hiển thị nội dung kết quả
        if (choice == 1) {
            exactlySearchingKanji(kanjiHT, keyword);
        } else if (choice == 2) {
            exactlySearching(vocabHT, keyword);
        } else {
            printf("  " CL_LOGO " [!] " RESET "Lựa chọn không hợp lệ.\n");
        }
        // Phần chân kết quả
        printf("\n  " CL_BORDER "────────────────────────────────────────────────────────────" RESET);
        printf("\n  " CL_KEY "Nhấn Enter để tiếp tục tìm kiếm hoặc quay lại..." RESET);
        getchar();
    }
}
void menuKMPSearch(KanjiList *L) {
    int option;
    char keyword[256];
    while (1) {
        printf("\x1b[2J\x1b[H"); // Clear screen mượt mà hơn system("cls")
        // 1. HEADER & BREADCRUMB
        printf("\n  " CL_BORDER "Dashboard > Search > " RESET BOLD "PATTERN MATCHING (KMP)" RESET "\n");
        printf(CL_BORDER "  " "────────────────────────────────────────────────────────────" "\n" RESET);
        // 2. OPTIONS BOX (Căn chỉnh chính xác 60 cột nội dung)
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
        getchar(); // Clear buffer
        printf(RESET);
        if (option == 0) break;
        if (option < 1 || option > 4) continue;
        // 3. SEARCH BAR (Thiết kế đồng bộ với Exact Search)
        printf("\n  " CL_HIGHLIGHT " KMP INPUT " RESET "\n");
        printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
        printf(CL_BORDER "  │ " RESET "Nhập chuỗi cần khớp " BOLD "🧩" RESET ": " CL_LOGO);
        // Sử dụng hàm input của bạn
        inputString(keyword, 256);
        printf(RESET CL_BORDER "  └────────────────────────────────────────────────────────────┘\n" RESET);
        // 4. RESULTS PAGE
        printf("\x1b[2J\x1b[H"); 
        printf("\n  " CL_BORDER "Dashboard > KMP Search > " RESET BOLD "RESULTS" RESET "\n");
        printf(CL_BORDER "  " "────────────────────────────────────────────────────────────" "\n" RESET);
        printf("  " BOLD "Kết quả cho mẫu: " RESET "\"" CL_LOGO "%s" RESET "\"\n", keyword);
        printf(CL_BORDER "  " "────────────────────────────────────────────────────────────" "\n\n" RESET);
        // Gọi logic tìm kiếm của bạn
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
        printf("\x1b[2J\x1b[H"); // Clear screen mượt mà
        // 1. HEADER & BREADCRUMB
        printf("\n  " CL_BORDER "Dashboard > Search > " RESET BOLD "FUZZY SEARCH (TÌM MỜ)" RESET "\n");
        printf(CL_BORDER "  " "────────────────────────────────────────────────────────────" "\n" RESET);
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
        getchar(); // Clear buffer
        printf(RESET);
        if (option == 0) break;
        if (option < 1 || option > 4) continue;
        // 3. FUZZY SEARCH BAR
        printf("\n  " CL_HIGHLIGHT " FUZZY INPUT " RESET "\n");
        printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
        printf(CL_BORDER "  │ " RESET "Nhập từ khóa (sai dấu/ký tự) " BOLD "☁️" RESET ": " CL_LOGO);
        inputString(keyword, 256);
        printf(RESET CL_BORDER "  └────────────────────────────────────────────────────────────┘\n" RESET);
        // Hiển thị trạng thái tính toán
        printf("  " CL_TEXT "Đang tính toán khoảng cách chỉnh sửa (Levenshtein)..." RESET "\n");
        // 4. RESULTS PAGE
        printf("\x1b[2J\x1b[H"); 
        printf("\n  " CL_BORDER "Dashboard > Fuzzy Search > " RESET BOLD "RESULTS" RESET "\n");
        printf(CL_BORDER "  " "────────────────────────────────────────────────────────────" "\n" RESET);
        printf("  " BOLD "Kết quả tìm mờ cho: " RESET "\"" CL_LOGO "%s" RESET "\"\n", keyword);
        printf(CL_BORDER "  " "────────────────────────────────────────────────────────────" "\n\n" RESET);
        fuzzySearching(L, keyword, option); 
        printf("\n  " CL_BORDER "────────────────────────────────────────────────────────────" RESET);
        printf("\n  " CL_KEY "Nhấn Enter để thực hiện lượt tìm kiếm mới..." RESET);
        getchar();
    }
}

void menuPrefixSearch(TrieNode *trieRoot) {
    char keyword[256];
    while (1) {
        printf("\x1b[2J\x1b[H"); // Clear screen mượt mà bằng mã ANSI Escape
        
        // 1. HEADER & BREADCRUMB
        printf("\n  " CL_BORDER "Dashboard > Tra cứu > " RESET BOLD "PREFIX SEARCH (TÌM THEO TIỀN TỐ)" RESET "\n");
        printf(CL_BORDER "  ────────────────────────────────────────────────────────────" "\n" RESET);
        
        getchar(); // Đọc bỏ ký tự '\n'
        printf(RESET);
        
        // 3. INPUT BAR
        printf("\n  " CL_HIGHLIGHT " PREFIX INPUT " RESET "\n");
        printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
        printf(CL_BORDER "  │ " RESET "Nhập chuỗi ký tự bắt đầu (Ví dụ: 'ka', 'sh') " BOLD "⚡" RESET ": " CL_LOGO);
        
        // Dùng hàm inputString của bạn để hỗ trợ unicode/chuỗi dài mượt mà
        inputString(keyword, 256);
        printf(RESET CL_BORDER "  └────────────────────────────────────────────────────────────┘\n" RESET);
        
        printf("  " CL_TEXT "Cây Trie đang xử lý luồng dữ liệu..." RESET "\n");
        
        // 4. RESULTS VIEW PAGE
        printf("\x1b[2J\x1b[H"); 
        printf("\n  " CL_BORDER "Dashboard > Prefix Search > " RESET BOLD "RESULTS" RESET "\n");
        printf(CL_BORDER "  ────────────────────────────────────────────────────────────" "\n" RESET);
        printf("  " BOLD "Danh sách từ vựng bắt đầu bằng: " RESET "\"" CL_LOGO "%s" RESET "\"\n", keyword);
        printf(CL_BORDER "  ────────────────────────────────────────────────────────────" "\n\n" RESET);
        
        TrieNode *matchNode = searchPrefixNode(trieRoot, keyword);
        if (!matchNode) {
            printf("  " CL_KEY "[!]" RESET " Không tồn tại từ vựng nào bắt đầu bằng từ khóa: \"" CL_HIGHLIGHT "%s" RESET "\"\n", keyword);
        } else {
            int resultCounter = 0;
            printAllWordsFromNode(matchNode, &resultCounter);
            if(resultCounter == 0) {
                printf("  " CL_KEY "[!]" RESET " Không có kết quả nào phù hợp hoàn toàn.\n");
            } else {
                printf("\n  " CL_HIGHLIGHT " THÀNH CÔNG " RESET " Tìm thấy tổng cộng " BOLD "%d" RESET " kết quả phù hợp.\n", resultCounter);
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
        // Dùng ANSI để xóa màn hình cho mượt (không bị nháy như system("cls"))
        printf("\x1b[2J\x1b[H"); 
        
        // 1. HEADER & BREADCRUMB
        printf("\n  " CL_BORDER "Dashboard > " RESET BOLD "HỆ THỐNG TÌM KIẾM" RESET "\n");
        printf(CL_BORDER "  ────────────────────────────────────────────────────────────" "\n" RESET);
        
        // 2. SEARCH MENU BOX
        // Toàn bộ các dòng đã được căn chỉnh độ dài ký tự hiển thị để khít với khung viền
        printf(CL_BORDER "  ┌────────────────────────────────────────────────────────────┐\n" RESET);
        printf(CL_BORDER "  │" RESET "  " BOLD "CHỌN PHƯƠNG THỨC TRA CỨU" RESET "                                 " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  ├──────┬─────────────────────────────────────────────────────┤\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "01" CL_BORDER "  │" RESET " 🎯  Exact Match (Tìm chính xác 100%%)                " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "02" CL_BORDER "  │" RESET " 🔍  Pattern Match (KMP - Tìm chuỗi con)              " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "03" CL_BORDER "  │" RESET " 🌫️  Fuzzy Match (Tìm gần đúng Levenshtein)          " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "04" CL_BORDER "  │" RESET " ⚡  Prefix Search (Gợi ý tiền tố cây Trie)          " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  │  " CL_KEY "00" CL_BORDER "  │" RESET " ↩️  Quay lại Menu chính                              " CL_BORDER "│\n" RESET);
        printf(CL_BORDER "  └──────┴─────────────────────────────────────────────────────┘\n" RESET);
        
        // 3. PROMPT NHẬP LIỆU
        printf("\n  " BOLD "Lựa chọn của bạn" RESET " » " CL_KEY);
        if (scanf("%d", &choice) != 1) { 
            while(getchar() != '\n'); 
            continue; 
        }
        getchar(); // Đọc bỏ ký tự '\n' dư thừa
        printf(RESET);
        
        if (choice == 0) break;
        switch (choice) {
            case 1: menuExactSearch(vHT, kHT); break;
            case 2: menuKMPSearch(L); break;
            case 3: menuFuzzySearch(L); break;
            case 4: menuPrefixSearch(trieRoot); break; // Gọi giao diện tìm kiếm Trie
            default:
                printf("\n  " "\x1b[31m" "(!) Lựa chọn không hợp lệ. Vui lòng chọn lại." RESET);
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
void caseNo6(KanjiList *L) {
    char keyword[256];
    printf("\x1b[2J\x1b[H"); 
    printf("\n  " CL_BORDER "Dashboard > " RESET BOLD "KANJI HỌ HÀNG" RESET "\n");
    RadicalGraph graph = buildRadicalGraph(*L);
    printf("\n  " CL_HIGHLIGHT " INPUT " RESET " Nhập bộ thủ (vd: 木, 亻, 氵): " CL_LOGO);
    inputString(keyword, 256);
    findRelatedKanjis(graph, *L, keyword);
    freeRadicalGraph(&graph);
    printf("\n  " CL_KEY "Nhấn Enter để quay lại..." RESET);
    getchar();
}
void handleMenuSelection(char *rawJson) {
    int choice;

    // Phân tích cú pháp JSON
    KanjiList myData = parseJsonToStruct(rawJson);

    // Khởi tạo các cấu trúc dữ liệu tra cứu siêu tốc
    HashTable *vocabHT = createHashTable(HASH_TABLE_SIZE);
    HashTableK *kanjiHT = createHashTableK(HASH_TABLE_SIZE);
    
    // --- KHỞI TẠO VÀ DỰNG CÂY TRIE TẠI ĐÂY ---
    TrieNode *trieRoot = buildTrieFromKanjiList(&myData);

    buildHashTableForVocab(&myData, vocabHT);
    buildHashTableForKanji(&myData, kanjiHT);

    while (1) {
        system("cls");
        displayMenu();
        if (scanf("%d", &choice) != 1) {
            printf("Vui long nhap so hop le.\n");
            while (getchar() != '\n'); // Clear buffer
            continue;
        }
        getchar(); // Clear newline character

        switch (choice) {
            case 1: {
                // Truyền thêm trieRoot vào caseNo1
                caseNo1(&myData, vocabHT, kanjiHT, trieRoot);
                break;
            }
            case 2:
                caseNo2(&myData);
                pauseAndClear();
                break;
            case 3:
                caseNo3(&myData);
                pauseAndClear();
                break;
            case 4:
                caseNo4(&myData);
                pauseAndClear();
                break;
            case 5:
                caseNo5(&myData); // bai tap
                break;
            case 6:
                caseNo6(&myData);
                break;
            case 0:
                printf("Dang thoat chuong trinh...\n");
                // --- GIẢI PHÓNG BỘ NHỚ CÂY TRIE TRƯỚC KHI THOÁT ---
                freeTrie(trieRoot);
                // (Nếu có hàm free cho HashTable, bạn cũng nên gọi ở đây)
                return;
            default:
                printf("Lua chon khong hop le. Vui long chon lai.\n");
                getchar(); // chờ người dùng nhấn phím
        }
    }
}