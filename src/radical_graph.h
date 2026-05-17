#ifndef RADICAL_GRAPH_H
#define RADICAL_GRAPH_H

#include "../data_structures.h"

// Cấu trúc một nút trong danh sách kề để lưu Kanji họ hàng
typedef struct RelatedNode {
    int kanjiIndex;          // Chỉ số (index) của Kanji trong mảng KanjiList
    struct RelatedNode *next;
} RelatedNode;

// Cấu trúc danh sách kề cho mỗi Bộ thủ
typedef struct {
    char *radicalChar;       // Ký tự bộ thủ (ví dụ: "亻", "氵", "木")
    RelatedNode *head;       // Danh sách các Kanji thuộc bộ này
} RadicalEntry;

typedef struct {
    RadicalEntry *entries;
    int count;
} RadicalGraph;

// Tìm index node có radicalChar khớp chính xác với radicalInput (-1 nếu không có)
int findRadicalIndex(RadicalGraph graph, const char *radicalInput);

// Xây dựng đồ thị từ KanjiList (tách multi-radical "木, 一" thành từng node riêng)
RadicalGraph buildRadicalGraph(KanjiList list);

// Tìm và in tất cả Kanji có cùng bộ thủ với radicalInput
void findRelatedKanjis(RadicalGraph graph, KanjiList list, const char *radicalInput);

// Giải phóng toàn bộ bộ nhớ của đồ thị
void freeRadicalGraph(RadicalGraph *graph);

#endif