#pragma once










#include "tokenizer.hpp"
#include <cstdlib>
#include <optional>
#include <vector>


struct Node_expr {
    Token int_lit; //TODO enforce value
};

struct Node_return {
    Node_expr expr;
};


class Parser {
public:
    inline explicit Parser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens)) {
    }




    std::optional<Node_expr> parse_expr() {
        // <expr> ::= <int_lit>
        // TODO    | <int_lit> <arit_op> <int_lit>
        if (peek().has_value() && peek().value().type == Tokentype::int_lit) {
            return Node_expr{.int_lit = consume()};
        } else {
            return {};
        }
    }



    std::optional<Node_return> parse() {
        // <return> ::= "zurueck" <expr> ";"
        Node_return return_node; // NOTE this is not named return node, because we are returning it, but because the return node is the entry node
        while (peek().has_value()) {
            if (peek().value().type == Tokentype::zurueck) {
                consume();
                if (auto node_expr = parse_expr()) { // condition is true if Node_expr has value
                    return_node = Node_return{.expr = node_expr.value()};
                } else {
                    std::cout << "Invalid expression" << std::endl;
                    exit(EXIT_FAILURE);
                }
                if (peek().has_value() && peek().value().type == Tokentype::semi) {
                    consume();
                }else {
                    // missing semicolon
                    std::cout << "error: expected symbol ';'" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
        }
        m_index = 0;
        return return_node;
    }



private:
    [[nodiscard]] std::optional<Token> peek(int ahead = 1) const {
        if (m_index + ahead > m_tokens.size()) {// TODO maybe >// TODO maybe >==
            return {};
        }
        else {
            return m_tokens.at(m_index);
        }
    }

    inline Token consume() {
        return m_tokens.at(m_index++);
    }

    const std::vector<Token> m_tokens;
    const std::string m_src;
    size_t m_index = 0;
};
