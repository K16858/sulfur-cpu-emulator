CC = gcc
CFLAGS = -Wall -g -Isrc
TARGET = cpu_emu

SRCDIR = src
SOURCES = $(SRCDIR)/main.c $(SRCDIR)/cpu.c
OBJS = main.o cpu.o

HEADERS = $(SRCDIR)/emulator.h $(SRCDIR)/cpu.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: $(SRCDIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o

.PHONY: all run clean
