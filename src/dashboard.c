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
    // 1. TIÊU ĐỀ HỆ THỐNG (Căn chỉnh khoảng trống lý tưởng, tối giản thanh lịch)
    printf("\n");
    printf("  " BG_HIGHLIGHT BOLD "  TỪ ĐIỂN 512 KANJILOOK&LEARN  " RESET);
    printf("  " CL_DIM "│  ##" RESET "\n");
    printf("  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n");

    // 2. DANH SÁCH TÁC VỤ (Cấu trúc Flat-List đối xứng cao độ)
    // Tách riêng Emoji ra ngoài chuỗi %s để đảm bảo độ rộng cột %-35s chính xác 100%
    printf("  " CL_PRIMARY "┃ " CL_DIM "%-6s" CL_HEADER BOLD "%-35s" RESET "\n", "STT", "CÁC CHỨC NĂNG");
    printf("  " CL_PRIMARY "┃ " CL_BORDER "────────────────────────────────────────────────────────────────" RESET "\n");

    // Chức năng 01
    printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "01" RESET "  │  🔍  " CL_TEXT "%-35s" RESET "\n", 
           "Tìm kiếm, tra cứu từ vựng nâng cao");
    
    // Chức năng 02
    printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "02" RESET "  │  📁  " CL_TEXT "%-35s" RESET "\n", 
           "Quản lý danh mục từ điển");
    
    // Chức năng 03
    printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "03" RESET "  │  📑  " CL_TEXT "%-35s" RESET "\n", 
           "Lọc từ vựng thông minh");
    
    // Chức năng 04
    printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "04" RESET "  │  🧠  " CL_TEXT "%-35s" RESET "\n", 
           "Phân tích Hán tự trong câu");
    
    // Chức năng 05
    printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "05" RESET "  │  📝  " CL_TEXT "%-35s" RESET "\n", 
           "Luyện tập trắc nghiệm");

    // Phân tách hệ thống
    printf("  " CL_PRIMARY "┃ " CL_BORDER "────────────────────────────────────────────────────────────────" RESET "\n");
    
    // Lệnh thoát (Thiết kế đồng bộ cấu trúc với các dòng trên)
    printf("  " CL_ERROR "┃ " CL_ERROR BOLD "00" RESET "  │  ❌  " CL_ERROR BOLD "%-35s" RESET "\n", 
           "THOÁT CHƯƠNG TRÌNH");
    
    //printf("  " CL_PRIMARY "┃" RESET "\n");

    // 3. THANH TRẠNG THÁI VÀ KHU VỰC NHẬP LỆNH (COMMAND PROMPT)
    printf("  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n");
    printf("  " CL_DIM "Hướng dẫn:" RESET " Chọn số [" CL_PRIMARY "1-5" RESET "] để khởi chạy chức năng " CL_DIM "│" RESET " Chọn [" CL_ERROR "0" RESET "] để thoát\n");
    printf("  " BOLD "Lựa chọn của bạn" RESET " " CL_PRIMARY "» " RESET);
}

