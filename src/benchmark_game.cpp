#include <benchmark/benchmark.h>
#include <iostream>

#include "game.hpp"

// eliminate freading overhead
static std::string test_input = read_file("while.ys");


static void BM_Tokenizer(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto tokens = run_tokenizer(test_input);
        benchmark::DoNotOptimize(tokens);
    }
}
BENCHMARK(BM_Tokenizer);


static void BM_Parser(benchmark::State& state)
{
    auto tokens = run_tokenizer(test_input);
    for (auto _ : state)
    {
        ArenaAllocator arena(4 * 1024 * 1024);
        auto ast = run_parser(tokens, arena);
        benchmark::DoNotOptimize(ast);
    }
}
BENCHMARK(BM_Parser);


static void BM_Generator(benchmark::State& state)
{
    auto arena = ArenaAllocator(4 * 1024 * 1024);
    auto tokens = run_tokenizer(test_input);
    auto ast = run_parser(std::move(tokens), arena);
    for (auto _ : state)
    {
        auto asm_code = run_generator(ast);
        benchmark::DoNotOptimize(asm_code);
    }
}
BENCHMARK(BM_Generator);



static void BM_FullPipeline(benchmark::State& state)
{
    for (auto _ : state)
    {
        ArenaAllocator arena(4 * 1024 * 1024);

        auto tokens = run_tokenizer(test_input);
        auto ast = run_parser(tokens, arena);
        auto asm_code = run_generator(ast);

        benchmark::DoNotOptimize(asm_code);
    }
}
BENCHMARK(BM_FullPipeline);


BENCHMARK_MAIN();