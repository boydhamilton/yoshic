# g++ -std=c++17 \
#     src/benchmark_game.cpp \
#     -isystem benchmark/include \
#     -Lbenchmark/build/src \
#     -lbenchmark -lpthread \
#     -O3 -DNDEBUG \
#     -o benchmark_game && 
g++ -fsanitize=address -g -O1 -fno-omit-frame-pointer \
    src/benchmark_game.cpp -lbenchmark -lpthread -o benchmark_game &&
./benchmark_game