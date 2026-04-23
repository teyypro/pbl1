#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
#endif

#include "parse_json_to_struct.h"
#include "exact_searching.h"
#include "data_structures.h"




int main() {
    configUTF8();

    const char *fileAddress = "data/kanjiData.json";
    char *rawJson = readFileToString(fileAddress);

    if (!rawJson) {
        printf("Không tìm th?y file: %s\n", fileAddress);
        return 1;
    }

    printf("Debug: Đang parse JSON...\n");
    KanjiList myData = parseJsonToStruct(rawJson);

    printf("Debug: Đang tạo Hash Table...\n");
    HashTable *vocabHT = createHashTable(HASH_TABLE_SIZE);
    
    printf("Debug: Đang nạp dữ liệu vào Hash Table...\n");
    buildHashTableForVocab(&myData, vocabHT);

    printf("Debug: Đang tìm kiếm...\n");
    exactlySearching(vocabHT, "学生");

    free(rawJson);
    return 0;
}