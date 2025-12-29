#pragma once

#include "parser.hpp"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <ostream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>






class Generator {
public:
    //Constructor
    inline explicit Generator (Node_program program, std::set<std::string> defined_labels)
        : m_root(std::move(program)), m_defined_labels(std::move(defined_labels)), m_output(), m_stack_offset(0), m_symbol_table() {}

    [[nodiscard]] std::string generate() {
        // PROLOG
        m_output << "global _start\n_start:\n"; //set entryp
        // set frame pointer
        m_output << "push rbp\n"; //store frame pointer on stack
        m_output << "mov rbp, rsp\n"; // initialize frame pointer

        Node_stmt_list* stmt_list = m_root.stm_list.get();
        gen_stmt_list(stmt_list);


        // EPILOG
        // restore frame pointer
        m_output << "mov rsp, rbp\n"; // restore stack pointer
        m_output << "pop rbp\n";    // restore frame pointer from stack
        m_output << "mov rax, 60\n";
        m_output << "mov rdi, 0\n";
        m_output << "syscall\n";
        return m_output.str();
    }



private:
    // MEMBER VARIALBES
    const Node_program m_root;
    const std::set<std::string> m_defined_labels;
    std::unordered_map<std::string, std::ptrdiff_t> m_symbol_table;
    std::ptrdiff_t m_stack_offset = 0;
    std::stringstream m_output;

    //MEMBER METHODS
    //
    //CODE GENERATION
    void gen_expr (const Node_expr* expr) {
        if (std::holds_alternative<std::unique_ptr<Node_expr_int_lit>>(expr->expr)) {
            // generate expr, store the value in rax
            const Node_expr_int_lit * int_lit = std::get<std::unique_ptr<Node_expr_int_lit>>(expr->expr).get();
            m_output << "mov rax, " << int_lit->int_lit.value.value() << "\n";
        }
        if (std::holds_alternative<std::unique_ptr<Node_expr_ident>>(expr->expr)) {
            // retrieve from stack
            const Node_expr_ident * ident = std::get<std::unique_ptr<Node_expr_ident>>(expr->expr).get();
            if (!ident->ident.identifier.value.has_value()) {
                std::cerr << "error: invalid identifier" << std::endl;
                exit(EXIT_FAILURE);
            }
            load(ident->ident.identifier.value.value()); // load value of identifier into rax
        }
        // handle bin expr
        if (std::holds_alternative<std::unique_ptr<Node_bin_expr>>(expr->expr)) {
            const Node_bin_expr * bin_expr = std::get<std::unique_ptr<Node_bin_expr>>(expr->expr).get();
            if (std::holds_alternative<std::unique_ptr<Node_bin_expr_add>>(bin_expr->bin_expr)){
                const Node_bin_expr_add * add = std::get<std::unique_ptr<Node_bin_expr_add>>(bin_expr->bin_expr).get();
                gen_expr(add->lhs.get()); // put lhs into rax
                // handle lhs
                m_output << "push rax\n"; //save lhs
                // handle rhs
                gen_expr(add->rhs.get()); // put rhs into rax
                m_output << "pop rbx\n"; // put lhs into rbx
                m_output << "add rax, rbx\n"; // add lhs onto rhs (add rbx, rax adds whats in rax onto rbx)
            }
            if (std::holds_alternative<std::unique_ptr<Node_bin_expr_multi>>(bin_expr->bin_expr)){
                const Node_bin_expr_add * multi = std::get<std::unique_ptr<Node_bin_expr_add>>(bin_expr->bin_expr).get();
                gen_expr(multi->lhs.get()); // put lhs into rax
                // handle lhs
                m_output << "push rax\n"; //save lhs
                // handle rhs
                gen_expr(multi->rhs.get()); // put rhs into rax
                m_output << "pop rbx\n"; // put lhs into rbx
                m_output << "imul rax, rbx\n"; // multi lhs with rhs (imul rbx, rax multiplies whats in rax with rbx and stores it in rbx)
                const Node_bin_expr_multi * bin_expr_multi = std::get<std::unique_ptr<Node_bin_expr_multi>>(bin_expr->bin_expr).get();
            }
        }
    }

 
    void gen_return(const Node_return& return_stmt) {
        gen_expr(return_stmt.expr.get()); //gen_expr generates the expression we want to return and loads it into rax
        m_output << "mov rdi, rax\n"; //load exitcode into rdi
        m_output << "mov rax, 60\n"; //we want to return syscallnumber for exit
        m_output << "syscall\n"; //syscall
    }
    void gen_var_def_assign(const Node_var_def_assign& assign_stmt) {
        if (!assign_stmt.identifier.identifier.value.has_value()) {
            std::cerr << "Invalid Identifier" <<std::endl;
            exit(EXIT_FAILURE);
        }
        std::string identifier = assign_stmt.identifier.identifier.value.value();
        gen_expr(&assign_stmt.expr); //generates the expression and stores the value in rax
        store(identifier); // pushes the value in rax onto the stack stores the identifier with its stack_offset in m_symbol_table and increments stack pointer
    }

