@echo off
echo Compiling project...
gcc main.c ^
    src/parse_json_to_struct.c ^
    src/vocab_search_exact.c ^
    src/kanji_search_exact.c ^
    src/search_fuzzy.c ^
    src/lessons_management.c ^
    src/dashboard.c ^
    lib/cJSON.c ^
    -Ilib -Isrc -o main.exe

if %ERRORLEVEL% neq 0 (
    echo Compilation failed!
    exit /b %ERRORLEVEL%
)

echo Build successful. Run with: .\main.exe
