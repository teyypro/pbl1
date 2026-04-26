#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <wchar.h>

typedef struct SampleInfo Sample;
typedef struct VocabInfo Vocab;
typedef struct YomiInfo Yomi;
typedef struct KanjiInfo Kanji;
typedef struct KanjiListInfo KanjiList;

struct SampleInfo {
    char *jp;
    char *vn;
};

struct VocabInfo {
    char *vocab;    //utf8
    wchar_t *vocab_w; //wide char để xử lý Levenshtein
    char *hiragana;
    wchar_t *hiragana_w; //wide char để xử lý Levenshtein
    char *romaji;
    wchar_t *romaji_w; //wide char để xử lý Levenshtein
    char *meaning;
    wchar_t *meaning_w; //wide char để xử lý Levenshtein
    Sample *samples;
    int samplesCount;
};

struct YomiInfo {
    char *jp;
    char *romaji;
};

struct KanjiInfo {
    int stt;
    char *kanji;
    wchar_t *kanji_w;
    char *hanViet;
    char *radical;
    char *stroke;
    char *description;

    Yomi *on;
    int onCount;
    Yomi *kun;
    int kunCount;

    Vocab *vocabs;
    int vocabsCount;
};

struct KanjiListInfo {
    Kanji *kanjis;
    int kanjiCount;
};


#endif