void menuExactSearch(HashTable *vocabHT, HashTableK *kanjiHT) {
    int choice;
    char keyword[256];
    while (1) {
        // Xóa màn hình và đưa con trỏ về góc trên bên trái
        printf("\x1b[2J\x1b[H");
        
        // 1. THANH DIỀU HƯỚNG SUB-HEADER
        printf("\n");
        printf("  " BG_HIGHLIGHT BOLD "  Tra cứu dữ liệu chính xác (HashTable)  " RESET);
        printf("\n\n");
        // 2. DANH SÁCH PHẠM VI TRA CỨU (Cấu trúc Flat-List đồng bộ)
        printf("  " CL_PRIMARY "┃ " CL_DIM "%-6s" CL_HEADER BOLD "%-35s" RESET "\n", "STT", "PHẠM VI DỮ LIỆU TRA CỨU");
        printf("  " CL_PRIMARY "┃ " CL_BORDER "──────────────────────────────────────────────────────────────" RESET "\n");

        // Chức năng 01
        printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "01" RESET "  │  ⛩️   " CL_TEXT "%-35s" RESET "\n", 
               "Tra cứu Kanji (Kí tự / Hán Việt)");
        
        // Chức năng 02
        printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "02" RESET "  │  📖  " CL_TEXT "%-35s" RESET "\n", 
               "Tra cứu Từ vựng (Kanji / Hiragana / Katakana / Romaji / Tiếng Việt)");

        // Phân tách hệ thống
        printf("  " CL_PRIMARY "┃ " CL_BORDER "──────────────────────────────────────────────────────────────" RESET "\n");
        
        // Lệnh quay lại
        printf("  " CL_PRIMARY "┃ " CL_DIM BOLD "00" RESET "  │  ↩️   " CL_DIM "%-35s" RESET "\n", 
               "Quay lại Menu tìm kiếm");
        
        printf("  " CL_PRIMARY "┃" RESET "\n");
        
        // 3. THANH TRẠNG THÁI VÀ DÒNG LỆNH CHỌN CHỨC NĂNG
        printf("  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n");
        printf("  " CL_DIM "Hướng dẫn:" RESET " Chọn số [" CL_PRIMARY "1-2" RESET "] để xác định phạm vi " CL_DIM "│" RESET " Chọn [" CL_ERROR "0" RESET "] để quay lại\n");
        printf("  " BOLD "Lựa chọn của bạn" RESET " " CL_PRIMARY "» " RESET CL_KEY);
        
        if (scanf("%d", &choice) != 1) { 
            while(getchar() != '\n'); 
            continue; 
        }
        getchar(); // Đọc ký tự newline thừa trong bộ đệm
        printf(RESET);
        
        if (choice == 0) break;
        if (choice != 1 && choice != 2) {
            printf("\n  " CL_ERROR "🚨 [LỖI]: Phạm vi chọn không hợp lệ. Vui lòng thử lại!" RESET "\n");
            printf("\n  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n");
            printf("  " CL_DIM "➔ Nhấn " CL_PRIMARY "Enter" CL_DIM " để tiếp tục..." RESET);
            getchar();
            continue;
        }
        
        // 4. KHU VỰC NHẬP TỪ KHÓA (Giao diện SEARCH BAR phẳng, hiện đại)
        printf("\n");
        printf("  " CL_BORDER ">>> Nhập từ khóa: " RESET CL_LOGO BOLD);
        
        inputString(keyword, 256);
        printf(RESET);
    
        
        // 5. TRANG HIỂN THỊ KẾT QUẢ (RESULTS PAGE)
        printf("\x1b[2J\x1b[H"); 
        printf("\n");
        printf("  " BG_HIGHLIGHT BOLD "  KẾT QUẢ TRA CỨU DỮ LIỆU  " RESET);
        printf("  " CL_DIM "Từ khóa:" RESET " \"" CL_LOGO BOLD "%s" RESET "\"\n", keyword);
        printf("  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n\n");
        
        // Gọi hàm xử lý logic tìm kiếm core
        if (choice == 1) {
            exactlySearchingKanji(kanjiHT, keyword);
        } else {
            exactlySearching(vocabHT, keyword);
        }
        
        // Thanh footer dừng xem kết quả
        printf("\n  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n");
        printf("  " CL_DIM "➔ Nhấn " CL_PRIMARY "Enter" CL_DIM " để tiếp tục ..." RESET);
        getchar();
    }
}

