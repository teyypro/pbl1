# TEYYPRO-PBL1 - Japanese Kanji Learning System

## 📚 Overview

**PBL1** is a comprehensive command-line based Japanese learning tool designed to help learners master Kanji and vocabulary through various interactive features. The system provides multiple search methods, quiz modes, and analysis tools to enhance the learning experience.

## ✨ Features

### 🔍 Multi-Method Search System
- **Exact Match**: Precise search for Kanji and vocabulary
- **Pattern Match (KMP)**: Substring search using the Knuth-Morris-Pratt algorithm
- **Fuzzy Match (Levenshtein)**: Approximate string matching to find similar words

### 📖 Lesson Management
- Organized by lessons with 16 Kanji per lesson
- Detailed view of each Kanji with readings, meanings, and related vocabulary
- Track learning progress across lessons

### 🎯 Interactive Quizzes
- **Multiple Choice Questions** for both Kanji and Vocabulary
- Customizable question and answer types (Kanji, Hán Việt, Furigana, Romaji, Meaning)
- Score tracking and immediate feedback

### 🔍 Sentence Analysis
- Analyze Japanese sentences to identify and display known Kanji
- Shows detailed information for each recognized Kanji
- Vocabulary extraction and meaning display

### 📝 Filter Learned Vocabulary
- Filter vocabulary based on learned lessons
- Display words containing specific Kanji from selected lessons
- Track which vocabulary you've mastered

## 🛠️ Technology Stack

- **Language**: C
- **JSON Parsing**: cJSON library
- **Algorithms**:
  - Knuth-Morris-Pratt (KMP) for substring search
  - Levenshtein distance for fuzzy matching
  - Hash tables for efficient exact search
- **Character Encoding**: UTF-8 with Wide Character support for Unicode

## 🚀 Getting Started

### Prerequisites

- GCC compiler
- Windows (or WSL) for build script, Linux with make support

### Installation

1. Clone the repository:
```bash
git clone https://github.com/yourusername/teyypro-pbl1.git
cd teyypro-pbl1
```

2. Build the project:
```bash
build.bat
```

3. Run the application:
```bash
.\main.exe
```

### Quick Start

The application provides a simple menu-driven interface:

```
======== BẢNG ĐIỀU KHIỂN ========
[1]. Tìm kiếm từ điển (Chính xác / KMP / Tìm mờ)
[2]. Quản lý từ điển cá nhân
[3]. Lọc từ vựng đã học
[4]. Phân tích câu
[5]. Bài tập
[6]. Ôn tập từ sai
[7]. Kanji họ hàng (Bộ thủ)
[0]. Thoát chương trình
```

## 📁 Project Structure

```
teyypro-pbl1/
├── build.bat                    # Build script
├── main.c                       # Main entry point
├── data_structures.h            # Core data structures
├── data/
│   └── data.json               # Kanji data file
├── lib/
│   └── cJSON.h                 # JSON parsing library
└── src/
    ├── dashboard.c/h           # Main menu controller
    ├── parse_json_to_struct.c/h # JSON to struct conversion
    ├── vocab_search_exact.c/h   # Exact vocabulary search
    ├── kanji_search_exact.c/h   # Exact Kanji search
    ├── substring_search.c/h     # KMP pattern search
    ├── fuzzy_search.c/h         # Levenshtein fuzzy search
    ├── lessons_management.c/h   # Lesson management
    ├── multiple_choice.c/h      # Quiz system
    ├── filter_learned_lesson.c/h # Learned vocabulary filter
    ├── sentence_analysis.c/h    # Sentence analyzer
    └── utils.c/h               # Utility functions
```

## 🎮 Feature Walkthrough

### 1. Search System
- **Exact Search**: Perfect match for Kanji, Hán Việt, or vocabulary
- **Pattern Search**: Find words containing your search string
- **Fuzzy Search**: Find similar words with typo tolerance (Levenshtein distance ≤ 2)

### 2. Lesson Management
- Browse lessons with Kanji display
- View detailed Kanji information including:
  - On/Kun readings
  - Hán Việt translation
  - Stroke count and radical
  - Related vocabulary with examples

### 3. Quiz System
- Configure question and answer types independently
- Choose between Kanji or Vocabulary quizzes
- Track performance with real-time scoring

### 4. Sentence Analysis
- Input any Japanese sentence
- Automatically identifies and displays known Kanji
- Shows readings and meanings for each Kanji

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the project
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📝 License

This project is open source and available under the MIT License.

## 🙏 Acknowledgments

- cJSON library for JSON parsing
- All contributors and users of this project

---

**Made with ❤️ for Japanese learners**