// ==================== src/prefix_search.c ====================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "prefix_search.h"
#include "../data_structures.h"
#include "utils.h"

#define ALPHABET_SIZE 28

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

int getCharIndex(char c) {
    c = tolower(c);
    if (c >= 'a' && c <= 'z') return c - 'a';
    if (c == '-') return 26;
    if (c == '.') return 27;
    return -1;
}

void insertTrie(TrieNode *root, const char *key, Vocab *vocab) {
    if (!root || !key || strlen(key) == 0) return;

    TrieNode *current = root;
    for (int i = 0; key[i] != '\0'; i++) {
        int index = getCharIndex(key[i]);
        if (index == -1) continue;

        if (!current->children[index]) {
            current->children[index] = createTrieNode();
        }
        current = current->children[index];
    }
    
    current->isEndOfWord = 1;
    
    Vocab **temp = (Vocab**)realloc(current->vocabs, (current->vocabsCount + 1) * sizeof(Vocab*));
    if (temp != NULL) {
        current->vocabs = temp;
        current->vocabs[current->vocabsCount++] = vocab;
    }
}

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

TrieNode* searchPrefixNode(TrieNode *root, const char *prefix) {
    if (!root || !prefix) return NULL;
    TrieNode *current = root;
    
    for (int i = 0; prefix[i] != '\0'; i++) {
        int index = getCharIndex(prefix[i]);
        if (index == -1) return NULL;
        
        if (!current->children[index]) {
            return NULL;
        }
        current = current->children[index];
    }
    return current;
}

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

void printVocabDetails(Vocab *v) {
    printf(CL_BORDER "   ┌────────────────────────────────────────────────────────────┐\n" RESET);
    printf(CL_BORDER "   │ " CL_LOGO "📖 %-42s " CL_BORDER "│\n" RESET, v->vocab ? v->vocab : "");
    printf(CL_BORDER "   │ " CL_TEXT "💬 %-42s " CL_BORDER "│\n" RESET, v->hiragana ? v->hiragana : "");
    printf(CL_BORDER "   │ " CL_KEY "🔍 %-42s " CL_BORDER "│\n" RESET, v->meaning ? v->meaning : "");
    
    if (v->samplesCount > 0 && v->samples) {
        printf(CL_BORDER "   ├────────────────────────────────────────────────────────────┤\n" RESET);
        printf(CL_BORDER "   │ " CL_DIM "📌 Ví dụ:" CL_RESET "                                                 │\n" RESET);
        for (int i = 0; i < v->samplesCount && i < 2; i++) {
            printf(CL_BORDER "   │   " CL_TEXT "・%s" CL_RESET "                                │\n", v->samples[i].jp);
            printf(CL_BORDER "   │     → %s" CL_RESET "                              │\n", v->samples[i].vn);
        }
    }
    printf(CL_BORDER "   └────────────────────────────────────────────────────────────┘\n" RESET);
}

void printAllWordsFromNode(TrieNode *node, int *counter) {
    if (!node) return;

    if (node->isEndOfWord && node->vocabs) {
        for (int i = 0; i < node->vocabsCount; i++) {
            (*counter)++;
            printf("\n  " CL_KEY "[%02d]" RESET, *counter);
            printVocabDetails(node->vocabs[i]);
        }
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            printAllWordsFromNode(node->children[i], counter);
        }
    }
}