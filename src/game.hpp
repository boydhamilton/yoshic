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

inline node::Program run_parser(std::vector<Token> tokens, ArenaAllocator& arena)
{
    Parser parser(std::move(tokens), arena);
    auto ast = parser.parse_program();

    if (!ast.has_value())
        throw std::runtime_error("parser failure");

    return ast.value();
}

inline std::string run_generator(const node::Program& program)
{
    Generator generator(program);
    return generator.generate_asm_x86();
}

inline std::string run_full_pipeline(const std::string& input)
{
    ArenaAllocator arena(4 * 1024 * 1024);
    auto tokens = run_tokenizer(input);
    auto ast = run_parser(std::move(tokens), arena);
    return run_generator(ast);
}