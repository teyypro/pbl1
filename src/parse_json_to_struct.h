#ifndef PARSE_JSON_TO_STRUCT_H
#define PARSE_JSON_TO_STRUCT_H
#include "../data_structures.h"
#include "../lib/cJSON.h"

wchar_t* convertToWchar(const char *source);
char* readFileToString(const char *fileName);
void configUTF8();
KanjiList parseJsonToStruct(const char *jsonString);

#endif