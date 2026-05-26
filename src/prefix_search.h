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

TrieNode* buildTrieFromKanjiList(KanjiList *L);
TrieNode* searchPrefixNode(TrieNode *root, const char *prefix);
void printAllWordsFromNode(TrieNode *node, int *counter);
void freeTrie(TrieNode *root);

#endif