void menuKMPSearch(KanjiList *L) {
    int option;
    char keyword[256];
    while (1) {
        // Xóa màn hình và đưa con trỏ về góc trên bên trái
        printf("\x1b[2J\x1b[H");
        
        // 1. THANH DIỀU HƯỚNG SUB-HEADER
        printf("\n");
        printf("  " BG_HIGHLIGHT BOLD "  Khớp mẫu phân đoạn chuỗi con (KMP Algorithm)  " RESET);
        printf("\n\n");
        // 2. DANH SÁCH CHẾ ĐỘ KHỚP MẪU (Cấu trúc Flat-List đồng bộ)
        printf("  " CL_PRIMARY "┃ " CL_DIM "%-6s" CL_HEADER BOLD "%-35s" RESET "\n", "STT", "CHẾ ĐỘ KHỚP MẪU");
        printf("  " CL_PRIMARY "┃ " CL_BORDER "──────────────────────────────────────────────────────────────" RESET "\n");

        // Chức năng 01
        printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "01" RESET "  │  ⛩️   " CL_TEXT "%-35s" RESET "\n", 
               "Khớp theo mặt chữ Kanji");
        
        // Chức năng 02
        printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "02" RESET "  │  🎋  " CL_TEXT "%-35s" RESET "\n", 
               "Khớp theo cách đọc Hiragana");
        
        // Chức năng 03
        printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "03" RESET "  │  🔡  " CL_TEXT "%-35s" RESET "\n", 
               "Khớp theo phiên âm Romaji");
        
        // Chức năng 04
        printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "04" RESET "  │  🇻🇳  " CL_TEXT "%-35s" RESET "\n", 
               "Khớp theo nghĩa Tiếng Việt");

        // Phân tách hệ thống
        printf("  " CL_PRIMARY "┃ " CL_BORDER "──────────────────────────────────────────────────────────────" RESET "\n");
        
        // Lệnh quay lại
        printf("  " CL_PRIMARY "┃ " CL_DIM BOLD "00" RESET "  │  ↩️   " CL_DIM "%-35s" RESET "\n", 
               "Quay lại Menu tìm kiếm");
        
        printf("  " CL_PRIMARY "┃" RESET "\n");
        
        // 3. THANH TRẠNG THÁI VÀ DÒNG LỆNH CHỌN CHẾ ĐỘ
        printf("  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n");
        printf("  " CL_DIM "Hướng dẫn:" RESET " Chọn số [" CL_PRIMARY "1-4" RESET "] để xác định chế độ " CL_DIM "│" RESET " Chọn [" CL_ERROR "0" RESET "] để quay lại\n");
        printf("  " BOLD "Lựa chọn của bạn" RESET " " CL_PRIMARY "» " RESET CL_KEY);
        
        if (scanf("%d", &option) != 1) { 
            while(getchar() != '\n'); 
            continue; 
        }
        getchar(); // Đọc ký tự newline thừa trong bộ đệm
        printf(RESET);
        
        if (option == 0) break;
        if (option < 1 || option > 4) {
            printf("\n  " CL_ERROR "🚨 [LỖI]: Chế độ chọn không hợp lệ. Vui lòng thử lại!" RESET "\n");
            printf("\n  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n");
            printf("  " CL_DIM "➔ Nhấn " CL_PRIMARY "Enter" CL_DIM " để tiếp tục..." RESET);
            getchar();
            continue;
        }
        
        // 4. KHU VỰC NHẬP MẪU CHUỖI (SEARCH BAR phẳng, hiện đại)
        printf("\n");
        printf("  " CL_BORDER ">>> Nhập từ khóa: " RESET CL_LOGO BOLD);

        inputString(keyword, 256);
        printf(RESET);
        
        // 5. TRANG HIỂN THỊ KẾT QUẢ (RESULTS PAGE)
        printf("\x1b[2J\x1b[H"); 
        printf("\n");
        printf("  " BG_HIGHLIGHT BOLD "  KẾT QUẢ TRA CỨU DỮ LIỆU  " RESET);
        printf("  " CL_DIM "Từ khóa:" RESET " \"" CL_LOGO BOLD "%s" RESET "\"\n", keyword);
        printf("  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n\n");
        
        // Gọi bộ xử lý logic tìm kiếm chuỗi con
        substringSearching(L, keyword, option);
        
        // Thanh footer dừng xem kết quả trước khi lặp lại vòng quét mới
        printf("\n  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n");
        printf("  " CL_DIM "➔ Nhấn " CL_PRIMARY "Enter" CL_DIM " để tiếp tục ..." RESET);
        getchar();
    }
}
void menuFuzzySearch(KanjiList *L) {
    int option;
    char keyword[256];
    while (1) {
        // Xóa màn hình và đưa con trỏ về góc trên bên trái
        printf("\x1b[2J\x1b[H");
        
        // 1. THANH DIỀU HƯỚNG SUB-HEADER
        printf("\n");
        printf("  " BG_HIGHLIGHT BOLD "  Tìm kiếm từ khóa gần đúng (Levenshtein Distance)  " RESET);
        printf("\n\n");
        // 2. DANH SÁCH CHẾ ĐỘ TÌM KIẾM (Cấu trúc Flat-List đồng bộ)
        printf("  " CL_PRIMARY "┃ " CL_DIM "%-6s" CL_HEADER BOLD "%-35s" RESET "\n", "STT", "DẠNG TỪ KHÓA");
        printf("  " CL_PRIMARY "┃ " CL_BORDER "──────────────────────────────────────────────────────────────" RESET "\n");

        // Chức năng 01
        printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "01" RESET "  │  🌫️  " CL_TEXT "%-35s" RESET "\n", 
               "Khớp gần đúng mặt chữ Kanji");
        
        // Chức năng 02
        printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "02" RESET "  │  🍃  " CL_TEXT "%-35s" RESET "\n", 
               "Khớp gần đúng cách đọc Hiragana");
        
        // Chức năng 03
        printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "03" RESET "  │  🔤  " CL_TEXT "%-35s" RESET "\n", 
               "Khớp gần đúng phiên âm Romaji");
        
        // Chức năng 04
        printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "04" RESET "  │  💬  " CL_TEXT "%-35s" RESET "\n", 
               "Khớp gần đúng nghĩa Tiếng Việt");

        // Phân tách hệ thống
        printf("  " CL_PRIMARY "┃ " CL_BORDER "──────────────────────────────────────────────────────────────" RESET "\n");
        
        // Lệnh quay lại
        printf("  " CL_PRIMARY "┃ " CL_DIM BOLD "00" RESET "  │  ↩️   " CL_DIM "%-35s" RESET "\n", 
               "Quay lại Menu tìm kiếm");
        
        printf("  " CL_PRIMARY "┃" RESET "\n");
        
        // 3. THANH TRẠNG THÁI VÀ DÒNG LỆNH CHỌN CHẾ ĐỘ
        printf("  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n");
        printf("  " CL_DIM "Hướng dẫn:" RESET " Chọn số [" CL_PRIMARY "1-4" RESET "] để xác định chế độ " CL_DIM "│" RESET " Chọn [" CL_ERROR "0" RESET "] để quay lại\n");
        printf("  " BOLD "Lựa chọn của bạn" RESET " " CL_PRIMARY "» " RESET CL_KEY);
        
        if (scanf("%d", &option) != 1) { 
            while(getchar() != '\n'); 
            continue; 
        }
        getchar(); // Đọc ký tự newline thừa trong bộ đệm
        printf(RESET);
        
        if (option == 0) break;
        if (option < 1 || option > 4) {
            printf("\n  " CL_ERROR "🚨 [LỖI]: Chế độ chọn không hợp lệ. Vui lòng thử lại!" RESET "\n");
            printf("\n  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n");
            printf("  " CL_DIM "➔ Nhấn " CL_PRIMARY "Enter" CL_DIM " để tiếp tục..." RESET);
            getchar();
            continue;
        }
        
        // 4. KHU VỰC NHẬP TỪ KHÓA TÌM MỜ (SEARCH BAR phẳng, hiện đại)
        printf("\n");
        printf("  " CL_BORDER ">>> Nhập từ khóa: " RESET CL_LOGO BOLD);
        
        inputString(keyword, 256);
        printf(RESET);
        

        // 5. TRANG HIỂN THỊ KẾT QUẢ (RESULTS PAGE)
        printf("\x1b[2J\x1b[H"); 
        printf("\n");
        printf("  " BG_HIGHLIGHT BOLD "  KẾT QUẢ TRA CỨU DỮ LIỆU  " RESET);
        printf("  " CL_DIM "Từ khóa:" RESET " \"" CL_LOGO BOLD "%s" RESET "\"\n", keyword);
        printf("  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n\n");
        
        // Gọi bộ xử lý logic tìm kiếm mờ dựa trên khoảng cách chỉnh sửa
        fuzzySearching(L, keyword, option);
        
        // Thanh footer dừng xem kết quả trước khi lặp lại vòng quét mới
        printf("\n  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n");
        printf("  " CL_DIM "➔ Nhấn " CL_PRIMARY "Enter" CL_DIM " để thực hiện lượt tìm kiếm mới..." RESET);
        getchar();
    }
}

