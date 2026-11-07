#include <benchmark/benchmark.h>
#include <cstddef>
#include <sstream>
#include <string>

#include "game.hpp"

// ---- fixed sample programs ---------------------------------------------
// Read once at static-init time so file I/O never shows up inside a
// measured iteration. Paths are relative to the repo root, matching how
// the rest of the project (main.cpp, bench.bash) is invoked from.
static const std::string kWhileProgram = read_file("while.ys"); // loop + branches
static const std::string kHelloProgram = read_file("hello.ys"); // small loop
static const std::string kEqProgram = read_file("eq.ys");       // straight-line + branch

// ---- synthetic programs of controlled size ------------------------------
// A straight-line program with `n` statements, so the scaling benchmarks
// below can vary input size independently of any one hand-written sample.
static std::string make_synthetic_program(std::size_t n)
{
    std::ostringstream out;
    out << "let x = 0;\n";
    for (std::size_t i = 0; i < n; ++i) {
        out << "x = x + 1;\n";
    }
    out << "exit x;\n";
    return out.str();
}

// ---- per-stage benchmarks, fixed input -----------------------------------

static void BM_Tokenizer(benchmark::State& state)
{
    for (auto _ : state) {
        auto tokens = run_tokenizer(kWhileProgram);
        benchmark::DoNotOptimize(tokens);
    }
}
BENCHMARK(BM_Tokenizer);

static void BM_Parser(benchmark::State& state)
{
    auto tokens = run_tokenizer(kWhileProgram);
    for (auto _ : state) {
        auto tokens_copy = tokens; // run_parser consumes (moves) its argument
        auto parsed = run_parser(std::move(tokens_copy));
        benchmark::DoNotOptimize(parsed.program);
    }
}
BENCHMARK(BM_Parser);

static void BM_Generator(benchmark::State& state)
{
    auto tokens = run_tokenizer(kWhileProgram);
    auto parsed = run_parser(std::move(tokens)); // kept alive: owns the arena `program` points into
    for (auto _ : state) {
        auto asm_code = run_generator(parsed.program);
        benchmark::DoNotOptimize(asm_code);
    }
}
BENCHMARK(BM_Generator);

static void BM_FullPipeline(benchmark::State& state)
{
    for (auto _ : state) {
        auto asm_code = run_full_pipeline(kWhileProgram);
        benchmark::DoNotOptimize(asm_code);
    }
}
BENCHMARK(BM_FullPipeline);

// ---- full pipeline across every hand-written sample ----------------------

static void BM_FullPipeline_Sample(benchmark::State& state, const std::string& source)
{
    for (auto _ : state) {
        auto asm_code = run_full_pipeline(source);
        benchmark::DoNotOptimize(asm_code);
    }
}
BENCHMARK_CAPTURE(BM_FullPipeline_Sample, while_ys, kWhileProgram);
BENCHMARK_CAPTURE(BM_FullPipeline_Sample, hello_ys, kHelloProgram);
BENCHMARK_CAPTURE(BM_FullPipeline_Sample, eq_ys, kEqProgram);

// ---- scaling: how does each stage behave as program size grows? ----------
// ->Complexity() has google benchmark fit Time vs N to O(1)/O(N)/O(N^2)/...
// and report which fits best - useful for catching an accidentally
// quadratic pass (e.g. an O(N) token vector copy done once per statement).
//
// Parser::m_allocator is a fixed 4MB arena (see arena.hpp) that isn't
// checked for exhaustion before use: ArenaAllocator::alloc<T>() returns
// nullptr once it's full, and the parser dereferences that pointer
// unconditionally, so overrunning it segfaults instead of failing
// cleanly. Empirically, this codegen pattern (~1 statement/line) exhausts
// the arena somewhere between 14k and 16k statements. kArenaSafeStmtCap
// keeps every arena-touching scaling benchmark well clear of that so the
// suite runs reliably; it's a real limitation of the compiler, not
// something to silently work around, so it's called out here rather than
// just picking a "safe-looking" number.
static constexpr std::size_t kArenaSafeStmtCap = 1 << 13; // 8192, ~2x margin below the observed crash point

static void BM_Tokenizer_Scaling(benchmark::State& state)
{
    auto source = make_synthetic_program(static_cast<std::size_t>(state.range(0)));
    for (auto _ : state) {
        auto tokens = run_tokenizer(source);
        benchmark::DoNotOptimize(tokens);
    }
    state.SetComplexityN(state.range(0));
}
// tokenizing never touches the arena, so it isn't bound by kArenaSafeStmtCap
BENCHMARK(BM_Tokenizer_Scaling)->RangeMultiplier(4)->Range(1 << 4, 1 << 14)->Complexity();

static void BM_Parser_Scaling(benchmark::State& state)
{
    auto source = make_synthetic_program(static_cast<std::size_t>(state.range(0)));
    auto tokens = run_tokenizer(source);
    for (auto _ : state) {
        auto tokens_copy = tokens;
        auto parsed = run_parser(std::move(tokens_copy));
        benchmark::DoNotOptimize(parsed.program);
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_Parser_Scaling)->RangeMultiplier(4)->Range(1 << 4, kArenaSafeStmtCap)->Complexity();

static void BM_Generator_Scaling(benchmark::State& state)
{
    auto source = make_synthetic_program(static_cast<std::size_t>(state.range(0)));
    auto tokens = run_tokenizer(source);
    auto parsed = run_parser(std::move(tokens));
    for (auto _ : state) {
        auto asm_code = run_generator(parsed.program);
        benchmark::DoNotOptimize(asm_code);
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_Generator_Scaling)->RangeMultiplier(4)->Range(1 << 4, kArenaSafeStmtCap)->Complexity();

static void BM_FullPipeline_Scaling(benchmark::State& state)
{
    auto source = make_synthetic_program(static_cast<std::size_t>(state.range(0)));
    for (auto _ : state) {
        auto asm_code = run_full_pipeline(source);
        benchmark::DoNotOptimize(asm_code);
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_FullPipeline_Scaling)->RangeMultiplier(4)->Range(1 << 4, kArenaSafeStmtCap)->Complexity();

BENCHMARK_MAIN();
