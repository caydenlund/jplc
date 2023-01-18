/**
 * @file lexer.hpp
 * @package Assignment 2
 * @author Cayden Lund (u1182408)
 * @brief Implements the lexing functions.
 * @details See the header file for documentation.
 *
 */

#include "lexer.hpp"

//  TODO: Remove.
#include <iostream>

namespace lexer {
    lexer::lexer(const std::string& pattern, token::token_type type) : pattern(pattern), type(type) {}

    std::tuple<token::token, unsigned int> lexer::operator()(const std::string& input, unsigned int index) {
        std::smatch match;
        if (std::regex_search(input.begin() + index, input.end(), match, this->pattern)) {
            return {{index, match[0], this->type}, index + match[0].length()};
        } else {
            return {token::token(), index};
        }
    }

    std::vector<lexer> assemble_lexers() {
        std::vector<lexer> lexers;

        //  Keywords:
        //  ---------
        lexers.emplace_back("^array", token::token_type::ARRAY);    //  The keyword "array".
        lexers.emplace_back("^assert", token::token_type::ASSERT);  //  The keyword "assert".
        lexers.emplace_back("^bool", token::token_type::BOOL);      //  The keyword "bool".
        lexers.emplace_back("^else", token::token_type::ELSE);      //  The keyword "else".
        lexers.emplace_back("^false", token::token_type::FALSE);    //  The keyword "false".
        lexers.emplace_back("^float", token::token_type::FLOAT);    //  The keyword "float".
        lexers.emplace_back("^fn", token::token_type::FN);          //  The keyword "fn".
        lexers.emplace_back("^if", token::token_type::IF);          //  The keyword "if".
        lexers.emplace_back("^image", token::token_type::IMAGE);    //  The keyword "image".
        lexers.emplace_back("^int", token::token_type::INT);        //  The keyword "int".
        lexers.emplace_back("^let", token::token_type::LET);        //  The keyword "let".
        lexers.emplace_back("^print", token::token_type::PRINT);    //  The keyword "print".
        lexers.emplace_back("^read", token::token_type::READ);      //  The keyword "read".
        lexers.emplace_back("^return", token::token_type::RETURN);  //  The keyword "return".
        lexers.emplace_back("^show", token::token_type::SHOW);      //  The keyword "show".
        lexers.emplace_back("^sum", token::token_type::SUM);        //  The keyword "sum".
        lexers.emplace_back("^then", token::token_type::THEN);      //  The keyword "then".
        lexers.emplace_back("^time", token::token_type::TIME);      //  The keyword "time".
        lexers.emplace_back("^to", token::token_type::TO);          //  The keyword "to".
        lexers.emplace_back("^true", token::token_type::TRUE);      //  The keyword "true".
        lexers.emplace_back("^type", token::token_type::TYPE);      //  The keyword "type".
        lexers.emplace_back("^write", token::token_type::WRITE);    //  The keyword "write".

        return lexers;
    }

    std::vector<token::token> lex_all(const std::string& input) {
        //  First, assemble the set of lexers in the correct order.
        //  These will be used one by one.
        std::vector<lexer> lexers = assemble_lexers();

        std::vector<token::token> tokens;
        return tokens;
    }
}  //  namespace lexer
