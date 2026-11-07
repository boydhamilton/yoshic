


CC = g++

TARGET = yoshic

SRCS = src/main.cpp

OBJS = $(SRCS:.c=.o)

CFLAGS = -std=c++20 -O2 -Wall

BENCHMARK_TARGET = benchmark_game

BENCHMARK_SRCS = src/benchmark_game.cpp

BENCHMARK_CFLAGS = -std=c++20 -O3 -DNDEBUG -isystem src/benchmark/include -Lsrc/benchmark/build/src

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

benchmark: $(BENCHMARK_TARGET)

$(BENCHMARK_TARGET): $(BENCHMARK_SRCS) src/*.hpp
	$(CC) $(BENCHMARK_CFLAGS) $(BENCHMARK_SRCS) -lbenchmark -lpthread -o $(BENCHMARK_TARGET)

run-benchmark: $(BENCHMARK_TARGET)
	./$(BENCHMARK_TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(BENCHMARK_TARGET)

.PHONY: all clean benchmark run-benchmark
