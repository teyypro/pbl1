#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#ifdef _WIN32
    #include <windows.h>
#endif
#include "parse_json_to_struct.h"
#include "../lib/cJSON.h"
#include "../data_structures.h"


char* readFileToString(const char *fileName) {
    FILE *file = fopen(fileName, "rb");
    if (!file) return NULL;

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = malloc(length + 1);
    if (data) {
        fread(data, 1, length, file);
        data[length] = '\0';
    }
    fclose(file);
    return data;
}

void configUTF8() {
    #ifdef _WIN32
        system("chcp 65001 > nul");
    #endif

}

char* safeStrdup(cJSON *item) {
    if (item && item->valuestring) {
        return strdup(item->valuestring);
    }
    return strdup("");  // tránh NULL
}

// ====================== PARSE ======================

Yomi* parseYomi(cJSON *yomiArr, int *count) {
    if (!yomiArr || !cJSON_IsArray(yomiArr)) {
        *count = 0;
        return NULL;
    }

    *count = cJSON_GetArraySize(yomiArr);
    Yomi *yomi = malloc(sizeof(Yomi) * (*count));

    for (int i = 0; i < *count; i++) {
        cJSON *item = cJSON_GetArrayItem(yomiArr, i);
        yomi[i].jp     = safeStrdup(cJSON_GetObjectItem(item, "jp"));
        yomi[i].romaji = safeStrdup(cJSON_GetObjectItem(item, "romaji"));
    }
    return yomi;
}

Sample* parseSamples(cJSON *samplesArr, int *count) {
    if (!samplesArr || !cJSON_IsArray(samplesArr)) {
        *count = 0;
        return NULL;
    }

    *count = cJSON_GetArraySize(samplesArr);
    Sample *samples = malloc(sizeof(Sample) * (*count));

    for (int i = 0; i < *count; i++) {
        cJSON *item = cJSON_GetArrayItem(samplesArr, i);
        samples[i].jp = safeStrdup(cJSON_GetObjectItem(item, "jp"));
        samples[i].vn = safeStrdup(cJSON_GetObjectItem(item, "vn"));
    }
    return samples;
}

// Hàm bổ trợ để chuyển đổi UTF-8 sang wchar_t
wchar_t* convertToWchar(const char *source) {
    if (!source) return NULL;
    // Bước 1: Tính độ dài cần thiết cho chuỗi wchar_t
    size_t w_len = mbstowcs(NULL, source, 0);
    if (w_len == (size_t)-1) return NULL; // Lỗi encoding
    // Bước 2: Cấp phát bộ nhớ (w_len + 1 cho ký tự kết thúc L'\0')
    wchar_t *w_str = malloc((w_len + 1) * sizeof(wchar_t));
    if (!w_str) return NULL;
    // Bước 3: Thực hiện chuyển đổi thực sự
    mbstowcs(w_str, source, w_len + 1);
    return w_str;
}

Vocab* parseVocabs(cJSON *vocabsArr, int *count) {
    if (!vocabsArr || !cJSON_IsArray(vocabsArr)) {
        *count = 0;
        return NULL;
    }

    *count = cJSON_GetArraySize(vocabsArr);
    Vocab *vocabs = malloc(sizeof(Vocab) * (*count));

    for (int i = 0; i < *count; i++) {
        cJSON *item = cJSON_GetArrayItem(vocabsArr, i);
        Vocab *v = &vocabs[i];

        v->vocab    = safeStrdup(cJSON_GetObjectItem(item, "vocab"));
        v->hiragana = safeStrdup(cJSON_GetObjectItem(item, "hiragana"));
        v->romaji   = safeStrdup(cJSON_GetObjectItem(item, "romaji"));
        v->meaning  = safeStrdup(cJSON_GetObjectItem(item, "meaning"));

        // Chuyển đổi sang wchar_t để dùng cho Levenshtein
        v->vocab_w = convertToWchar(v->vocab);
        v->hiragana_w = convertToWchar(v->hiragana);
        v->romaji_w = convertToWchar(v->romaji);
        v->meaning_w = convertToWchar(v->meaning);

        v->samples = parseSamples(cJSON_GetObjectItem(item, "samples"), &v->samplesCount);
    }
    return vocabs;
}

KanjiList parseJsonToStruct(const char *jsonString) {
    KanjiList data = {NULL, 0};
    cJSON *root = cJSON_Parse(jsonString);
    if (!root) {
        printf("Lỗi: Parse JSON thất bại!\n");
        return data;
    }

    data.kanjiCount = cJSON_GetArraySize(root);
    data.kanjis = malloc(sizeof(Kanji) * data.kanjiCount);

    for (int i = 0; i < data.kanjiCount; i++) {
        cJSON *item = cJSON_GetArrayItem(root, i);
        Kanji *k = &data.kanjis[i];

        k->stt        = cJSON_GetObjectItem(item, "stt") ? cJSON_GetObjectItem(item, "stt")->valueint : 0;
        k->kanji      = safeStrdup(cJSON_GetObjectItem(item, "kanji"));
        k->hanViet    = safeStrdup(cJSON_GetObjectItem(item, "hanViet"));
        k->radical    = safeStrdup(cJSON_GetObjectItem(item, "radical"));
        k->stroke     = safeStrdup(cJSON_GetObjectItem(item, "stroke"));
        k->description= safeStrdup(cJSON_GetObjectItem(item, "description"));

        k->on    = parseYomi(cJSON_GetObjectItem(item, "on"),    &k->onCount);
        k->kun   = parseYomi(cJSON_GetObjectItem(item, "kun"),   &k->kunCount);
        k->vocabs= parseVocabs(cJSON_GetObjectItem(item, "vocabs"), &k->vocabsCount);
        k->kanji_w = convertToWchar(k->kanji);
    }

    cJSON_Delete(root);
    return data;
}
