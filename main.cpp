/**
 * @file main.cpp
 * @author Cayden Lund (u1182408)
 * @brief Main program entry point.
 *
 */

#include <iostream>
#include <string>

#include "ast_node/ast_node.hpp"
#include "file/file.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "token/token.hpp"

/**
 * @brief Only lexes the input file.
 *
 * @param filename The file to read.
 * @return 0 on success; an exception is thrown otherwise.
 */
int lex_only(const std::string& filename) {
    const lexer::token_list_t tokens = lexer::lex_all(file::read_file(filename));

    for (const lexer::token_ptr_t& token : tokens) {
        std::cout << token::token_type_to_string(token->type);
        if (token->type != token::token_type::NEWLINE && token->type != token::token_type::END_OF_FILE)
            std::cout << " '" << token->text << "'";
        std::cout << "\n";
    }

    std::cout << "Compilation succeeded: lexical analysis complete\n";

    return 0;
}

/**
 * @brief Only lexes and parses the input file.
 *
 * @param filename The file to read.
 * @return 0 on success; an exception is thrown otherwise.
 */
int lex_and_parse_only(const std::string& filename) {
    const lexer::token_list_t tokens = lexer::lex_all(file::read_file(filename));
    const std::vector<parser::node_ptr_t> nodes = parser::parse(tokens);

    for (const parser::node_ptr_t& node : nodes) { std::cout << node->s_expression() << "\n"; }

    std::cout << "Compilation succeeded: parsing complete\n";

    return 0;
}

/**
 * @brief Only lexes, parses, and type-checks the input file.
 * @details TODO: Implement.
 *
 * @param filename The file to read.
 * @return 0 on success; an exception is thrown otherwise.
 */
int lex_parse_and_check_only(const std::string&) {
    std::cout << "Compilation failed\n";
    return 1;
}

/**
 * @brief Main program entry point.
 *
 * @return 0 on success; 1 on error.
 */
int main(int argc, char** argv) {
    std::string filename;
    bool debug = false;
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
        else if (arg == "--debug")
            debug = true;
        else
            filename = arg;
    }

    if (filename.empty()) {
        //  No filename was given.
        std::cout << "Compilation failed\n";
        return 1;
    }

    try {
        if (lex_parse_and_check_only_flag) return lex_parse_and_check_only(filename);
        if (lex_and_parse_only_flag) return lex_and_parse_only(filename);
        if (lex_only_flag) return lex_only(filename);

        //  If we've made it this far, then we perform the whole compilation.
        //  This is not yet implemented; quit.
        std::cout << "Compilation failed\n";
        return 1;
    } catch (const std::exception& err) {
        //  The lexer, parser, and type checker may throw an exception.
        //  Catch it, print "Compilation failed", and quit.
        std::cout << "Compilation failed\n";
        //  If the debug flag was given, then also print the exception message.
        if (debug) std::cerr << "An exception was thrown: \"" << err.what() << "\"" << std::endl;
        return 1;
    }
}
