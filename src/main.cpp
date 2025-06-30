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
    // first argument is input second output file
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
    std::optional<Node_program> progam_root = parser.parse_program();
    if (!progam_root.has_value()) {
        std::cerr << "error: unable to parse input" << std::endl;
        return EXIT_FAILURE;
    }

    Generator generator(std::move(progam_root.value()), std::move(parser.get_defined_labels()));

    {
        std::fstream file("out.asm", std::ios::out);
        file << generator.generate();
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");


    // tokenize the file
    
    return EXIT_SUCCESS;
}
