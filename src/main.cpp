#include <csignal>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>
#include "./tokenizer.hpp"






int main(int argc, char* argv[]) {
    // taking input
    if (argc != 2) {
        std::cerr << "incorrect usage!" << std::endl;
        std::cerr << "correct usage: <input.lgk>" << std::endl;
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

    for (int i=0; i<tokens.size(); i++) {
        if (tokens.at(i).type == Tokentype::zurueck) std::cout << "zurueck ";
        if (tokens.at(i).type == Tokentype::int_lit) std::cout << "0";
        if (tokens.at(i).type == Tokentype::semi) std::cout << ";" << std::endl;
    }
    std::stringstream output;
    output << "global _start" << std::endl;
    output << "_start:" << std::endl;
    output << "mov rax, 60" << std::endl;
    output << "mov rdi, 100" << std::endl;
    output << "syscall" << std::endl;

    {
        std::fstream file("out.asm", std::ios::out);
        file << output.str();
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");


    // tokenize the file
    
    return EXIT_SUCCESS;
}
