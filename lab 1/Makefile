CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99 -ggdb3

TARGET = main.out
SRCS = main.c sp_1.c
OBJS = $(SRCS:.c=.o)
HEADERS = sp_1.h const_sp_1.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

main.o: main.c $(HEADERS)
	$(CC) $(CFLAGS) -c main.c -o main.o

sp_1.o: sp_1.c sp_1.h
	$(CC) $(CFLAGS) -c sp_1.c -o sp_1.o

clean:
	rm -f $(OBJS)

.PHONY: all clean



