


CC = g++

TARGET = brhcc

SRCS = src/main.cpp

OBJS = $(SRCS:.c=.o)

CFLAGS = -std=c++20 -O2 -Wall

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET)

.PHONY: al
