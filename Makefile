# 変数の定義
CC = gcc
CFLAGS = -Wall -g
TARGET = cpu_emu

OBJS = main.o cpu.o
HEADERS = emulator.h cpu.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c $(HEADERS)
	$(CC) $(CFLAGS) -c main.c

cpu.o: cpu.c $(HEADERS)
	$(CC) $(CFLAGS) -c cpu.c

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o

.PHONY: all run clean
