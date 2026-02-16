
#pragma once

#include<sstream>
#include<unordered_map>
#include<algorithm>

#include "parser.hpp"

class Generator {
    public:
        inline Generator(node::Program prog)
            : m_prog(std::move(prog))
        {

        }

        void generate_term(const node::Term* term){
            struct term_visitor{
                Generator* gen; 
                void operator()(const node::TermIntLit* int_lit) const{
                    gen->m_output << "\tmov rax, " << int_lit->int_lit.val.value() << "\n";
                    gen->push("rax"); // push int lit onto the stack
                }
                void operator()(const node::TermIdent* ident) const {

                    auto it = std::find_if(gen->m_vars.begin(), gen->m_vars.end(), 
                        [ident](const var& v) { return v.name == ident->ident.val.value(); });
                    if(it == gen->m_vars.end()){
                        std::cerr << "Identifier " << ident->ident.val.value() << " undeclared" << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    
                    const auto& var = *it; // know it isnt end cause we would terminate
                    // add location to stack pointer (rsp) 
                    std::stringstream ss; // so concat is easy between strings & numbers lol
                    // ss << "QWORD [rsp + " << (gen->m_stack_size - var.stack_location -1) * 8 <<  "]";
                    ss << "QWORD [rbp - " << (var.stack_location+1)*8 << "]";
                    gen->push(ss.str());
                }
                void operator()(const node::TermParen* paren) const {
                    gen->generate_expr(paren->expr);
                }
                void operator()(const node::TermExclaim* term_exclaim) const {
                    gen->generate_term(term_exclaim->term);
                    gen->pop("rax");
                    gen->m_output << "\tcmp rax, 0\n"; // compare term to 0
                    gen->m_output << "\tsete al\n"; // set al to 1 if rax is 0 (term is false), else set to 0 (term is true)
                    gen->m_output << "\tmovzx rax, al\n"; // zero extend al into rax, so rax is now 1 if term was false, 0 if term was true
                    gen->push("rax");
                }
                void operator()(const node::TermNegate* term_negate) const {
                    gen->generate_term(term_negate->term);
                    gen->pop("rax");
                    gen->m_output << "\tneg rax\n";
                    gen->push("rax");
                }
            };
            term_visitor visitor({.gen = this});
            std::visit(visitor, term->var);
        }

        void generate_expr(const node::Expr* expr){
            
            struct expr_visitor{
                Generator* gen;

                void operator()(const node::Term* term){
                    gen->generate_term(term);
                }

                void operator()(const node::BinExpr* bin_expr){
                    // both sides of bin expr on top of stack
                    if(auto* add = std::get_if<node::BinExprAdd*>(&bin_expr->var)){
                        gen->generate_expr((*add)->lhs);
                        gen->generate_expr((*add)->rhs);
                        gen->pop("rax");
                        gen->pop("rbx");
                        gen->m_output << "\tadd rax, rbx\n"; 
                        gen->push("rax");
                    }else if(auto* multi = std::get_if<node::BinExprMulti*>(&bin_expr->var)){
                        gen->generate_expr((*multi)->lhs);
                        gen->generate_expr((*multi)->rhs);
                        gen->pop("rax");
                        gen->pop("rbx");
                        gen->m_output << "\tmul rbx\n"; // x86 mul takes one opearand, assumes other is in rax
                        gen->push("rax");
                    }else if(auto* sub = std::get_if<node::BinExprSub*>(&bin_expr->var)){
                        gen->generate_expr((*sub)->lhs);
                        gen->generate_expr((*sub)->rhs);
                        gen->pop("rbx"); // rhs
                        gen->pop("rax"); // lhs
                        gen->m_output << "\tsub rax, rbx\n"; 
                        gen->push("rax");
                    }else if(auto* div = std::get_if<node::BinExprDiv*>(&bin_expr->var)){
                        gen->generate_expr((*div)->lhs);
                        gen->generate_expr((*div)->rhs);
                        gen->pop("rbx"); // rhs
                        gen->pop("rax"); // lhs
                        gen->m_output << "\txor rdx, rdx\n"; // clear rdx for div, as it stores remainder and we only care about quotient
                        gen->m_output << "\tdiv rbx\n"; // x86 div takes one operand, assumes other is in rax, quotient stored in rax after
                        gen->push("rax");
                    }else{
                        std::cerr << "Invalid binary expression" << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    
                    // now pop off stack into registers (put onto stack at first to hit rest of expr generation)
                   
                }
            };

            expr_visitor visitor{.gen = this};
            std::visit(visitor, expr->var);
        }

        void generate_stmt(const node::Stmt* stmt ) {
            
            struct stmt_visitor{
                Generator* gen;

                
                void operator()(const node::StmtExit* exit){
                    gen->generate_expr(exit->expr);

                    gen->m_output << "\tmov rax, 60\n"; // exit syscall 
                    gen->pop("rdi"); // pop top of stack, as that should contain the expr we want to pass to exit
                    gen->m_output << "\tsyscall\n";
                }
                void operator()(const node::StmtLet* let){
                    auto it = std::find_if(gen->m_vars.begin(), gen->m_vars.end(), 
                        [let](const var& v) { 
                            return v.name == let->ident.val.value(); 
                        });
                    if(it != gen->m_vars.end()){
                        std::cerr << "Identifier " << let->ident.val.value() << " previously defined" << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    gen->m_vars.push_back( var{let->ident.val.value(), gen->m_stack_size});
                    gen->generate_expr(let->expr); // now evaluated value of expression is on the top of the stack, generate_expr puts on stack for us
                    gen->m_output << "\tmov " << "QWORD [rbp - " << (gen->m_stack_size)*8 << "], rax\n";
                }
                void operator()(const node::StmtAssign* assign){
                    auto it = std::find_if(gen->m_vars.begin(), gen->m_vars.end(), 
                        [assign](const var& v) { return v.name == assign->ident.val.value(); });
                    if(it == gen->m_vars.end()){
                        std::cerr << "Identifier " << assign->ident.val.value() << " not previously defined" << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    gen->generate_expr(assign->expr); // top of stack 
                    
                    gen->pop("rax"); // pop new value into rax
                    gen->m_output << "\tmov " << "QWORD [rbp - " << ((*it).stack_location+1)*8 << "], rax\n"; // move rax into stack location for variable
                
                }

                void operator()(const node::Scope* scope){
                    gen->begin_scope();

                    for(const node::Stmt* stmt : scope->statements){
                        gen->generate_stmt(stmt);
                    }

                    gen->end_scope();
                }

                void operator()(const node::StmtIf* stmt_if){
                    gen->generate_expr(stmt_if->condition); // condition on top of stack

                    gen->pop("rax"); // pop condition into rax
                    gen->m_output << "\tcmp rax, 0\n"; // compare condition to 0
                    std::string else_label = "else_l" + std::to_string(gen->m_labelcount); // unique label for else block, can be based on stack size as it changes with each new scope/variable declaration
                    std::string end_label = "end_if_l" + std::to_string(gen->m_labelcount);
                    gen->m_labelcount++;
                    gen->m_output << "\tje " << else_label << "\n"; // jump to else block if condition is false (0)
                    
                    // if body
                    for(const node::Stmt* stmt : stmt_if->body->statements){
                        gen->generate_stmt(stmt);
                    }
                    gen->m_output << "\tjmp " << end_label << "\n"; // jump to end of if after if body
                    
                    // else body would go here
                    gen->m_output << else_label << ":\n";

                    gen->m_output << end_label << ":\n";
                }

                void operator()(const node::StmtWhile* stmt_while){
                    std::string loop_start_label = "while_loop_start_l" + std::to_string(gen->m_labelcount);
                    std::string loop_end_label = "while_loop_end_l" + std::to_string(gen->m_labelcount);
                    gen->m_labelcount++;

                    gen->m_output << loop_start_label << ":\n";
                    gen->generate_expr(stmt_while->condition); // condition on top of stack

                    gen->pop("rax"); // pop condition into rax
                    gen->m_output << "\tcmp rax, 0\n"; // compare condition to 0
                    gen->m_output << "\tje " << loop_end_label << "\n"; // jump to end of loop if condition is false (0)
                    
                    // loop body
                    for(const node::Stmt* stmt : stmt_while->body->statements){
                        gen->generate_stmt(stmt);
                    }
                    gen->m_output << "\tjmp " << loop_start_label << "\n"; // jump back to start of loop to check condition again
                    
                    gen->m_output << loop_end_label << ":\n";
                }
            };
            stmt_visitor visitor{.gen = this};
            std::visit(visitor, stmt->var);

        }

        [[nodiscard]] std::string generate_asm_x86() {

            m_output << "global _start\n_start:\n";
            m_output << "\tpush rbp\n\tmov rbp, rsp\n\n"; // prologue for rbp (fixed sp to calculate variable offsets from)


            for(const node::Stmt* stmt : m_prog.statements){
                generate_stmt(stmt);
            }

            // this is default exit, runs if user does not specify 
            // if user does specify, obviously it'll just have exited previously
            m_output << "\n\tmov rax, 60\n";
			m_output << "\tmov rdi, 0\n";
			m_output << "\tsyscall\n";

            return m_output.str();
        }
    private:

        // use these methods instead of raw push/pop so we track stack size 
        void push(const std::string& reg){
            m_output << "\tpush " << reg << "\n";
            m_stack_size++;
        }
        void pop(const std::string& reg){
            m_output << "\tpop " << reg << "\n";
            m_stack_size--;
        }

        void begin_scope(){
            m_scopes.push_back(m_vars.size()); // push current number of variables, so we know how many to pop off when we end scope
        }

        void end_scope(){
            if(m_scopes.empty()){
                std::cerr << "Unexpected '}', no scope to end" << std::endl;
                exit(EXIT_FAILURE);
            }
            size_t popcount = m_vars.size() - m_scopes.back();
            m_output << "\tadd rsp, " << popcount*8 << "\n"; // move stack pointer back to dealloc variables in current scope, ts moves independant of rbp
            m_stack_size -= popcount; 
            for(int i=0; i<popcount; i++){
                m_vars.pop_back();
            }
            m_scopes.pop_back(); // kill last scope
        }

        struct var {
            std::string name;
            size_t stack_location;
        };
        
        const node::Program m_prog;
        std::stringstream m_output;
        size_t m_stack_size = 0;
        unsigned long m_labelcount = 0;

        std::vector<var> m_vars {};

        std::vector<size_t> m_scopes; // indicies of variables array

};