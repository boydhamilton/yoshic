
#pragma once

// std lib
#include<string>
#include<vector>
#include<optional>
#include<iostream>
#include<variant>

// local 
#include "tokenizer.hpp"
#include "arena.hpp"


// definitions
namespace node {

    struct TermIntLit {
        Token int_lit;
    };

    struct TermIdent {
        Token ident;
    };

    struct Expr;
    struct BinExprAdd {
        node::Expr* lhs;
        node::Expr* rhs;
    };

    // struct BinExprMulti{
    //     node::Expr* lhs;
    //     node::Expr* rhs;
    // };

    struct BinExpr{
        // std::variant<node::BinExprAdd*, node::BinExprMulti*> var;
        node::BinExprAdd* var;
    };


    struct Term{
        std::variant<node::TermIntLit*, node::TermIdent*> var;
    };

    struct Expr{
        std::variant<node::Term*, node::BinExpr*> var;
    };

    struct StmtLet {
        Token ident;
        node::Expr* expr;
    };

    struct StmtExit{
        node::Expr* expr;  
    };


    struct Stmt {
        std::variant<node::StmtExit*, node::StmtLet*> var;
    };
    struct Program {
        std::vector<node::Stmt*> statements;
    };
}


class Parser {
    public:
        inline explicit Parser(std::vector<Token> tokens)
            : m_tokens(std::move(tokens))
            , m_allocator(1024 * 1024 * 4) // 4mb 
        {

        }

        

        std::optional<node::Term*> parse_term(){
             if(peek().has_value() && peek().value().type == TokenType::int_lit){

                auto expr_int_lit = m_allocator.alloc<node::TermIntLit>();
                expr_int_lit->int_lit = consume(); // structure is implicit in allocation
                
                auto term = m_allocator.alloc<node::Term>();
                term->var = expr_int_lit;
                return term;

            }else if(peek().has_value() && peek().value().type == TokenType::ident){
                auto expr_ident = m_allocator.alloc<node::TermIdent>();
                expr_ident->ident = consume();
                
                auto term = m_allocator.alloc<node::Term>();
                term->var = expr_ident;
                return term;
            }
        }

        std::optional<node::Expr*> parse_expr(){

            if(auto term = parse_term()){
                if(peek().has_value() && peek().value().type == TokenType::plus){
                    auto bin_expr = m_allocator.alloc<node::BinExpr>();
                    
                    consume(); // consume + 
                    auto bin_expr_add = m_allocator.alloc<node::BinExprAdd>();
                    auto lhs = m_allocator.alloc<node::Expr>();
                    lhs->var = term.value();
                    bin_expr_add->lhs = lhs;

                    if(auto rhs = parse_expr()){
                        bin_expr_add->rhs = rhs.value();
                        bin_expr->var = bin_expr_add;
                        // this whole thing is gross but wtv fix it later
                        auto expr = m_allocator.alloc<node::Expr>();
                        expr->var = bin_expr;
                        return expr;
                    }else{
                        std::cerr << "Expected rhs" << std::endl;
                    }
                    
                }else{
                    auto expr = m_allocator.alloc<node::Expr>();
                    expr->var = term.value();
                    return expr;
                }
            }else{
                return {};
            }

           
        }

        std::optional<node::Stmt*> parse_stmt(){
            if(peek().has_value() && peek().value().type == TokenType::exit
            && peek(1).has_value() ){

                consume();
                auto stmt_exit = m_allocator.alloc<node::StmtExit>();
                
                if(auto node_expr = parse_expr() ){
                    stmt_exit->expr=node_expr.value();
                }
                if(peek().has_value() && peek().value().type == TokenType::semi){
                    consume();
                }else{
                    std::cerr << "Expected ';' in statement exit" << std::endl;
                    exit(EXIT_FAILURE);
                }
                auto stmt = m_allocator.alloc<node::Stmt>();
                stmt->var = stmt_exit;
                
                return stmt;

            }else if(peek().has_value() && peek().value().type == TokenType::let 
            && peek(1).has_value() && peek(1).value().type == TokenType::ident
            && peek(2).has_value() && peek(2).value().type == TokenType::equal_sign){

                // for var decl we check first three tokens lol really fat if statement
                consume(); // consuming let
                auto stmt_let = m_allocator.alloc<node::StmtLet>();
                stmt_let->ident = consume();
                consume(); // consume eq
                if(auto node_expr = parse_expr() ){
                    stmt_let->expr = node_expr.value();
                }else{
                    std::cerr << "Invalid expr in statement let" << std::endl;
                    exit(EXIT_FAILURE);
                }
                if(peek().has_value() && peek().value().type == TokenType::semi){
                    consume();
                }else{
                    std::cerr << "Expected ';' in statement let" << std::endl;
                    exit(EXIT_FAILURE);
                }
                auto stmt = m_allocator.alloc<node::Stmt>();
                stmt->var = stmt_let;
                return stmt;

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

        ArenaAllocator m_allocator; // oh blimey

};