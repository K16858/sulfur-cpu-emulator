# 変数の定義
CC = gcc
CFLAGS = -Wall -g
TARGET = main

all: $(TARGET)

$(TARGET): main.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o
