#ifndef UTILS_H
#define UTILS_H

#include <wchar.h>

// ==================== MÃ MÀU ANSI TRUE COLOR (24-BIT) ====================

// Màu chữ (Foreground)
// ==================== Bổ sung vào utils.h ====================
/// ==================================================================
// 1. MÀU CHỦ ĐẠO & HỆ THỐNG (GIỮ NGUYÊN HOÀN TOÀN THEO YÊU CẦU)
// ==================================================================
#define CL_PRIMARY   "\x1b[38;2;137;234;254m" // Xanh Cyan chủ đạo: Mang lại cảm giác công nghệ, logic và sự tập trung (#89eafe)
#define CL_KEY       "\x1b[38;2;137;234;254m" 
#define CL_LOGO      "\x1b[38;2;137;234;254m" 
#define CL_TEXT      "\x1b[38;2;230;230;230m" // Trắng xám nhẹ: Giảm độ chói so với trắng tinh, chống mỏi mắt (#e6e6e6)
#define CL_DIM       "\x1b[38;2;110;110;110m" // Xám trầm: Hạ thấp mức độ chú ý cho các thành phần phụ, ký tự phân tách (#6e6e6e)

// ==================================================================
// 2. MÀU TÍN HIỆU UX / CẢNH BÁO / TRẠNG THÁI
// ==================================================================
#define CL_WARN      "\x1b[38;2;255;212;102m" // Vàng hổ phách: Kích thích sự chú ý vừa phải, không gây ức chế thị giác (#ffd466)
#define CL_HEADER    "\x1b[38;2;255;212;102m" 
#define CL_ERROR     "\x1b[38;2;255;107;107m" // Đỏ San Hô: Tín hiệu lỗi rõ ràng, thúc đẩy phản xạ nhận biết tức thì (#ff6b6b)
#define CL_SUCCESS   "\x1b[38;2;114;242;151m" // Xanh Ngọc Mint: Biểu thị sự chính xác, an toàn, giải tỏa áp lực tâm lý (#72f297)

// ==================================================================
// 3. MÀU CHUYÊN BIỆT CHO NGÔN NGỮ (TỐI ƯU GHI NHỚ THỊ GIÁC)
// ==================================================================
// Kanji là phần khó nhất, sử dụng sắc đỏ hoàng gia/hồng đậm để kích hoạt vùng ghi nhớ sâu của não bộ
#define CL_KANJI     "\x1b[38;2;255;92;141m"  // Hồng Đậm Neon: Điểm neo thị giác mạnh giúp tập trung vào chữ Kanji gốc (#ff5c8d)

// Kana & Romaji đóng vai trò bắc cầu âm thanh, dùng màu trung tính ấm/lạnh dịu hơn
#define CL_KANA      "\x1b[38;2;255;160;122m" // Cam Thạch Anh (Salmon): Thúc đẩy liên kết tư duy đọc nhanh (#ffa07a)
#define CL_ROMAJI    "\x1b[38;2;147;226;214m" // Xanh Sage: Màu lạnh dịu, làm mượt quá trình đọc ký tự Latin (#93e2d6)

// Ý nghĩa từ vựng tiếng Việt là đích đến của thông tin bản xứ, sử dụng sắc màu tinh tế
#define CL_MEANING   "\x1b[38;2;194;161;255m" // Tím Hoa Oải Hương: Sang trọng, hỗ trợ phân loại nghĩa rất tốt (#c2a1ff)

// Phân tách câu ví dụ: Sử dụng nhịp độ màu từ Lạnh sang Ấm để tạo khoảng nghỉ cho mắt
#define CL_JP_SENT   "\x1b[38;2;117;213;253m" // Xanh Bầu Trời: Màu thư giãn, giúp đọc chuỗi ký tự Nhật dài không bị rối (#75d5fd)
#define CL_VN_SENT   "\x1b[38;2;244;241;134m" // Vàng Chanh Nhạt: Sắc ấm dịu, hỗ trợ đọc hiểu phần chuyển ngữ tự nhiên (#f4f186)

// ==================================================================
// 4. ĐỊNH DẠNG KHUNG VIỀN & NỀN (STRUCTURE BACKGROUND)
// ==================================================================
// Thay thế màu xám thô bằng màu xanh xám Slate để bao bọc cấu trúc dashboard chắc chắn và hiện đại hơn
#define CL_BORDER    "\x1b[38;2;82;104;133m"  // Xanh Slate Tối: Đóng khung dữ liệu gọn gàng, tăng chiều sâu (#526885)

#define BG_PANEL     "\x1b[48;2;42;44;54m"    // Nền tối Dracula-hybrid: Hấp thụ ánh sáng chói, bảo vệ võng mạc (#2a2c36)
#define BG_MENU      "\x1b[48;2;42;44;54m"    
#define BG_HIGHLIGHT "\x1b[48;2;137;234;254m\x1b[38;2;42;44;54m" // Nền xanh chủ đạo, chữ tối tương phản cao

// ==================================================================
// 5. HIỆU ỨNG HỆ THỐNG
// ==================================================================
#define BOLD         "\x1b[1m"
#define CL_RESET     "\x1b[0m"
#define RESET        "\x1b[0m"

// ==================== HÀM TIỆN ÍCH ====================
wchar_t* convertToWchar(const char *source);
int isKanjiWChar(wchar_t wc);
void inputString(char *buffer, int maxLength);
void clearScreen(void);
void waitForEnter(void);
#endif