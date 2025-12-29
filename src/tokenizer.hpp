#pragma once

#include <cctype>
#include <cstdlib>
#include <optional>
#include <string>
#include <vector>
#include <iostream>



enum class Tokentype {
    zurueck,
    int_lit,
    semicolon,
    identifier,
    sei,
    assign_op,      // =
    comp_eq,        // ==
    comp_neq,       // !=
    comp_geq,       // >=
    comp_leq,       // <=
    comp_greater,   // >
    comp_less,      // <
    kommando_entry,
    colon,
    springe,
    wenn,
    dann,
    ende,
    sonst,
    wahr,
    falsch,
    plus,
    minus,
    times,
    division,
    modulo,
};

struct Token {
    Tokentype type;
    std::optional<std::string> value;
};


class Tokenizer {
public:
    inline explicit Tokenizer(const std::string src)
        : m_src (std::move(src)) {
    }

    /* turns input file into a list/vector of tokens */
    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        std::string buf;
        while (peek().has_value()) {
            if (std::isalpha(peek().value())) {
                // if char is alphabetical check for further alphanumerical letters
                buf.push_back(consume());
                while (peek().has_value() && std::isalnum(peek().value())) {
                    buf.push_back(consume());
                }
                // check if it is a return-keyword
                if (buf == "zurueck") {
                    tokens.push_back({.type = Tokentype::zurueck});
                    buf.clear();
                    continue;
                }
                else if (buf == "sei") {
                    // check if its a sei-keyword
                    tokens.push_back({.type = Tokentype::sei});
                    buf.clear();
                    continue;
                }
                else if (buf == "Kommando") {
                    tokens.push_back({.type = Tokentype::kommando_entry});
                    buf.clear();
                    continue;
                }
                else if (buf == "springe") {
                    tokens.push_back({.type = Tokentype::springe});
                    buf.clear();
                    continue;
                }else if (buf == "wenn") {
                    tokens.push_back({.type = Tokentype::wenn});
                    buf.clear();
                    continue;
                }else if (buf == "dann") {
                    tokens.push_back({.type = Tokentype::dann});
                    buf.clear();
                    continue;
                }else if (buf == "ende") {
                    tokens.push_back({.type = Tokentype::ende});
                    buf.clear();
                    continue;
                }else if (buf == "sonst") {
                    tokens.push_back({.type = Tokentype::sonst});
                    buf.clear();
                    continue;
                }
                else if (buf == "wahr") {
                    tokens.push_back({.type = Tokentype::wahr});
                    buf.clear();
                    continue;
                }
                else {
                    tokens.push_back({.type = Tokentype::identifier, .value = buf});
                    buf.clear();
                    continue; ;
                }
            }
            else if (std::isdigit(peek().value())) {
                // if char is a digit check for further digits
                buf.push_back(consume());
                while (peek().has_value() && std::isdigit(peek().value())) {
                    buf.push_back(consume());
                }
                tokens.push_back({.type = Tokentype::int_lit, .value = buf});
                buf.clear();
                continue;
            }
            else if (peek().value() == ';') {
                // if char is a ; tokenize it
                tokens.push_back({.type = Tokentype::semicolon});
                consume();
                continue;
            }
            else if (peek().value() == ':') {
                tokens.push_back({.type = Tokentype::colon});
                consume();
                continue;
            }
            else if (peek().value() =='=') {
                if (peek(1).has_value() && peek(1).value() == '=') {
                    tokens.push_back({.type = Tokentype::comp_eq});
                    consume();// '='
                    consume(); //'='
                }
                else {
                    tokens.push_back({.type = Tokentype::assign_op});
                    consume(); // '='
                }
                continue;
            }
            else if (peek().value() == '!' && peek(1).has_value() && peek(1).value() == '=') {
                tokens.push_back({.type = Tokentype::comp_neq});
                consume(); // '!'
                consume(); // '='
            }
            else if (peek().value() == '<') {
                if (peek(1).has_value() && peek(1).value() == '=') {
                    tokens.push_back({.type = Tokentype::comp_leq});
                    consume(); //'='
                }
                else {
                    tokens.push_back({.type = Tokentype::comp_less});
                    consume();
                }
                continue;
            }
            else if (peek().value() == '>') {
                if (peek(1).has_value() && peek(1).value() == '=') {
                    tokens.push_back({.type = Tokentype::comp_geq});
                    consume(); //'='
                }
                else {
                    tokens.push_back({.type = Tokentype::comp_greater});
                    consume();
                }
                continue;
            }
            else if (peek().value() == '+') {
                tokens.push_back({.type = Tokentype::plus});
                consume();
                continue;
            }
            // TODO add other arithmetix operators
            else if (std::isspace(peek().value())) {
                // if char is whitespace skip
                consume();
                continue;
            }
            else {
                std::cerr << "Invalid Token!" << peek().value() << std::endl;
                std::cerr << buf << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        // reset index, incase we want to tokanize again
        m_index = 0;
        return tokens;
    }

private:

    [[nodiscard]] std::optional<char> peek(int offset = 0) const {
        if (m_index + offset >= m_src.length()) {
            return {};
        }
        else {
            return m_src.at(m_index + offset);
        }
    }

    inline char consume() {
        return m_src.at(m_index++); // returns character at index and increments it afterwards
    }

    const std::string m_src;
    size_t m_index = 0;
};
