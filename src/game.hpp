#pragma once

// std
#include <string>
#include <vector>
#include <optional>
#include <fstream>
#include <sstream>

// local
#include "arena.hpp"
#include "tokenizer.hpp"
#include "parser.hpp"
#include "generation.hpp"

inline std::string read_file(const std::string& path)
{
    std::ifstream in(path, std::ios::in);
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

inline std::vector<Token> run_tokenizer(const std::string& input)
{
    Tokenizer tokenizer(input);
    return tokenizer.tokenize();
}

// node::Program is just a vector of pointers into the Parser's
// ArenaAllocator - the AST is only valid as long as that arena is alive.
// Returning node::Program alone from a free function is a dangling-pointer
// trap: the Parser (and its arena) is destroyed at the end of the function,
// so a caller reading the "returned" AST afterwards reads freed memory.
// Bundling the Parser alongside the Program keeps the arena alive for as
// long as the caller holds onto the result.
struct ParseResult {
    node::Program program;
    Parser parser; // owns the arena `program`'s pointers live in - keep alive
};

inline ParseResult run_parser(std::vector<Token> tokens)
{
    Parser parser(std::move(tokens));
    auto ast = parser.parse_program();

    if (!ast.has_value())
        throw std::runtime_error("parser failure");

    return ParseResult{ast.value(), std::move(parser)};
}

inline std::string run_generator(const node::Program& program)
{
    Generator generator(program);
    return generator.generate_asm_x86();
}

inline std::string run_full_pipeline(const std::string& input)
{
    auto tokens = run_tokenizer(input);
    auto parsed = run_parser(std::move(tokens));
    return run_generator(parsed.program);
}
