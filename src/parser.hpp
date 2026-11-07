
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

    struct TermParen {
        node::Expr* expr; // parenthesized expression
    };

    struct Term;

    struct TermExclaim {
        node::Term* term; // !term
    };

    struct TermNegate {
        node::Term* term; // -term
    };
    

    struct Term{
        std::variant<node::TermIntLit*, 
        node::TermIdent*, node::TermParen*,
        node::TermExclaim*, node::TermNegate*> var;
    };

    struct BinExprAdd {
        node::Expr* lhs;
        node::Expr* rhs;
    };
    struct BinExprSub{
        node::Expr* lhs;
        node::Expr* rhs;
    };

    struct BinExprMulti{
        node::Expr* lhs;
        node::Expr* rhs;
    };
    struct BinExprDiv{
        node::Expr* lhs;
        node::Expr* rhs;
    };

    struct BinExprGt{
        node::Expr* lhs;
        node::Expr* rhs;
    };

    struct BinExprLt{
        node::Expr* lhs;
        node::Expr* rhs;
    };

    struct BinExprEq{
        node::Expr* lhs;
        node::Expr* rhs;
    };

    struct BinExprNeq{
        node::Expr* lhs;
        node::Expr* rhs;
    };

    struct BinExpr{
        std::variant<node::BinExprAdd*, node::BinExprMulti*, node::BinExprSub*, node::BinExprDiv*,
        node::BinExprGt*, node::BinExprLt*,
        node::BinExprEq*, node::BinExprNeq*> var;
        //node::BinExprAdd* var;
    };

    struct Expr{
        std::variant<node::Term*, node::BinExpr*> var;
    };

    struct Stmt;
    
    struct StmtLet {
        Token ident;
        node::Expr* expr;
    };

    struct StmtAssign { // different because we have to check name conflicts at let
        Token ident;
        node::Expr* expr;
    };

    struct StmtExit{
        node::Expr* expr;  
    };

    struct Scope{
        std::vector<node::Stmt*> statements;
    };

    struct StmtIf{
        node::Expr* condition;
        node::Scope* body;
    };    

    struct StmtWhile{
        node::Expr* condition;
        node::Scope* body;
    };
    
    struct Stmt {
        std::variant<node::StmtExit*, node::StmtLet*, 
        node::StmtAssign*, node::Scope*, node::StmtIf*,
        node::StmtWhile*> var;
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
             if(peek() && peek()->type == TokenType::int_lit){

                auto expr_int_lit = m_allocator.alloc<node::TermIntLit>();
                expr_int_lit->int_lit = consume(); // structure is implicit in allocation
                
                auto term = m_allocator.alloc<node::Term>();
                term->var = expr_int_lit;
                return term;

            }else if(peek() && peek()->type == TokenType::ident){
                auto expr_ident = m_allocator.alloc<node::TermIdent>();
                expr_ident->ident = consume();
                
                auto term = m_allocator.alloc<node::Term>();
                term->var = expr_ident;
                return term;
            }else if(peek() && peek()->type == TokenType::open_paren){
                consume(); // consume open paren
                auto term_paren = m_allocator.alloc<node::TermParen>();
                if(auto node_expr = parse_expr() ){
                    term_paren->expr=node_expr.value();
                }else{
                    std::cerr << "Invalid expr in term paren" << std::endl;
                    exit(EXIT_FAILURE);
                }

                if(peek() && peek()->type == TokenType::close_paren){
                    consume(); // consume close paren
                }else{
                    std::cerr << "Expected ')'" << std::endl;
                    exit(EXIT_FAILURE);
                }
                auto term = m_allocator.alloc<node::Term>();
                term->var = term_paren;
                return term;
            }else if(peek() && peek()->type == TokenType::exclaim){
                consume(); // consume !
                if(auto term = parse_term() ){
                    auto term_exclaim = m_allocator.alloc<node::TermExclaim>();
                    term_exclaim->term = term.value();
                    auto new_term = m_allocator.alloc<node::Term>();
                    new_term->var = term_exclaim;
                    return new_term;
                }
            }else if(peek() && peek()->type == TokenType::sub){
                consume(); // consume -
                if(auto term = parse_term() ){
                    auto term_exclaim = m_allocator.alloc<node::TermNegate>();
                    term_exclaim->term = term.value();
                    auto new_term = m_allocator.alloc<node::Term>();
                    new_term->var = term_exclaim;
                    return new_term;
                }
            }
            
            else{
                return {};  
            }
        }

        std::optional<node::Expr*> parse_expr(int min_prec = -1){
            
            std::optional<node::Term*> term_lhs = parse_term();
            if(!term_lhs.has_value()){
                return {};
            }

            auto expr_lhs = m_allocator.alloc<node::Expr>();
            expr_lhs->var = term_lhs.value();

            while(true){
                const Token* curr_tok = peek();
                std::optional<int> prec;
                if(curr_tok){
                    prec = bin_prec(curr_tok->type);
                    if(!prec.has_value() || prec.value() < min_prec){
                        break;
                    }
                }else{
                    break;
                }

                const Token& op = consume(); // consume operator
                int next_min_prec = prec.value() + 1;
                std::optional<node::Expr*> expr_rhs = parse_expr(next_min_prec);

                if(!expr_rhs.has_value()){
                    std::cerr << "Expected rhs in binary expression" << std::endl;
                    exit(EXIT_FAILURE);
                }

                auto expr = m_allocator.alloc<node::BinExpr>();
                if(op.type == TokenType::plus){
                    auto add = m_allocator.alloc<node::BinExprAdd>();
                    add->lhs = expr_lhs;
                    add->rhs = expr_rhs.value();
                    expr->var = add;
                }else if(op.type == TokenType::multi){
                    auto multi = m_allocator.alloc<node::BinExprMulti>();
                    multi->lhs = expr_lhs;
                    multi->rhs = expr_rhs.value();
                    expr->var = multi;
                }else if(op.type == TokenType::sub){
                    auto sub = m_allocator.alloc<node::BinExprSub>();
                    sub->lhs = expr_lhs;
                    sub->rhs = expr_rhs.value();
                    expr->var = sub;
                }else if(op.type == TokenType::div){
                    auto div = m_allocator.alloc<node::BinExprDiv>();
                    div->lhs = expr_lhs;
                    div->rhs = expr_rhs.value();
                    expr->var = div;    
                }else if(op.type == TokenType::greaterthan){
                    auto gt = m_allocator.alloc<node::BinExprGt>();
                    gt->lhs = expr_lhs;
                    gt->rhs = expr_rhs.value();
                    expr->var = gt;
                }else if(op.type == TokenType::lessthan){
                    auto lt = m_allocator.alloc<node::BinExprLt>();
                    lt->lhs = expr_lhs;
                    lt->rhs = expr_rhs.value();
                    expr->var = lt;
                }else if(op.type == TokenType::equal_cmp){
                    auto eq = m_allocator.alloc<node::BinExprEq>();
                    eq->lhs = expr_lhs;
                    eq->rhs = expr_rhs.value();
                    expr->var = eq;
                }else if(op.type == TokenType::nequal_cmp){
                    auto neq = m_allocator.alloc<node::BinExprNeq>();
                    neq->lhs = expr_lhs;
                    neq->rhs = expr_rhs.value();
                    expr->var = neq;
                }
                
                else{
                    std::cerr << "Expected binary operator" << std::endl;
                    exit(EXIT_FAILURE);
                }

                // wrapper kinda gross but fixed it
                auto new_expr_lhs = m_allocator.alloc<node::Expr>();
                new_expr_lhs->var = expr;
                expr_lhs = new_expr_lhs; // now this whole expr becomes the lhs for the next iteration of the loop, allowing for left associativity

            }
            return expr_lhs;           
        }

        std::optional<node::Scope*> parse_scope(){
            if(peek() && peek()->type == TokenType::open_curly){
                consume(); // consume open curly
                auto scope = m_allocator.alloc<node::Scope>();
                while(peek() && peek()->type != TokenType::close_curly){
                    if(auto node_stmt = parse_stmt() ){
                        scope->statements.push_back(node_stmt.value());
                    }else{
                        std::cerr << "Invalid statement in scope" << std::endl;
                        exit(EXIT_FAILURE);
                    }
                }
                if(peek() && peek()->type == TokenType::close_curly){
                    consume(); // consume close curly
                }else{
                    std::cerr << "Expected '}' at end of scope" << std::endl;
                    exit(EXIT_FAILURE);
                }
                return scope;
            }else{
                return {};
            }
        }

        std::optional<node::Stmt*> parse_stmt(){
            if(peek() && peek()->type == TokenType::exit
            && peek(1) ){

                consume();
                auto stmt_exit = m_allocator.alloc<node::StmtExit>();
                
                if(auto node_expr = parse_expr() ){
                    stmt_exit->expr=node_expr.value();
                }
                if(peek() && peek()->type == TokenType::semi){
                    consume();
                }else{
                    std::cerr << "Expected ';' in statement exit" << std::endl;
                    exit(EXIT_FAILURE);
                }
                auto stmt = m_allocator.alloc<node::Stmt>();
                stmt->var = stmt_exit;
                
                return stmt;

            }else if(peek() && peek()->type == TokenType::let 
            && peek(1) && peek(1)->type == TokenType::ident
            && peek(2) && peek(2)->type == TokenType::equal_assign){

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
                if(peek() && peek()->type == TokenType::semi){
                    consume();
                }else{
                    std::cerr << "Expected ';' in statement let" << std::endl;
                    exit(EXIT_FAILURE);
                }
                auto stmt = m_allocator.alloc<node::Stmt>();
                stmt->var = stmt_let;
                return stmt;
            }else if(peek() && peek()->type == TokenType::ident
            && peek(1) && peek(1)->type == TokenType::equal_assign){

                auto stmt_let = m_allocator.alloc<node::StmtAssign>();
                stmt_let->ident = consume(); // consume ident
                consume(); // consume eq
                if(auto node_expr = parse_expr() ){
                    stmt_let->expr = node_expr.value();
                }else{
                    std::cerr << "Invalid expr in statement assignment to " << stmt_let->ident.val << std::endl;
                    exit(EXIT_FAILURE);
                }
                if(peek() && peek()->type == TokenType::semi){
                    consume();
                }else{
                    std::cerr << "Expected ';' in statement assignment to " << stmt_let->ident.val << std::endl;
                    exit(EXIT_FAILURE);
                }
                auto stmt = m_allocator.alloc<node::Stmt>();
                stmt->var = stmt_let;
                return stmt;
            }else if(peek() && peek()->type == TokenType::open_curly){
                consume(); // consume open curly
                auto stmt_scope = m_allocator.alloc<node::Scope>();
                while(peek() && peek()->type != TokenType::close_curly){
                    if(auto node_stmt = parse_stmt() ){
                        stmt_scope->statements.push_back(node_stmt.value());
                    }else{
                        std::cerr << "Invalid statement in scope" << std::endl;
                        exit(EXIT_FAILURE);
                    }
                }
                if(peek() && peek()->type == TokenType::close_curly){
                    consume(); // consume close curly
                }else{
                    std::cerr << "Expected '}' at end of scope" << std::endl;
                    exit(EXIT_FAILURE);
                }
                auto stmt = m_allocator.alloc<node::Stmt>();
                stmt->var = stmt_scope;
                return stmt;
            }else if(peek() && peek()->type == TokenType::if_kw){
                consume(); // consume if
                if(peek() && peek()->type == TokenType::open_paren){
                    consume();
                }else { 
                    std::cerr << "Expected '(' after 'if'" << std::endl;
                    exit(EXIT_FAILURE);
                }
                auto stmt_if = m_allocator.alloc<node::StmtIf>();
                if(auto expr = parse_expr() ){
                    stmt_if->condition = expr.value();
                }else{
                    std::cerr << "Invalid condition expression in if statement" << std::endl;
                    exit(EXIT_FAILURE);
                }
                
                if(peek() && peek()->type == TokenType::close_paren){
                    consume();
                }else{
                    std::cerr << "Expected ')' after condition in if statement" << std::endl;
                    exit(EXIT_FAILURE);
                }

                if(auto scope = parse_scope()){
                    stmt_if->body = scope.value();
                }else{
                    std::cerr << "Invalid scope in if statement" << std::endl;
                    exit(EXIT_FAILURE);
                }
                auto stmt = m_allocator.alloc<node::Stmt>();
                stmt->var = stmt_if;
                return stmt;
            }else if(peek() && peek()->type == TokenType::while_kw){
                consume(); // consume while
                if(peek() && peek()->type == TokenType::open_paren){
                    consume();
                }else { 
                    std::cerr << "Expected '(' after 'while'" << std::endl;
                    exit(EXIT_FAILURE);
                }
                auto stmt_while = m_allocator.alloc<node::StmtWhile>();
                if(auto expr = parse_expr() ){
                    stmt_while->condition = expr.value();
                }else{
                    std::cerr << "Invalid condition expression in while statement" << std::endl;
                    exit(EXIT_FAILURE);
                }
                
                if(peek() && peek()->type == TokenType::close_paren){
                    consume();
                }else{
                    std::cerr << "Expected ')' after condition in while statement" << std::endl;
                    exit(EXIT_FAILURE);
                }

                if(auto scope = parse_scope()){
                    stmt_while->body = scope.value();
                }else{
                    std::cerr << "Invalid scope in if statement" << std::endl;
                    exit(EXIT_FAILURE);
                }
                auto stmt = m_allocator.alloc<node::Stmt>();
                stmt->var = stmt_while;
                return stmt;
            }else{
                return {};
            }
            
        }

        std::optional<node::Program> parse_program(){
            node::Program prog;
            while(peek() ){
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
        // returns a pointer into m_tokens (nullptr past the end) rather
        // than a copy: with the old std::optional<Token> return type,
        // every single lookahead - and this is called from nearly every
        // parser decision point - copied a whole Token (including its
        // std::string val) just to check a type tag. m_tokens is const
        // and never resized after construction, so the pointer stays
        // valid for the Parser's whole lifetime.
        [[nodiscard]] inline const Token* peek(int ahead = 0) const {
            if(m_index + ahead >= m_tokens.size()){
                return nullptr;
            }else{
                return &m_tokens.at(m_index + ahead);
            }
        }

        // same idea: a reference into m_tokens, not a copy. Callers that
        // need to keep the value (e.g. `stmt_let->ident = consume();`)
        // still copy exactly once, at the point they actually need to;
        // the many call sites that just skip a token (`consume(); //
        // consume eq`) now copy nothing at all.
        inline const Token& consume(){
            return m_tokens.at(m_index++);
        }
        const std::vector<Token> m_tokens;
        size_t m_index = 0; // size cause its index 

        ArenaAllocator m_allocator; // oh blimey

};