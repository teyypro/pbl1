#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "radical_graph.h"

// Tìm index của node có radicalChar khớp CHÍNH XÁC với radicalInput
int findRadicalIndex(RadicalGraph graph, const char *radicalInput) {
    for (int i = 0; i < graph.count; i++) {
        if (strcmp(graph.entries[i].radicalChar, radicalInput) == 0) {
            return i;
        }
    }
    return -1;
}

// Hàm nội bộ: thêm kanjiIndex vào node của một radical đơn
// Nếu node chưa tồn tại thì tạo mới
static void addKanjiToRadicalNode(RadicalGraph *graph, const char *radToken, int kanjiIndex) {
    int idx = findRadicalIndex(*graph, radToken);
    if (idx == -1) {
        graph->count++;
        graph->entries = (RadicalEntry*)realloc(graph->entries,sizeof(RadicalEntry) * graph->count);
        idx = graph->count - 1;
        graph->entries[idx].radicalChar = strdup(radToken);
        graph->entries[idx].head = NULL;
    }
    RelatedNode *newNode = (RelatedNode*)malloc(sizeof(RelatedNode));
    newNode->kanjiIndex = kanjiIndex;
    newNode->next = graph->entries[idx].head;
    graph->entries[idx].head = newNode;
}
// Xây dựng Adjacency List từ KanjiList
RadicalGraph buildRadicalGraph(KanjiList list) {
    RadicalGraph graph = {NULL, 0};
    for (int i = 0; i < list.kanjiCount; i++) {
        char *rad = list.kanjis[i].radical;
        if (!rad || strlen(rad) == 0) continue;
        char *copy = strdup(rad);
        if (!copy) continue;
        // Tách theo ", " — 
        char *token = strtok(copy, ", ");
        while (token != NULL) {
            if (strlen(token) > 0) {
                addKanjiToRadicalNode(&graph, token, i);
            }
            token = strtok(NULL, ", ");
        }
        free(copy);
    }
    return graph;
}
// Hiển thị tất cả Kanji có cùng bộ thủ với radicalInput
void findRelatedKanjis(RadicalGraph graph, KanjiList list, const char *radicalInput) {
    int idx = findRadicalIndex(graph, radicalInput);
    if (idx != -1) {
        printf("\n  \x1b[1mCac Kanji ho hang cua bo [%s]:\x1b[0m\n", radicalInput);
        RelatedNode *curr = graph.entries[idx].head;
        while (curr != NULL) {
            Kanji *k = &list.kanjis[curr->kanjiIndex];
            printf("  - \x1b[38;5;208m%s\x1b[0m (%s): %s\n",
                   k->kanji, k->hanViet, k->description);
            curr = curr->next;
        }
    } else {
        printf("\n  \x1b[33m[!] Khong tim thay trong muc 'Radical'. Dang quet mo phong...\x1b[0m\n");
        int found = 0;
        for (int i = 0; i < list.kanjiCount; i++) {
            if (list.kanjis[i].description && strstr(list.kanjis[i].description, radicalInput)) {
                printf("  - \x1b[38;5;208m%s\x1b[0m (%s): %s\n",
                       list.kanjis[i].kanji, list.kanjis[i].hanViet, list.kanjis[i].description);
                found = 1;
            }
        }
        if (!found) printf("  \x1b[31m[!] Khong tim thay bat ky chu nao lien quan.\x1b[0m\n");
    }
}
void freeRadicalGraph(RadicalGraph *graph) {
    if (!graph || !graph->entries) return;

    for (int i = 0; i < graph->count; i++) {
        if (graph->entries[i].radicalChar) {
            free(graph->entries[i].radicalChar);
        }
        RelatedNode *curr = graph->entries[i].head;
        while (curr) {
            RelatedNode *tmp = curr;
            curr = curr->next;
            free(tmp);
        }
    }
    free(graph->entries);
    graph->entries = NULL;
    graph->count = 0;
}