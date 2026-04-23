gcc main.c src/parse_json_to_struct.c lib/cJSON.c -Ilib -Isrc -o main.exe

gcc main.c src/parse_json_to_struct.c src/exact_searching.c lib/cJSON.c -Ilib -Isrc -o main.exe