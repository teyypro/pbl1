#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H


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
    char *vocab;
    char *hiragana;
    char *romaji;
    char *meaning;
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