void menuPrefixSearch(TrieNode *trieRoot) {
    char keyword[256];
    int choice;
    while (1) {
        // Xóa màn hình và đưa con trỏ về góc trên bên trái
        printf("\x1b[2J\x1b[H");
        
        // 1. THANH DIỀU HƯỚNG SUB-HEADER
        printf("\n");
        printf("  " BG_HIGHLIGHT BOLD "  Tìm theo tiền tố (Trie Data Structure)  " RESET);
        printf("\n\n");
        // 2. DANH SÁCH CHẾ ĐỘ (Cấu trúc Flat-List đồng bộ)
        printf("  " CL_PRIMARY "┃ " CL_DIM "%-6s" CL_HEADER BOLD "%-35s" RESET "\n", "STT", "ĐỊNH DẠNG CẦN TÌM");
        printf("  " CL_PRIMARY "┃ " CL_BORDER "──────────────────────────────────────────────────────────────" RESET "\n");

        // Chức năng 01
        printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "01" RESET "  │  ⚡  " CL_TEXT "%-35s" RESET "\n", 
               "Tìm theo Romaji (chữ cái Alphabet)");

        // Phân tách hệ thống
        printf("  " CL_PRIMARY "┃ " CL_BORDER "──────────────────────────────────────────────────────────────" RESET "\n");
        
        // Lệnh quay lại
        printf("  " CL_PRIMARY "┃ " CL_DIM BOLD "00" RESET "  │  ↩️   " CL_DIM "%-35s" RESET "\n", 
               "Quay lại Menu tìm kiếm");
        
        printf("  " CL_PRIMARY "┃" RESET "\n");
        
        // 3. THANH TRẠNG THÁI VÀ DÒNG LỆNH CHỌN CHẾ ĐỘ
        printf("  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n");
        printf("  " CL_DIM "Hướng dẫn:" RESET " Chọn số [" CL_PRIMARY "1" RESET "] để kích hoạt bộ lọc " CL_DIM "│" RESET " Chọn [" CL_ERROR "0" RESET "] để quay lại\n");
        printf("  " BOLD "Lựa chọn của bạn" RESET " " CL_PRIMARY "» " RESET CL_KEY);
        
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n');
            continue;
        }
        getchar(); // Đọc ký tự newline thừa trong bộ đệm
        printf(RESET);
        
        if (choice == 0) break;
        if (choice != 1) {
            printf("\n  " CL_ERROR "🚨 [LỖI]: Lựa chọn không hợp lệ. Vui lòng thử lại!" RESET "\n");
            printf("\n  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n");
            printf("  " CL_DIM "➔ Nhấn " CL_PRIMARY "Enter" CL_DIM " để tiếp tục..." RESET);
            getchar();
            continue;
        }
        
        // 4. KHU VỰC NHẬP TIỀN TỐ (SEARCH BAR phẳng, hiện đại)
        printf("\n");
        printf("  " CL_BORDER ">>> Nhập từ khóa: " RESET CL_LOGO BOLD);
        
        inputString(keyword, 256);
        printf(RESET);
        
        // 5. TRANG HIỂN THỊ KẾT QUẢ (RESULTS PAGE)
        printf("\x1b[2J\x1b[H"); 
        printf("\n");
        printf("  " BG_HIGHLIGHT BOLD "  KẾT QUẢ TRA CỨU DỮ LIỆU  " RESET);
        printf("  " CL_DIM "Từ khóa:" RESET " \"" CL_LOGO BOLD "%s" RESET "\"\n", keyword);
        printf("  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n\n");
        
        TrieNode *matchNode = searchPrefixNode(trieRoot, keyword);
        if (!matchNode) {
            printf("  " CL_ERROR "🚨 [THÔNG BÁO]:" RESET " Không tồn tại từ vựng nào bắt đầu bằng từ khóa: \"" CL_LOGO BOLD "%s" RESET "\"\n", keyword);
        } else {
            int resultCounter = 0;
            printAllWordsFromNode(matchNode, &resultCounter);
            
            if (resultCounter == 0) {
                printf("  " CL_ERROR "🚨 [THÔNG BÁO]:" RESET " Không có kết quả nào phù hợp hoàn toàn với chuỗi cung cấp.\n");
            } else {
                printf("\n  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n");
                printf("  " BG_HIGHLIGHT BOLD "  TRUY XUẤT HOÀN TẤT  " RESET " Tìm thấy tổng cộng " CL_KEY BOLD "%d" RESET " kết quả phù hợp.\n", resultCounter);
            }
        }
        
        // Thanh footer dừng xem kết quả trước khi lặp lại vòng quét mới
        printf("\n  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n");
        printf("  " CL_DIM "➔ Nhấn " CL_PRIMARY "Enter" CL_DIM " để thực hiện lượt tìm kiếm mới..." RESET);
        getchar();
    }
}

