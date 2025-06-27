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
    semi
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
                // check if it is a valid keyword
                if (buf == "zurueck") { // TODO string comparison
                    tokens.push_back({.type = Tokentype::zurueck});
                    buf.clear();
                    continue;
                }
                else {
                    std::cerr << "invalid keyword" << std::endl;
                    std::cerr << buf << std::endl;
                    exit(EXIT_FAILURE);
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
                tokens.push_back({.type = Tokentype::semi});
                consume();
                continue; // TODO maybe increment index
            }
            else if (std::isspace(peek().value())) {
                // if char is whitespace skip
                consume();
                continue;
            }
            else {
                std::cerr << "Invalid syntax!" << std::endl;
                std::cerr << buf << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        // reset indx, incase we want to tokanize again
        m_index = 0;
        return tokens;
    }

private:

    [[nodiscard]] std::optional<char> peek(int ahead = 1) const {
        if (m_index + ahead > m_src.length()) {// TODO maybe >// TODO maybe >==
            return {};
        }
        else {
            return m_src.at(m_index);
        }
    }

    inline char consume() {
        return m_src.at(m_index++);
    }





    const std::string m_src;
    size_t m_index = 0;

};
