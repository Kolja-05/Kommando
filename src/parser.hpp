#pragma once

#include "tokenizer.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

struct Node_ident {
    Token identifier;
};

struct Node_expr_int_lit {
    Token int_lit;
};

struct Node_expr_ident {
    Node_ident ident;
};

struct Node_expr {
    std::variant<Node_expr_int_lit, Node_expr_ident> expr; //TODO maybe rename to ident
};

struct Node_return {
    Node_expr expr;
};

struct Node_assign {
    Node_ident identifier; //TODO maybe add extra node
    Node_expr expr;
};

struct Node_goto {
    Node_ident target_ident;
};

struct Node_if {
    // TODO
    // condition
};

struct Node_label {
    Node_ident ident;
};

struct Node_stmt {
    std::variant<Node_return, Node_assign, Node_goto, Node_if> stmt;
};

struct Node_stmt_elem {
    Node_stmt stmt;
    std::optional<Node_label> label;
};

struct Node_stmt_list {
    Node_stmt_elem stmt_elem;
    std::unique_ptr<Node_stmt_list> next; // pointer to next or Null if empty (epsilon)
};

struct Node_program {
    std::unique_ptr<Node_stmt_list> stm_list;
};

class Parser {
public:
    inline explicit Parser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens)) {
    }

    std::optional<Node_expr> parse_expr() {
        // <expr> ::= <int_lit>
        //          | <ident>
        // TODO     | <int_lit> <arit_op> <int_lit>
        if (!peek().has_value()) {
            return {};
        }
        if (peek()->type == Tokentype::int_lit) {
            return Node_expr {.expr = Node_expr_int_lit{.int_lit = consume()}};
        }
        if (peek()->type == Tokentype::identifier) {
            auto node_ident = parse_ident();
            return Node_expr {.expr = Node_expr_ident{.ident = node_ident.value()}};
        }
        return {};
    }

    std::optional<Node_stmt> parse_stmt() {
        /*<stmt> ::= <return>
              |  <assign>
         TODO |  <goto>
         TODO |  <if>     */
        if (!peek().has_value()) {
            return {};
        }
        Tokentype type = peek()->type;

        if (type == Tokentype::zurueck) {
            auto node_return = parse_return();
            if (!node_return) {
                std::cout << "error invalid keyword" << std::endl;
                exit(EXIT_FAILURE);
            }
            return Node_stmt {.stmt = node_return.value()};
        }

        if (type == Tokentype::sei) {
            auto node_assign = parse_assign();
            if (!node_assign) {
                std::cout << "error " << std::endl;
                exit(EXIT_FAILURE);
            }
            return Node_stmt {.stmt = node_assign.value()};
        }
        //TODO add if and goto
        return {};
    }

    std::optional<Node_ident> parse_ident() {
        if (!peek().has_value() || peek().value().type != Tokentype::identifier) {
            return {}; // error handling in parse assing
        }
        return Node_ident {.identifier = consume()};
    }

    std::optional<Node_assign> parse_assign() {
        // <assign>     ::= "sei" <var> "=" <expr> ";
        if (!peek().has_value() || peek().value().type != Tokentype::sei) {
            return {};
        }
        consume(); //"sei"

        auto node_ident = parse_ident();
        if (!node_ident) {
            std::cout << "error: expected identifier after 'sei'" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (!peek().has_value() || peek().value().type != Tokentype::assign_op) {
            std::cout << "error: expected '=' after identifier" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume(); // "="

        auto node_expr = parse_expr();
        if (!node_expr) {
            std::cout << "error: expected expression" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (!peek().has_value() || peek().value().type != Tokentype::semicolon) {
            std::cout << "error: expected ';" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume(); // ";"

        return Node_assign {.identifier = node_ident.value(), .expr = node_expr.value()};
    }



    std::optional<Node_return> parse_return() {
        // <return> ::= "zurueck" <expr> ";"
        if (!peek().has_value() || peek().value().type != Tokentype::zurueck) {
            return {};
        }
        consume(); // "zurueck"
        auto node_expr = parse_expr();
        if (!node_expr) {
            std::cout << "error: expected expression" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (!peek().has_value() || peek().value().type != Tokentype::semicolon) {
            std::cout << "error: symbol ';'" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume(); // ";"
        return Node_return {.expr = node_expr.value()};
    }
    std::optional<Node_stmt_elem> parse_stmt_elem() {
        // <stmt_elem>  ::= <label> <stmt> | <stmt>
        if (!peek().has_value()) {
            return {};
        }
        if (peek().has_value() && peek().value().type == Tokentype::identifier && peek(1).has_value() && peek(1).value().type == Tokentype::colon) {
            Token label_identifier = consume();
            consume(); // ":"
            auto node_stmt = parse_stmt();
            if (!node_stmt) {
                std::cout << "error: expected statement" << std::endl;
                exit(EXIT_FAILURE);
            }
            return Node_stmt_elem {.stmt = node_stmt.value(), .label = Node_label{.ident = Node_ident{.identifier = label_identifier}}};
            //creates stmt and label as node_identifier with token label_identifier
        }
        auto node_stmt = parse_stmt();
        if (!node_stmt) {
            std::cout << "error: expected statement" << std::endl;
            exit(EXIT_FAILURE);
        }
        return Node_stmt_elem {.stmt = node_stmt.value()}; //TODO
    }

    std::unique_ptr<Node_stmt_list> parse_stmt_list() {
        // <stmt_list>  ::= <stmt_elem> <stmt_list> | ε
        if (!peek().has_value()) {
            return {};
        }
        auto node_stmt_elem = parse_stmt_elem();
        if (!node_stmt_elem) {
            return {};
        }
        auto node_stmt_list = std::make_unique<Node_stmt_list>();
        node_stmt_list->stmt_elem = node_stmt_elem.value();
        node_stmt_list->next = parse_stmt_list();

        return node_stmt_list;
    }

    std::optional<Node_program> parse_program() {
        // <program>    ::= "Kommando:" <stmt_list>
        if (!peek().has_value() || peek().value().type != Tokentype::kommando_entry){
            std::cout << "error: invalid syntax, kommando needs to start with 'Kommando:" <<std::endl;
            exit(EXIT_FAILURE);
        }
        consume(); // "Kommando""
        if (!peek().has_value() || peek().value().type != Tokentype::colon) {
            std::cout << "error: expected ':' after Kommando"<<std::endl;
            exit(EXIT_FAILURE);
        }
        consume(); // ":"
        auto node_stmt_list = parse_stmt_list();
        if (!node_stmt_list) {
            std::cout << "error: expected list of statements (list can be empty)" <<std::endl;
            exit(EXIT_FAILURE);
        }
        return Node_program {.stm_list = std::move(node_stmt_list)};
    }



private:
    [[nodiscard]] std::optional<Token> peek(int offset = 0) const {
        if (m_index + offset >= m_tokens.size()) {// TODO maybe >// TODO maybe >==
            return {};
        }
        else {
            return m_tokens.at(m_index + offset);
        }
    }

    inline Token consume() {
        return m_tokens.at(m_index++);
    }

    const std::vector<Token> m_tokens;
    size_t m_index = 0;
};
