
#pragma once

// std lib
#include<string>
#include<vector>
#include<optional>
#include<iostream>
#include<variant>

// local 
#include "tokenizer.hh"


// definitions
namespace node {

    struct ExprIntLit {
        Token int_lit;
    };

    struct ExprIdent {
        Token ident;
    };

    struct Expr{
        std::variant<node::ExprIntLit, node::ExprIdent> var;
    };

    struct StmtLet {
        Token ident;
        node::Expr expr;
    };

    struct StmtExit{
        node::Expr expr;  
    };


    struct Stmt {
        std::variant<node::StmtExit, node::StmtLet> var;
    };
    struct Program {
        std::vector<node::Stmt> statements;
    };
}


class Parser {
    public:
        inline explicit Parser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens)){

        }

        std::optional<node::Expr> parse_expr(){

            if(peek().has_value() && peek().value().type == TokenType::int_lit){
                return node::Expr{.var = node::ExprIntLit{.int_lit = consume()}} ;
            }else if(peek().has_value() && peek().value().type == TokenType::ident){
                return node::Expr{.var = node::ExprIdent{.ident = consume()}};
            }
            else{
                return {};
            }
        }

        std::optional<node::Stmt> parse_stmt(){
            if(peek().has_value() && peek().value().type == TokenType::exit
            && peek(1).has_value() ){

                consume();
                node::StmtExit stmt_exit; 
                if(auto node_expr = parse_expr() ){
                    stmt_exit = node::StmtExit{.expr = node_expr.value()};
                }
                if(peek().has_value() && peek().value().type == TokenType::semi){
                    consume();
                }else{
                    std::cerr << "Expected ';' " << std::endl;
                    exit(EXIT_FAILURE);
                }
                return node::Stmt{.var = stmt_exit};

            }else if(peek().has_value() && peek().value().type == TokenType::let 
            && peek(1).has_value() && peek(1).value().type == TokenType::ident
            && peek(2).has_value() && peek(2).value().type == TokenType::equal_sign){

                // for var decl we check first three tokens lol really fat if statement
                consume(); // consuming let
                node::StmtLet stmt_let = node::StmtLet{.ident = consume() };
                consume(); // consume eq
                if(auto node_expr = parse_expr() ){
                    stmt_let.expr = node_expr.value();
                }else{
                    std::cerr << "Invalid expr in variable assignment" << std::endl;
                    exit(EXIT_FAILURE);
                }
                if(peek().has_value() && peek().value().type == TokenType::semi){
                    consume();
                }else{
                    std::cerr << "Expected semicolon" << std::endl;
                    exit(EXIT_FAILURE);
                }
                return node::Stmt{.var = stmt_let};

            }else{
                return {};
            }
            
        }

        std::optional<node::Program> parse_program(){
            node::Program prog;
            while(peek().has_value() ){
                if(auto stmt = parse_stmt() ){
                    prog.statements.push_back(stmt.value());
                }else{
                    std::cerr << "Error parsing statement" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            return prog;
        }

    private:
        [[nodiscard]] inline std::optional<Token> peek(int ahead = 0) const { 
            if(m_index + ahead >= m_tokens.size()){
                return {};
            }else{
                return m_tokens.at(m_index + ahead);
            }
        }

        inline Token consume(){
            return m_tokens.at(m_index++);
        }
        const std::vector<Token> m_tokens;
        size_t m_index = 0; // size cause its index 

};