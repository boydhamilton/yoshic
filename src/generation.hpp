
#pragma once

#include<sstream>
#include<unordered_map>

#include "parser.hh"

class Generator {
    public:
        inline Generator(node::Program prog)
            : m_prog(std::move(prog))
        {

        }

       void generate_expr(const node::Expr& expr){
            
            struct expr_visitor{
                Generator* gen;

                void operator()(const node::ExprIdent& ident){
                    if(gen->m_vars.find(ident.ident.val.value() ) == gen->m_vars.end() ){
                        std::cerr << "Undeclared identifier " << ident.ident.val.value() << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    const auto& var = gen->m_vars.at(ident.ident.val.value());
                    // add location to stack pointer (rsp) 
                    std::stringstream ss; // so concat is easy between strings & numbers lol
                    // ss << "QWORD [rsp + " << (gen->m_stack_size - var.stack_location -1) * 8 <<  "]";
                    ss << "QWORD [rbp -" << (var.stack_location+1)*8 << "]";
                    gen->push(ss.str());
                }

                void operator()(const node::ExprIntLit& int_lit){
                    gen->m_output << "\tmov rax, " << int_lit.int_lit.val.value() << "\n";
                    gen->push("rax"); // push int lit onto the stack
                    // gen->m_output << "\tpush rax\n"; 
                }
            };

            expr_visitor visitor{.gen = this};
            std::visit(visitor, expr.var);

        }

        void generate_stmt(const node::Stmt& stmt ) {
            
            struct stmt_visitor{
                Generator* gen;

                
                void operator()(const node::StmtExit& exit){
                    gen->generate_expr(exit.expr);

                    gen->m_output << "\tmov rax, 60\n";
                    //gen->m_output << "\tpop rdi\n"; 
                    gen->pop("rdi"); // pop top of stack, as that should contain the expr we want to pass to exit
                    gen->m_output << "\tsyscall\n";
                }
                void operator()(const node::StmtLet& let){
                    if(gen->m_vars.find(let.ident.val.value()) != gen->m_vars.end() ){
                        std::cerr << "Identifier " << let.ident.val.value() << " already used" << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    gen->m_vars.insert( {let.ident.val.value(), var {.stack_location = gen->m_stack_size} } );
                    gen->generate_expr(let.expr); // now evaluated value of expression is on the top of the stack, generate_expr puts on stack for us
                }
            };
            stmt_visitor visitor{.gen = this};
            std::visit(visitor, stmt.var);

        }

        [[nodiscard]] std::string generate_asm_x86() {

            m_output << "global _start\n_start:\n";
            m_output << "\tpush rbp\n\tmov rbp, rsp\n\n"; // prologue for rbp (fixed sp to calculate variable offsets from)


            for(const node::Stmt stmt : m_prog.statements){
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

        struct var {
            size_t stack_location;
        };
        
        const node::Program m_prog;
        std::stringstream m_output;
        size_t m_stack_size = 0;

        std::unordered_map<std::string, var> m_vars {};

};