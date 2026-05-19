#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "prefix_search.h"
#include "../data_structures.h"
#include "utils.h" // Chứa hàm inputString của bạn nếu cần
#define MAX_GAP 2
#define RESET         "\x1b[0m"
#define BOLD          "\x1b[1m"
#define CL_LOGO       "\x1b[38;5;208m" 
#define CL_BORDER     "\x1b[38;5;239m" 
#define CL_TEXT       "\x1b[38;5;253m" 
#define CL_KEY        "\x1b[38;5;111m" 
#define CL_HIGHLIGHT  "\x1b[48;5;236m\x1b[38;5;208m" 
#define CL_SUCCESS    "\x1b[1;32m"
// Khởi tạo một nút Trie mới hoàn toàn trống
TrieNode* createTrieNode() {
    TrieNode *node = (TrieNode*)malloc(sizeof(TrieNode));
    if (node) {
        node->isEndOfWord = 0;
        node->vocabsCount = 0;
        node->vocabs = NULL;
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            node->children[i] = NULL;
        }
    }
    return node;
}

// Chuyển ký tự Romaji (không phân biệt hoa thường) thành Index từ 0 -> 27
int getCharIndex(char c) {
    c = tolower(c);
    if (c >= 'a' && c <= 'z') return c - 'a';
    if (c == '-') return 26;
    if (c == '.') return 27;
    return -1; // Ký tự không hợp lệ (sẽ bỏ qua)
}

// Thêm một từ vựng (key là chuỗi Romaji) vào cây Trie
void insertTrie(TrieNode *root, const char *key, Vocab *vocab) {
    if (!root || !key || strlen(key) == 0) return;

    TrieNode *current = root;
    for (int i = 0; key[i] != '\0'; i++) {
        int index = getCharIndex(key[i]);
        if (index == -1) continue; // Bỏ qua ký tự nằm ngoài bảng Alphabet Romaji

        if (!current->children[index]) {
            current->children[index] = createTrieNode();
        }
        current = current->children[index];
    }
    
    // Đánh dấu đây là điểm kết thúc của một từ Romaji hợp lệ
    current->isEndOfWord = 1;
    
    // Cấp phát hoặc mở rộng mảng con trỏ để lưu thông tin Vocab
    current->vocabsCount++;
    Vocab **temp = (Vocab**)realloc(current->vocabs, current->vocabsCount * sizeof(Vocab*));
    if (temp != NULL) {
        current->vocabs = temp;
        current->vocabs[current->vocabsCount - 1] = vocab;
    }
}

// Hàm duyệt qua danh sách KanjiList và tự động dựng cây Trie
TrieNode* buildTrieFromKanjiList(KanjiList *L) {
    if (!L) return NULL;
    
    TrieNode *root = createTrieNode();
    if (!root) return NULL;

    for (int i = 0; i < L->kanjiCount; i++) {
        Kanji *k = &L->kanjis[i];
        for (int j = 0; j < k->vocabsCount; j++) {
            Vocab *v = &k->vocabs[j];
            if (v->romaji && strlen(v->romaji) > 0) {
                insertTrie(root, v->romaji, v);
            }
        }
    }
    return root;
}

// Tìm đến nút (Node) đại diện cho ký tự cuối cùng của chuỗi Prefix nhập vào
TrieNode* searchPrefixNode(TrieNode *root, const char *prefix) {
    if (!root || !prefix) return NULL;
    TrieNode *current = root;
    
    for (int i = 0; prefix[i] != '\0'; i++) {
        int index = getCharIndex(prefix[i]);
        if (index == -1) return NULL; // Tiền tố chứa ký tự không hợp lệ
        
        if (!current->children[index]) {
            return NULL; // Không khớp tiền tố trong cây
        }
        current = current->children[index];
    }
    return current;
}

// Giải phóng bộ nhớ cây Trie một cách an toàn bằng đệ quy khử đuôi (Post-order)
void freeTrie(TrieNode *root) {
    if (!root) return;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i]) {
            freeTrie(root->children[i]);
        }
    }
    if (root->vocabs) {
        free(root->vocabs);
    }
    free(root);
}

// Định dạng in thông tin chi tiết của một từ vựng
void printVocabDetails(Vocab *v) {
    printf("   " CL_HIGHLIGHT "» From: " RESET CL_LOGO "%s" RESET " (%s) [%s]\n", 
           v->vocab ? v->vocab : "", 
           v->hiragana ? v->hiragana : "", 
           v->romaji ? v->romaji : "");
    printf("     " CL_TEXT "Ý nghĩa: %s\n", v->meaning ? v->meaning : "Chưa có nghĩa");
    
    if (v->samplesCount > 0 && v->samples) {
        printf("     " CL_BORDER "Ví dụ mẫu:\n" RESET);
        for (int i = 0; i < v->samplesCount; i++) {
            printf("       • %s : %s\n", 
                   v->samples[i].jp ? v->samples[i].jp : "", 
                   v->samples[i].vn ? v->samples[i].vn : "");
        }
    }
    printf(CL_BORDER "   ────────────────────────────────────────────────────────────\n" RESET);
}

// Đệ quy Depth-First Search (DFS) để in toàn bộ các từ thuộc cây con
void printAllWordsFromNode(TrieNode *node, int *counter) {
    if (!node) return;

    if (node->isEndOfWord && node->vocabs) {
        for (int i = 0; i < node->vocabsCount; i++) {
            (*counter)++;
            printf("  " CL_KEY "[%02d]" RESET, *counter);
            printVocabDetails(node->vocabs[i]);
        }
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            printAllWordsFromNode(node->children[i], counter);
        }
    }
}
