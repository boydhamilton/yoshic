#pragma once

// std lib
#include<string>
#include<string_view>
#include<vector>
#include<optional>
#include<iostream>


// definitions
enum class TokenType {
	exit,
	int_lit,
	semi,
    open_paren,
    close_paren,
    ident,
    let,
    equal_assign,  // =
    plus,
    multi,
    sub,
    div,
    open_curly,
    close_curly,
    if_kw,
    while_kw,
    exclaim,
    greaterthan,
    lessthan,
    equal_cmp, // ==
    nequal_cmp // !=
};

typedef struct {
	TokenType type;
	// view into the *original source buffer* the Tokenizer was built with -
	// only valid as long as that buffer is alive. Only ident/int_lit ever
	// set this; every other token type leaves it default-constructed
	// (empty), which doubles as the "no value" case since the tokenizer
	// never produces an ident/int_lit with zero-length text.
	std::string_view val;
} Token;

bool is_bin_op(TokenType type){
    switch(type){
        case TokenType::plus:
        case TokenType::multi:
            return true;
        default:
            return false;
    }
}

std::optional<int> bin_prec(TokenType type){
    switch(type){
        case TokenType::greaterthan:
        case TokenType::lessthan:
        case TokenType::equal_cmp:
        case TokenType::nequal_cmp:
            return -1;
        case TokenType::sub:
        case TokenType::plus:
            return 0;
        case TokenType::div:
        case TokenType::multi:
            return 1;
        default:
            return {};
    }
}

class Tokenizer {

    public:
        // takes the source by reference rather than owning a copy: token
        // values are string_views into this buffer, so the caller must
        // keep it alive at least as long as the resulting tokens (and, in
        // turn, the AST/asm built from them) are used - the rest of the
        // pipeline already holds the source string open that long (see
        // main.cpp's `contents`, game.hpp's `input`/`kWhileProgram`).
        inline explicit Tokenizer(const std::string& src)
           : m_src(src)
           {
           }

           inline std::vector<Token> tokenize() {

                std::vector<Token> tokens;

                while(peek().has_value()){
                    if(std::isalpha(peek().value())){
                        std::size_t start = m_index;
                        consume();
                        while(peek().has_value() && std::isalnum(peek().value())){
                            consume();
                        }
                        std::string_view word(m_src.data() + start, m_index - start);

                        if(word == "exit"){
                            tokens.push_back({.type = TokenType::exit});
                        }else if(word == "let"){
                            tokens.push_back({.type = TokenType::let});
                        }else if(word == "if"){
                            tokens.push_back({.type = TokenType::if_kw});
                        }else if(word == "while"){
                            tokens.push_back({.type = TokenType::while_kw});
                        }
                        else{
                            tokens.push_back({.type = TokenType::ident, .val = word});
                        }
                        continue;

                    }else if(std::isdigit(peek().value()) ){
                        std::size_t start = m_index;
                        consume();
                        while(peek().has_value() && std::isdigit(peek().value() )){
                            consume();
                        }
                        std::string_view number(m_src.data() + start, m_index - start);
                        tokens.push_back({ .type= TokenType::int_lit, .val = number});
                        continue;

                    }else if(peek().value() == '('){
                        consume();
                        tokens.push_back( {.type = TokenType::open_paren} );
                        continue;
                    }else if(peek().value() == ')'){
                        consume();
                        tokens.push_back( {.type = TokenType::close_paren });
                        continue;
                    }else if(peek().value() == ';'){
                        consume(); // make sure you consume every token so you move through the program properly. semi lowk inconsequential
                        tokens.push_back( {.type=TokenType::semi} );
                        continue;
                    }else if(peek().value() == '='){
                        consume();
                        if(peek().has_value() && peek().value() == '='){
                            consume();
                            tokens.push_back( {.type = TokenType::equal_cmp});
                            continue;
                        }else{
                            tokens.push_back( {.type = TokenType::equal_assign});
                            continue;
                        }

                    }else if(peek().value() == '+'){
                        consume();
                        tokens.push_back( {.type = TokenType::plus});
                        continue;
                    }else if(peek().value() == '*'){
                        consume();
                        tokens.push_back({.type = TokenType::multi});
                        continue;
                    }else if(peek().value() == '-'){
                        consume();
                        tokens.push_back({.type = TokenType::sub});
                        continue;
                    }else if(peek().value() == '/'){
                        consume();
                        tokens.push_back({.type = TokenType::div});
                        continue;
                    }else if(peek().value() == '{'){
                        consume();
                        tokens.push_back({.type = TokenType::open_curly});
                        continue;
                    }else if(peek().value() == '}'){
                        consume();
                        tokens.push_back({.type = TokenType::close_curly});
                        continue;
                    }else if(peek().value() == '!'){
                        consume();
                        if(peek().has_value() && peek().value() == '='){
                            consume();
                            tokens.push_back({.type = TokenType::nequal_cmp});
                            continue;
                        }else{
                            tokens.push_back({.type = TokenType::exclaim});
                            continue;
                        }
                    }else if(peek().value() == '>'){
                        consume();
                        tokens.push_back({.type = TokenType::greaterthan});
                        continue;
                    }else if(peek().value() == '<'){
                        consume();
                        tokens.push_back({.type = TokenType::lessthan});
                        continue;
                    }
                    else if(isspace(peek().value())){
                        consume();
                        continue;
                    }else{
                        std::cerr << "Error in typewise tokenization" << std::endl;
                        exit(EXIT_FAILURE);
                    }
                }
                m_index = 0; // reset
                return tokens;

           }

    private:

        [[nodiscard]] inline std::optional<char> peek(int ahead = 0) const {
            /*
            SUPER interesting method def, so let's break it down as this is for learning:
            nodiscard means we get compilation warning if we don't use returned value.
            ahead = 1 means it'll default to 1, but you can overload to specify how many values you want to peek ahead
            the const that follows means this is a CONSTANT method, it cant alter fields in the class
            */
            if(m_index + ahead >= m_src.length()){
                return {};
            }else{
                return m_src.at(m_index + ahead);
            }
        }

        inline char consume(){
            return m_src.at(m_index++);
        }

        const std::string& m_src;
        size_t m_index = 0;
};
