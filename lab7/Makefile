CC = gcc
CFLAGS = -D_BSD_SOURCE -pedantic -std=c99 -ggdb3 -D_DEFAULT_SOURCE

TARGET = m_ls.out
SRCS = m_ls.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

m_ls.o: m_ls.c
	$(CC) $(CFLAGS) -c m_ls.c -o m_ls.o

clean:
	rm -f $(OBJS)

.PHONY: all clean



