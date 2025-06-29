#pragma once

#include "parser.hpp"
#include <cstdlib>
#include <sstream>
#include <variant>






class Generator {
public:
    inline explicit Generator (Node_program program)
        : m_root(std::move(program)), m_output() {}

    void gen_expr (const Node_expr& expr) {
        if (std::holds_alternative<Node_expr_int_lit>(expr.expr)) {
            const Node_expr_int_lit int_lit = std::get<Node_expr_int_lit>(expr.expr);
            m_output << "mov rax, " << int_lit.int_lit.value.value() << "\n";
        }
        if (std::holds_alternative<Node_expr_ident>(expr.expr)) {
            //TODO retrieve from stack
        }
    }
    void gen_return(const Node_return& return_stmt) {
        //TODO
        gen_expr(return_stmt.expr); //gen_expr generates the expression we want to return and loads it into rax
        m_output << "mov rdi, rax\n"; //load exitcode into rdi
        m_output << "mov rax, 60\n"; //we want to return syscallnumber for exit
        m_output << "syscall\n"; //syscall
    }
    void gen_assign(const Node_assign& return_stmt) {
        //TODO
    }
    void gen_if(const Node_if& return_stmt) {
        //TODO
    }
    void gen_goto(const Node_goto& return_stmt) {
        //TODO
    }
    void gen_label(const Node_label& label) {
        const auto& label_str = label.ident.identifier;
        if (!label_str.value.has_value()){
            std::cout << "Invalid identifier" <<std::endl;
            exit(EXIT_FAILURE);
        }
        m_output << label_str.value.value() << ":\n";
    }
    void gen_stmt(const Node_stmt& stmt) {
        if (std::holds_alternative<Node_return>(stmt.stmt)) {
            const Node_return& return_stmt = std::get<Node_return>(stmt.stmt);
            gen_return(return_stmt);
        }
        if (std::holds_alternative<Node_assign>(stmt.stmt)) {
            const Node_assign& assign_stmt = std::get<Node_assign>(stmt.stmt);
            gen_assign(assign_stmt);
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
        //TODO maybe add NULL check
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
            Node_stmt_elem cur_stmt_elem = cur->stmt_elem;
            gen_stmt_elem(cur_stmt_elem);
            cur = cur->next.get();
        }

    }
    [[nodiscard]] std::string generate() {
        m_output << "global _start\n_start:\n"; //set entryp
        Node_stmt_list* stmt_list = m_root.stm_list.get();
        gen_stmt_list(stmt_list);

        m_output << "mov rax, 60\n";
        m_output << "mov rdi, 0 \n";
        m_output << "syscall\n";
        return m_output.str();
    }



private:
    const Node_program m_root;
    std::stringstream m_output;
};
