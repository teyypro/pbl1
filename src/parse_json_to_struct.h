#ifndef PARSE_JSON_TO_STRUCT_H
#define PARSE_JSON_TO_STRUCT_H
#include "../data_structures.h"
#include "../lib/cJSON.h"

wchar_t* convertToWchar(const char *source);
char* readFileToString(const char *fileName);
void configUTF8();
char* safeStrdup(cJSON *item);
Yomi* parseYomi(cJSON *yomiArr, int *count);
Sample* parseSamples(cJSON *samplesArr, int *count);
Vocab* parseVocabs(cJSON *vocabsArr, int *count);
KanjiList parseJsonToStruct(const char *jsonString);


#endif