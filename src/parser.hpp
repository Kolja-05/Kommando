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

struct Node_bin_expr;

struct Node_expr {
    std::variant<
        std::unique_ptr<Node_expr_int_lit>,
        std::unique_ptr<Node_expr_ident>,
        std::unique_ptr<Node_bin_expr>
    > expr;
};

struct Node_bin_expr_add {
    std::unique_ptr<Node_expr> lhs; // left handside
    std::unique_ptr<Node_expr> rhs; // right handside
};

struct Node_bin_expr_multi {
    std::unique_ptr<Node_expr> lhs; // left handside
    std::unique_ptr<Node_expr> rhs; // right handside
};

struct Node_bin_expr {
    std::variant<
        std::unique_ptr<Node_bin_expr_add>,
        std::unique_ptr<Node_bin_expr_multi> 
    > bin_expr;
};

struct Node_return {
    std::unique_ptr<Node_expr> expr;
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
    std::variant<
        Node_return,
        Node_var_def_assign,
        Node_var_assign,
        Node_goto,
        Node_if>
    stmt;
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
            std::cerr << "error: invalid syntax, kommando needs to start with 'Kommando:" <<std::endl;
            exit(EXIT_FAILURE);
        }
        consume(); // "Kommando""
        if (!peek().has_value() || peek().value().type != Tokentype::colon) {
            std::cerr << "error: expected ':' after Kommando"<<std::endl;
            exit(EXIT_FAILURE);
        }
        consume(); // ":"
        auto node_stmt_list = parse_stmt_list();
        if (!node_stmt_list) {
            std::cerr << "error: expected list of statements (list can be empty)" <<std::endl;
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

    
    std::optional<std::unique_ptr<Node_expr>> parse_prim_expr() {
        //Parses primary expressions like integer literals and identifiers
        if (!peek().has_value()) {
            return {};
        }
        if (peek().value().type == Tokentype::int_lit) {
            auto expr = std::make_unique<Node_expr>();
            auto int_lit =std::make_unique<Node_expr_int_lit>();
            int_lit->int_lit = consume();
            expr->expr = std::move(int_lit);
            return expr;
        }
        if (peek().value().type == Tokentype::identifier) {
            auto expr = std::make_unique<Node_expr>();
            auto ident = std::make_unique<Node_expr_ident>();
            ident->ident.identifier = consume();
            expr->expr = std::move(ident);
            return expr;
        }
        return {};
    }



    std::optional<std::unique_ptr<Node_expr>> parse_bin_expr (std::unique_ptr<Node_expr> lhs) {
        if (!peek().has_value()) {
            if (lhs) {
                return lhs;
            }
            else {
                return {};
            }
        }
        if (peek().value().type == Tokentype::plus) {
            consume(); //"+"
            auto rhs = parse_prim_expr();
            if (!rhs.has_value()) {
                std::cerr << "error: exprected expression after +" << std::endl;
                exit(EXIT_FAILURE);
            }
            auto bin_expr_add = std::make_unique<Node_bin_expr_add>();
            bin_expr_add->lhs = std::move(lhs);
            bin_expr_add->rhs = std::move(rhs.value());

            auto bin_expr = std::make_unique<Node_bin_expr>();
            bin_expr->bin_expr = std::move(bin_expr_add);

            auto expr = std::make_unique<Node_expr>();
            expr->expr = std::move(bin_expr);
            //handle chains like x + y +z
            return parse_bin_expr(std::move(expr));
        }
        if (!lhs) {
            return {};
        }
        return lhs;
    }

    std::optional<std::unique_ptr<Node_expr>> parse_expr() {
        auto lhs = parse_prim_expr();
        if (!lhs) {
            return {};
        }

        auto expr = std::make_unique<Node_expr>();
        expr = std::move(lhs.value());
        return parse_bin_expr(std::move(expr));
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
                std::cerr << "error invalid keyword" << std::endl;
                exit(EXIT_FAILURE);
            }
            return Node_stmt {.stmt = std::move(node_return.value())};
        }
        if (type == Tokentype::sei) {
            auto node_var_def_assign = parse_var_def_assign();
            if (!node_var_def_assign) {
                std::cerr << "error: expected variable definition" << std::endl;
                exit(EXIT_FAILURE);
            }
            return Node_stmt {.stmt = std::move(node_var_def_assign.value())};
        }
        if (type == Tokentype::identifier) {
            auto node_var_assign = parse_var_assign();
            if (!node_var_assign) {
                std::cerr << "error expected variable assignment" << std::endl;
                exit(EXIT_FAILURE);
            }
            return Node_stmt {.stmt = std::move(node_var_assign.value())};
        }
        if (type == Tokentype::springe) {
            auto Node_goto = parse_goto();
            if (!Node_goto) {
                std::cerr <<"error: expected jump" << std::endl;
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
            std::cerr << "error: expected identifier (label) after springe stamtement" << std::endl;
            exit(EXIT_FAILURE);
        }
        Token identifier = consume(); // <label>
        if (!identifier.value.has_value()) {
            std::cerr << "error: identifier token has no value" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (!peek().has_value() || peek().value().type != Tokentype::semicolon) {
            std::cerr << "error: expected ';'" << std::endl;
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
            std::cerr << "error: expected identifier after 'sei'" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (!peek().has_value() || peek().value().type != Tokentype::assign_op) {
            std::cerr << "error: expected '=' after identifier" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume(); // "="

        auto node_expr = parse_expr();
        if (!node_expr) {
            std::cerr << "error: expected expression" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (!peek().has_value() || peek().value().type != Tokentype::semicolon) {
            std::cerr << "error: expected ';'" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume(); // ";"

        return Node_var_def_assign {.identifier = node_ident.value(), .expr = std::move(*node_expr.value())};
    }

    std::optional<Node_var_assign> parse_var_assign() {
        // <var_assign> ::= <var> "=" <expr> ";"
        if (!peek().has_value() || peek().value().type != Tokentype::identifier) {
            return {};
        }
        auto node_ident = parse_ident();
        if (!node_ident) {
            std::cerr << "error: expected identifier" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (!peek().has_value() || peek().value().type != Tokentype::assign_op) {
            std::cerr << "error: expected '=' after identifier" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume(); // "="
        auto node_expr = parse_expr();
        if (!node_expr) {
            std::cerr << "error: expected expression" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (!peek().has_value() || peek().value().type != Tokentype::semicolon) {
            std::cerr << "error: expected ';'" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume(); //";"
        return  Node_var_assign{.identifier = node_ident.value(), .expr = std::move(*node_expr.value())};
    }


    std::optional<Node_return> parse_return() {
        // <return> ::= "zurueck" <expr> ";"
        if (!peek().has_value() || peek().value().type != Tokentype::zurueck) {
            return {};
        }
        consume(); // "zurueck"
        auto node_expr = parse_expr();
        if (!node_expr) {
            std::cerr << "error: expected expression" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (!peek().has_value() || peek().value().type != Tokentype::semicolon) {
            std::cerr << "error: symbol ';'" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume(); // ";"
        return Node_return {.expr = std::move(node_expr.value())};
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
                std::cerr << "error: expected statement and label" << std::endl;
                exit(EXIT_FAILURE);
            }
            std::string label_str = label_identifier.value.value();
            if (m_defined_labels.find(label_str) != m_defined_labels.end()) {
                std::cerr << "error: redefinition of label '" << label_str << "'" <<std::endl;
                exit(EXIT_FAILURE);
            }
            m_defined_labels.insert(label_str);
            return Node_stmt_elem {.stmt = std::move(node_stmt.value()), .label = Node_label{.ident = Node_ident{.identifier = label_identifier}}};
            //creates stmt and label as node_identifier with token label_identifier
        }
        auto node_stmt = parse_stmt();
        if (!node_stmt) {
            std::cerr << "error: expected statement" << std::endl;
            exit(EXIT_FAILURE);
        }
        return Node_stmt_elem {.stmt = std::move(node_stmt.value())}; //TODO
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
        node_stmt_list->stmt_elem = std::move(node_stmt_elem.value());
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
            std::cerr << "error: expected 'dann'" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume(); // "dann"
        if (!peek().has_value() || peek().value().type != Tokentype::colon) {
            std::cerr << "error expected ':" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume(); // ":"
        return Node_if {.condition = Node_cond.value()};
    }
};
