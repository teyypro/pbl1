#include <windows.h>
#include <wchar.h>

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