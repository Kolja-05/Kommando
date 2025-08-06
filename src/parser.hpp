#pragma once

#include "tokenizer.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <set>
#include <string>
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
    std::variant<Node_expr_int_lit, Node_expr_ident> expr;
};

struct Node_return {
    Node_expr expr;
};
struct Node_print {
    Node_expr expr;
};

struct Node_var_def_assign {
    Node_ident identifier;
    Node_expr expr;
};

struct Node_var_assign {
    Node_ident identifier;
    Node_expr expr;
};

struct Node_goto {
    Node_ident target_ident;
};

struct Node_cond {
    // TODO
};
struct Node_if {
    // TODO
    Node_cond condition;
};

struct Node_label {
    Node_ident ident;
};

struct Node_stmt {
    std::variant<Node_return, Node_var_def_assign, Node_var_assign, Node_goto, Node_if, Node_print> stmt;
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

    std::set<std::string> get_defined_labels () const {
        return m_defined_labels;
    }


private:
    const std::vector<Token> m_tokens;
    size_t m_index = 0;
    std::set<std::string> m_defined_labels;
    // HELPER METHODS
    inline Token consume() {
        return m_tokens.at(m_index++);
    }

    [[nodiscard]] std::optional<Token> peek(int offset = 0) const {
        if (m_index + offset >= m_tokens.size()) {// TODO maybe >// TODO maybe >==
            return {};
        }
        else {
            return m_tokens.at(m_index + offset);
        }
    }

    // PARSING METHODS
    std::optional<Node_expr> parse_expr() {
        //<expr> ::= <int_lit>
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
        if (type == Tokentype::schreibe) {
            auto Node_print = parse_print();
            if (!Node_print) {
                std::cout <<"error: expected schreibe" << std::endl;
                exit(EXIT_FAILURE);
            }
            return Node_stmt {.stmt = Node_print.value()};
        }
        if (type == Tokentype::sei) {
            auto node_var_def_assign = parse_var_def_assign();
            if (!node_var_def_assign) {
                std::cout << "error: expected variable definition" << std::endl;
                exit(EXIT_FAILURE);
            }
            return Node_stmt {.stmt = node_var_def_assign.value()};
        }
        if (type == Tokentype::identifier) {
            auto node_var_assign = parse_var_assign();
            if (!node_var_assign) {
                std::cout << "error expected variable assignment" << std::endl;
                exit(EXIT_FAILURE);
            }
            return Node_stmt {.stmt = node_var_assign.value()};
        }
        if (type == Tokentype::springe) {
            auto Node_goto = parse_goto();
            if (!Node_goto) {
                std::cout <<"error: expected jump" << std::endl;
                exit(EXIT_FAILURE);
            }
            return Node_stmt {.stmt = Node_goto.value()};
        }

        //TODO add if
        return {};
    }
    std::optional<Node_goto> parse_goto() {
        //TODO
        if (!peek().has_value() || peek().value().type != Tokentype::springe) {
            return {};
        }
        consume(); // "springe"
        if (!peek().has_value() || peek()->type != Tokentype::identifier) {
            std::cout << "error: expected identifier (label) after springe stamtement" << std::endl;
            exit(EXIT_FAILURE);
        }
        Token identifier = consume(); // <label>
        if (!identifier.value.has_value()) {
            std::cerr << "error: identifier token has no value" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (!peek().has_value() || peek().value().type != Tokentype::semicolon) {
            std::cout << "error: expected ';'" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume(); // ";"
        return Node_goto{.target_ident = Node_ident{.identifier = identifier}};
    }
    std::optional<Node_ident> parse_ident() {
        if (!peek().has_value() || peek().value().type != Tokentype::identifier) {
            return {}; // error handling in parse assing
        }
        return Node_ident {.identifier = consume()};
    }

    std::optional<Node_var_def_assign> parse_var_def_assign() {
        // <var_def_assign>     ::= "sei" <var> "=" <expr> ";
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

        return Node_var_def_assign {.identifier = node_ident.value(), .expr = node_expr.value()};
    }

    std::optional<Node_var_assign> parse_var_assign() {
        // <var_assign> ::= <var> "=" <expr> ";"
        if (!peek().has_value() || peek().value().type != Tokentype::identifier) {
            return {};
        }
        auto node_ident = parse_ident();
        if (!node_ident) {
            std::cout << "error: expected identifier" << std::endl;
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
            std::cout << "error: expected ';'" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume(); //";"
        return  Node_var_assign{.identifier = node_ident.value(), .expr = node_expr.value()};
        
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
    std::optional<Node_print> parse_print() {
        // <return> ::= "zurueck" <expr> ";"
        if (!peek().has_value() || peek().value().type != Tokentype::schreibe) {
            return {};
        }
        consume(); // "schreibe"
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
        return Node_print {.expr = node_expr.value()};
    }
    std::optional<Node_stmt_elem> parse_stmt_elem() {
        // <stmt_elem>  ::= <label> <stmt> | <stmt>
        if (!peek().has_value()) {
            return {};
        }
        if (peek().has_value() && peek().value().type == Tokentype::identifier && peek(1).has_value() &&
            peek(1).value().type == Tokentype::colon) {
            Token label_identifier = consume();

            consume(); // ":"
            auto node_stmt = parse_stmt();
            if (!node_stmt) {
                std::cout << "error: expected statement and label" << std::endl;
                exit(EXIT_FAILURE);
            }
            std::string label_str = label_identifier.value.value();
            if (m_defined_labels.find(label_str) != m_defined_labels.end()) {
                std::cout << "error: redefinition of label '" << label_str << "'" <<std::endl;
                exit(EXIT_FAILURE);
            }
            m_defined_labels.insert(label_str);
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
    std::optional<Node_cond> parse_condition() {
        // TODO
        return {};
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
    std::optional<Node_if> parse_if() {
        if (!peek().has_value() || peek().value().type != Tokentype::wenn) {
            return {};
        }
        consume(); // "wenn"
        auto Node_cond = parse_condition(); // parse condition and consume the tokens
        if (!peek().has_value() || peek().value().type != Tokentype::wenn) {
            std::cout << "error: expected 'dann'" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume(); // "dann"
        if (!peek().has_value() || peek().value().type != Tokentype::colon) {
            std::cout << "error expected ':" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume(); // ":"
        return Node_if {.condition = Node_cond.value()};
    }
};