    void gen_var_assign(const Node_var_assign& assign_stmt) {
        if (!assign_stmt.identifier.identifier.value.has_value()) {
            std::cerr << "Invalid Identifier" << std::endl;
            exit(EXIT_FAILURE);
        }
        std::string identifier = assign_stmt.identifier.identifier.value.value();
        gen_expr(&assign_stmt.expr);
        std::ptrdiff_t ident_stack_offset = get_stack_offset(identifier);
        if (ident_stack_offset < 0) {
            m_output << "mov QWORD [rbp" << ident_stack_offset << "], rax\n"; //move onto stack
        } else {
            m_output << "mov QWORD [rbp+" << ident_stack_offset << "], rax\n";
        }
    }

    void gen_if(const Node_if& if_stmt) {
        //TODO
    }
    void gen_goto(const Node_goto& goto_stmt) {
        //TODO
        Node_ident target_ident = goto_stmt.target_ident;
        std::string target_ident_str = target_ident.identifier.value.value();
        if (m_defined_labels.find(target_ident_str) == m_defined_labels.end()) {
            std::cerr << "error: call to undeclared identifier (label) " << target_ident_str << "'"<<std::endl;
            exit(EXIT_FAILURE);
        }
        m_output << "jmp " << target_ident_str << "\n"; // jump to targen label
    }
    void gen_label(const Node_label& label) {
        const auto& label_str = label.ident.identifier;
        if (!label_str.value.has_value()){
            std::cerr << "Invalid identifier" <<std::endl;
            exit(EXIT_FAILURE);
        }
        m_output << label_str.value.value() << ":\n";
    }
    void gen_stmt(const Node_stmt& stmt) {
        if (std::holds_alternative<Node_return>(stmt.stmt)) {
            const Node_return& return_stmt = std::get<Node_return>(stmt.stmt);
            gen_return(return_stmt);
        }
        if (std::holds_alternative<Node_var_def_assign>(stmt.stmt)) {
            const Node_var_def_assign& assign_stmt = std::get<Node_var_def_assign>(stmt.stmt);
            gen_var_def_assign(assign_stmt);
        }
        if (std::holds_alternative<Node_var_assign>(stmt.stmt)) {
            const Node_var_assign& assign_stmt = std::get<Node_var_assign>(stmt.stmt);
            gen_var_assign((assign_stmt));
        }
        if (std::holds_alternative<Node_if>(stmt.stmt)) {
            const Node_if& if_stmt = std::get<Node_if>(stmt.stmt);
            gen_if(if_stmt);
        }
        if (std::holds_alternative<Node_goto>(stmt.stmt)) {
            const Node_goto& goto_stmt = std::get<Node_goto>(stmt.stmt);
            gen_goto(goto_stmt);
        }
    }
    void gen_stmt_elem(const Node_stmt_elem& stmt_elem) {
        if (stmt_elem.label.has_value()) {
            //gen label
            gen_label(stmt_elem.label.value());
        }
        gen_stmt(stmt_elem.stmt);
    }

    void  gen_stmt_list(const Node_stmt_list* stmt_list) {
        const Node_stmt_list* cur = stmt_list;
        //gen all statments in the list
        while (cur) {
            const Node_stmt_elem& cur_stmt_elem = cur->stmt_elem;
            gen_stmt_elem(cur_stmt_elem);
            cur = cur->next.get(); }

    }

    // HELPER METHODS
    void store(const std::string& identifier) {
        // store value of identifier and stack offset in m_symbol_table and increment stack offset
        // the caller should provide a value to be stored in the rax register
        m_stack_offset -= 8; //adjust stack pointer (stack grows downwards)
        std::ptrdiff_t offset = m_stack_offset;
        m_symbol_table[identifier] = offset;
        if (offset < 0) {
            m_output << "mov QWORD [rbp" << offset << "], rax\n"; //move onto stack
        } else {
            m_output << "mov QWORD [rbp+" << offset << "], rax\n";
        }
    }
    void load(const std::string& identifier) {
        // load value of identifier from stack frame into rax regiser
        std::ptrdiff_t offset = get_stack_offset(identifier);
        if (offset < 0) {
            m_output << "mov rax, QWORD [rbp" << offset << "]\n";

        } else {
            m_output << "mov rax, QWORD [rbp+" << offset << "]\n";
        }
    }

    std::ptrdiff_t get_stack_offset(const std:: string& identifier) const {
        auto it = m_symbol_table.find(identifier);
        if (it == m_symbol_table.end()) {
            std::cerr << "error: call to undeclared identifier '" << identifier << "'" <<std::endl;
            exit(EXIT_FAILURE);
        }
        return it->second;
    }
};
