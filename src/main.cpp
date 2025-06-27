#include <csignal>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <system_error>
#include <utility>
#include <vector>
#include "./tokenizer.hpp"
#include "./parser.hpp"
#include "./generator.hpp"






int main(int argc, char* argv[]) {
    // taking input
    if (argc != 2) {
        std::cerr << "incorrect usage!" << std::endl;
        std::cerr << "correct usage: <input.ko>" << std::endl;
        return EXIT_FAILURE;
    }
    // reading input file
    std::string contents;
    {
        std::stringstream content_stream;
        std::fstream input(argv[1], std::ios::in);
        content_stream << input.rdbuf();
        input.close();
        contents = content_stream.str();
    }

    Tokenizer tokenizer(std::move(contents));
    std::vector<Token> tokens = tokenizer.tokenize();

    Parser parser(std::move(tokens));
    std::optional<Node_return> tree = parser.parse();
    if (!tree.has_value()) {
        std::cerr << "error: unable to parse input, no zurueck statement" << std::endl;
        return EXIT_FAILURE;
    }

    Generator generator(tree.value());

    {
        std::fstream file("out.asm", std::ios::out);
        file << generator.generate();
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");


    // tokenize the file
    
    return EXIT_SUCCESS;
}
