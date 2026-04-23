# Tên trình biên dịch
CC = gcc

# Các cờ biên dịch (Warnings và Include paths)
CFLAGS = -Wall -Ilib -Isrc

# Tên file thực thi đầu ra
TARGET = main.exe

# Danh sách các tệp nguồn (.c)
SRCS = main.c \
       src/parse_json_to_struct.c \
       src/exact_searching.c \
       lib/cJSON.c

# Chuyển đổi danh sách .c thành .o (object files)
OBJS = $(SRCS:.c=.o)

# Quy tắc mặc định: Biên dịch chương trình
all: $(TARGET)

# Quy tắc tạo ra file thực thi từ các file object
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Quy tắc biên dịch các file .c thành .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Quy tắc dọn dẹp các file rác sau khi biên dịch
clean:
	del /q $(OBJS) $(TARGET)

# Quy tắc chạy chương trình
run: all
	./$(TARGET)