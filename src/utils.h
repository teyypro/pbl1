#ifndef UTILS_H
#define UTILS_H

#include <wchar.h>

// ==================== MÃ MÀU ANSI ====================
// Màu chính
#define CL_LOGO      "\x1b[38;5;208m"  // Cam đậm
#define CL_PRIMARY   "\x1b[38;5;75m"   // Xanh dương
#define CL_SUCCESS   "\x1b[38;5;82m"   // Xanh lá
#define CL_WARN      "\x1b[38;5;214m"  // Cam
#define CL_ERROR     "\x1b[38;5;196m"  // Đỏ
#define CL_KEY       "\x1b[38;5;111m"  // Xanh lơ
#define CL_TEXT      "\x1b[38;5;253m"  // Trắng xám
#define CL_HEADER    "\x1b[38;5;225m"  // Hồng
#define CL_DIM       "\x1b[38;5;244m"  // Xám

#define CL_RESET   "\x1b[0m"

// Màu nền
#define BG_MENU      "\x1b[48;5;236m"
#define BG_HIGHLIGHT "\x1b[48;5;236m\x1b[38;5;208m"

// Khung viền
#define CL_BORDER    "\x1b[38;5;239m"

// Hiệu ứng
#define BOLD         "\x1b[1m"
#define RESET        "\x1b[0m"

// ==================== HÀM TIỆN ÍCH ====================
wchar_t* convertToWchar(const char *source);
int isKanjiWChar(wchar_t wc);
void inputString(char *buffer, int maxLength);
void clearScreen(void);
void waitForEnter(void);
void pauseAndClear(void);
#endif