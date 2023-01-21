/**
 * @file main.cpp
 * @author Cayden Lund (u1182408)
 * @brief Main program entry point.
 *
 */

#include <iostream>
#include <string>

#include "file/file.hpp"
#include "lexer/lexer.hpp"
#include "token/token.hpp"

int lex_only(const std::string& filename) {
    try {
        const lexer::token_list_t tokens = lexer::lex_all(file::read_file(filename));

        for (const lexer::token_ptr_t& token : tokens) {
            std::cout << token::token_type_to_string(token->type);
            if (token->type != token::token_type::NEWLINE && token->type != token::token_type::END_OF_FILE)
                std::cout << " '" << token->text << "'";
            std::cout << "\n";
        }

        std::cout << "Compilation succeeded: lexical analysis complete\n";

        return 0;
    } catch (const std::exception& e) {
        std::cout << "Compilation failed\n";
        std::cerr << "Exception emitted: \"" << e.what() << "\"" << std::endl;
        return 1;
    }
}

int lex_and_parse_only(const std::string&) {
    //  TODO: Implement.

    std::cout << "Compilation failed\n";
    return 1;
}

int lex_parse_and_check_only(const std::string&) {
    //  TODO: Implement.

    std::cout << "Compilation failed\n";
    return 1;
}

/**
 * @brief Main program entry point.
 *
 * @return 0 on success; 1 on error.
 */
int main(int argc, char** argv) {
    //  TODO: Parse options more elegantly.
    if (argc == 1) {
        std::cout << "Compilation failed\n";
        return 1;
    }

    std::string filename;
    bool lex_only_flag = false;
    bool lex_and_parse_only_flag = false;
    bool lex_parse_and_check_only_flag = false;
    for (unsigned int index = 1; index < (unsigned int)argc; index++) {
        const std::string arg(argv[index]);
        if (arg == "-l") lex_only_flag = true;
        else if (arg == "-p")
            lex_and_parse_only_flag = true;
        else if (arg == "-t")
            lex_parse_and_check_only_flag = true;
        else
            filename = arg;
    }

    if (filename.empty()) {
        std::cout << "Compilation failed\n";
        return 1;
    }

    if (lex_parse_and_check_only_flag) return lex_parse_and_check_only(filename);

    if (lex_and_parse_only_flag) return lex_and_parse_only(filename);

    if (lex_only_flag) return lex_only(filename);

    //  Temporarily always just lex the input file.
    return lex_only(filename);
}
