#pragma once

// std lib
#include<string>
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
    equal_sign,
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
    lessthan
};

typedef struct {
	TokenType type;
	std::optional<std::string> val;
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
        inline explicit Tokenizer(const std::string& src)
           : m_src(std::move(src))
           {
                // constructor body empty as all our work (init members list) is done in decl
           }

           inline std::vector<Token> tokenize() {

                std::vector<Token> tokens;
                std::string buf;

                while(peek().has_value()){
                    if(std::isalpha(peek().value())){
                        buf.push_back(consume());
                        while(peek().has_value() && std::isalnum(peek().value())){
                            buf.push_back(consume());
                        }
                        if(buf == "exit"){
                            tokens.push_back({.type = TokenType::exit});
                            buf.clear();
                            continue;
                        }else if(buf == "let"){
                            tokens.push_back({.type = TokenType::let});
                            buf.clear();
                            continue;
                        }else if(buf == "if"){
                            tokens.push_back({.type = TokenType::if_kw});
                            buf.clear();
                            continue;
                        }else if(buf == "while"){
                            tokens.push_back({.type = TokenType::while_kw});
                            buf.clear();
                            continue;
                        }
                        else{
                            tokens.push_back({.type = TokenType::ident, .val = buf});
                            buf.clear();
                        }
                        
                        // else{
                        //     std::cerr << "Error in keyword tokenization" << std::endl;
                        //     std::cout << buf <<" | "<< buf.length() << std::endl;
                        //     exit(EXIT_FAILURE);
                        // }

                    }else if(std::isdigit(peek().value()) ){
                        buf.push_back(consume());
                        while(peek().has_value() && std::isdigit(peek().value() )){
                            buf.push_back(consume());
                        }
                        tokens.push_back({ .type= TokenType::int_lit, .val = buf});
                        buf.clear();
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
                        tokens.push_back( {.type = TokenType::equal_sign});
                        continue;
                    
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
                        tokens.push_back({.type = TokenType::exclaim});
                        continue;
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

        const std::string m_src;
        size_t m_index = 0;
};