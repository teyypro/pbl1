#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
#endif
#include <wchar.h>
#include <utils.h>
// Hàm bổ trợ để chuyển đổi UTF-8 sang wchar_t 

// Hàm bổ trợ để chuyển đổi UTF-8 sang wchar_t 
wchar_t* convertToWchar(const char *source)
{
    if (!source || *source == '\0') 
        return NULL;
    // Dùng Win32 API - cách đáng tin cậy nhất trên Windows
    int w_len = MultiByteToWideChar(CP_UTF8, 0, source, -1, NULL, 0);
    if (w_len <= 0) 
        return NULL;

    wchar_t *w_str = (wchar_t*)malloc(w_len * sizeof(wchar_t));
    if (!w_str) 
        return NULL;

    MultiByteToWideChar(CP_UTF8, 0, source, -1, w_str, w_len);
    return w_str;

}

int isKanjiWChar(wchar_t wc) {
    return (wc >= 0x4E00 && wc <= 0x9FFF) || 
           (wc >= 0x3400 && wc <= 0x4DBF);
}

//Đọc trực tiếp utf-16 từ bàn phím rồi chuyển lại sang char* utf8
// do trên win (CMD, PowerShell, Windows Terminal) nội bộ dùng UTF-16.
void inputString(char *buffer, int maxLength) {
    if (!buffer || maxLength <= 0) return;

    // Xóa bộ đệm
    fflush(stdin);

#ifdef _WIN32
    wchar_t *wBuf = (wchar_t*)malloc(maxLength * sizeof(wchar_t));
    if (!wBuf) {
        buffer[0] = '\0';
        return;
    }

    DWORD read = 0;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);

    if (ReadConsoleW(hStdin, wBuf, maxLength - 1, &read, NULL) && read > 0) {
        // Xóa ký tự xuống dòng (\r\n hoặc \n)
        if (read >= 2 && wBuf[read-2] == L'\r' && wBuf[read-1] == L'\n')
            wBuf[read-2] = L'\0';
        else if (read >= 1 && (wBuf[read-1] == L'\n' || wBuf[read-1] == L'\r'))
            wBuf[read-1] = L'\0';
        else
            wBuf[read] = L'\0';

        // Chuyển wchar_t sang UTF-8
        WideCharToMultiByte(CP_UTF8, 0, wBuf, -1, buffer, maxLength, NULL, NULL);
    } else {
        buffer[0] = '\0';
    }

    free(wBuf);
#else
    // Linux / macOS
    if (fgets(buffer, maxLength, stdin)) {
        buffer[strcspn(buffer, "\n\r")] = '\0';
    } else {
        buffer[0] = '\0';
    }
#endif
}

void clearScreen(void) {
    //printf("\x1b[2J\x1b[H");
    system("cls");
}

void waitForEnter(void) {
    printf("\n  " CL_DIM ">> Nhấn Enter để tiếp tục..." RESET);
    getchar();
}