void caseNo1(KanjiList *L, HashTable *vHT, HashTableK *kHT, TrieNode *trieRoot) {
    int choice;
    while(1) {
        // Xóa màn hình và đưa con trỏ về góc trên bên trái
        printf("\x1b[2J\x1b[H"); 
        
        // 1. THANH DIỀU HƯỚNG SUB-HEADER (Hiện đại, tối giản)
        printf("\n");
        printf("  " BG_HIGHLIGHT BOLD "  HỆ THỐNG TÌM KIẾM NÂNG CAO  " RESET);
        printf("  " CL_DIM "│  04 phương thức tra cứu" RESET "\n");
        printf("  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n\n");
        
        // 2. DANH SÁCH PHƯƠNG THỨC TRA CỨU (Cấu trúc phẳng, đồng bộ 100% với Menu chính)
        printf("  " CL_PRIMARY "┃ " CL_DIM "%-6s" CL_HEADER BOLD "%-35s" RESET "\n", "STT", "⚡ PHƯƠNG THỨC TRA CỨU");
        printf("  " CL_PRIMARY "┃ " CL_BORDER "──────────────────────────────────────────────────────────────" RESET "\n");

        // Chức năng 01
        printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "01" RESET "  │  🔍  " CL_TEXT "%-35s" RESET "\n", 
               "Tra cứu dữ liệu chính xác (HashTable)");
        
        // Chức năng 02
        printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "02" RESET "  │  🔍  " CL_TEXT "%-35s" RESET "\n", 
               "Khớp mẫu phân đoạn chuỗi con (KMP Algorithm)");
        
        // Chức năng 03
        printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "03" RESET "  │  🔍  " CL_TEXT "%-35s" RESET "\n", 
               "Tìm kiếm từ khóa gần đúng (Levenshtein Distance)");
        
        // Chức năng 04
        printf("  " CL_PRIMARY "┃ " CL_KEY BOLD "04" RESET "  │  🔍  " CL_TEXT "%-35s" RESET "\n", 
               "Tìm theo tiền tố (Trie Data Structure)");

        // Phân tách hệ thống
        printf("  " CL_PRIMARY "┃ " CL_BORDER "──────────────────────────────────────────────────────────────" RESET "\n");
        
        // Lệnh quay lại
        printf("  " CL_PRIMARY "┃ " CL_DIM BOLD "00" RESET "  │  ↩️  " CL_DIM "%-35s" RESET "\n", 
               "Quay lại Menu chính");
        
        printf("  " CL_PRIMARY "┃" RESET "\n");
        
        // 3. THANH TRẠNG THÁI VÀ DÒNG LỆNH INPUT
        printf("  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n");
        printf("  " CL_DIM "Hướng dẫn:" RESET " Chọn số [" CL_PRIMARY "1-4" RESET "] để kích hoạt bộ lọc " CL_DIM "│" RESET " Chọn [" CL_ERROR "0" RESET "] để về trang chủ\n");
        printf("  " BOLD "Lựa chọn của bạn" RESET " " CL_PRIMARY "» " RESET CL_KEY);
        
        if (scanf("%d", &choice) != 1) { 
            while(getchar() != '\n'); 
            continue; 
        }
        getchar(); // Đọc ký tự newline thừa còn lại trong bộ đệm
        printf(RESET);
        
        if (choice == 0) break;
        
        switch (choice) {
            case 1: menuExactSearch(vHT, kHT); break;
            case 2: menuKMPSearch(L); break;
            case 3: menuFuzzySearch(L); break;
            case 4: menuPrefixSearch(trieRoot); break;
            default:
                // Chuẩn hóa thông báo lỗi theo tông màu CL_ERROR đồng bộ hệ thống
                printf("\n  " CL_ERROR "🚨 [LỖI]: Lựa chọn không hợp lệ. Vui lòng thử lại!" RESET "\n");
                break;
        }
        
        // Thanh footer giữ giao diện dừng lại xem kết quả trước khi xóa màn hình vòng lặp mới
        printf("\n  " CL_BORDER "──────────────────────────────────────────────────────────────────" RESET "\n");
        printf("  " CL_DIM "➔ Nhấn " CL_PRIMARY "Enter" CL_DIM " để tiếp tục hệ thống tra cứu..." RESET);
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
            case 1: caseNo1(&myData, vocabHT, kanjiHT, trieRoot); getchar(); break;
            case 2: caseNo2(&myData); getchar(); break;
            case 3: caseNo3(&myData); getchar(); break;
            case 4: caseNo4(&myData); getchar(); break;
            case 5: caseNo5(&myData); getchar(); break;
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