#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
#endif

#include "parse_json_to_struct.h"
#include "data_structures.h"
#include "dashboard.h"



int main() {
    configUTF8();

    const char *fileAddress = "data/kanjiData.json";
    char *rawJson = readFileToString(fileAddress);

    if (!rawJson) {
        printf("Không tìm th?y file: %s\n", fileAddress);
        return 1;
    }

    handleMenuSelection(rawJson);

    free(rawJson);
    return 0;
}