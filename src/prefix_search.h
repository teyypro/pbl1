#ifndef PREFIX_SEARCH_H
#define PREFIX_SEARCH_H

#include "../data_structures.h"
#include "dashboard.h" // Để sử dụng các mã màu CL_BORDER, CL_KEY, RESET, v.v.

#define ALPHABET_SIZE 28 // 26 chữ cái từ a-z + ký tự '-' + ký tự '.'

// Cấu trúc một nút trong cây Trie tổng quát
typedef struct TrieNodeInfo TrieNode;
struct TrieNodeInfo {
    TrieNode *children[ALPHABET_SIZE];
    int isEndOfWord;
    
    // Mảng động lưu các con trỏ trỏ tới Struct Vocab trong bộ nhớ gốc (tránh nhân bản dữ liệu)
    Vocab **vocabs; 
    int vocabsCount;
};

// ─── CÁC HÀM QUẢN LÝ CẤU TRÚC ĐỒ THỊ/CÂY TRIE ───────────────────────────
TrieNode* createTrieNode();
int getCharIndex(char c);
void insertTrie(TrieNode *root, const char *key, Vocab *vocab);
TrieNode* buildTrieFromKanjiList(KanjiList *L);
TrieNode* searchPrefixNode(TrieNode *root, const char *prefix);
void freeTrie(TrieNode *root);

// ─── CÁC HÀM HIỂN THỊ VÀ GIAO DIỆN MENU ─────────────────────────────────
void printVocabDetails(Vocab *v);
void printAllWordsFromNode(TrieNode *node, int *counter);

#endif