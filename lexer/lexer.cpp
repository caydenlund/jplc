/**
 * @file lexer.hpp
 * @package Assignment 2
 * @author Cayden Lund (u1182408)
 * @brief Implements the lexing functions.
 * @details See the header file for documentation.
 *
 */

#include "lexer.hpp"

namespace lexer {
    lexing_exception::lexing_exception(const std::string& invalid_token, unsigned int line, unsigned int column)
        : runtime_error("There was an invalid character sequence."),
          message("There was invalid character sequence: \"" + invalid_token + "\" at " + std::to_string(line) + ":"
                  + std::to_string(column) + ".") {}

    const char* lexing_exception::what() const noexcept { return this->message.c_str(); }

    lexer::lexer(const std::string& pattern, token::token_type type) : pattern(pattern), type(type) {}

    result_t lexer::operator()(const std::string& input, unsigned int index) const {
        std::smatch match;

        if (std::regex_search(input.begin() + index, input.end(), match, this->pattern)) {
            //  This creates a smart pointer to a new `token` object.
            const token_ptr_t new_token = std::make_shared<token::token>(token::token {index, match[0], this->type});
            return {new_token, index + (unsigned int)match[0].length()};
        }

        return {std::make_shared<token::token>(), index};
    }

    float_lexer::float_lexer() : lexer(R"((^\d+\.\d*)|(^\d*\.\d+))", token::token_type::FLOATVAL) {}

    result_t float_lexer::operator()(const std::string& input, unsigned int index) const {
        std::smatch match;

        if (std::regex_search(input.begin() + index, input.end(), match, this->pattern)) {
            const double value = std::stod(match[0]);
            //  This creates a smart pointer to a new `float_token` object.
            const token_ptr_t new_token = std::make_shared<token::float_token>(
                    token::float_token {{index, match[0], this->type}, value});
            return {new_token, index + (unsigned int)match[0].length()};
        }

        return {std::make_shared<token::token>(), index};
    }

    int_lexer::int_lexer() : lexer("^\\d+", token::token_type::INTVAL) {}

    result_t int_lexer::operator()(const std::string& input, unsigned int index) const {
        std::smatch match;

        if (std::regex_search(input.begin() + index, input.end(), match, this->pattern)) {
            const long value = std::stol(match[0]);
            //  This creates a smart pointer to a new `int_token` object.
            const token_ptr_t new_token = std::make_shared<token::int_token>(
                    token::int_token {{index, match[0], this->type}, value});
            return {new_token, index + (unsigned int)match[0].length()};
        }

        return {std::make_shared<token::token>(), index};
    }

    string_lexer::string_lexer() : lexer(R"(^"((\\")|([^"]))*")", token::token_type::STRING) {}

    result_t string_lexer::operator()(const std::string& input, unsigned int index) const {
        std::smatch match;

        if (std::regex_search(input.begin() + index, input.end(), match, this->pattern)) {
            const std::string value = std::string(match[0]).substr(1, match[0].length() - 2);
            //  This creates a smart pointer to a new `string_token` object.
            const token_ptr_t new_token = std::make_shared<token::string_token>(
                    token::string_token {{index, match[0], this->type}, value});
            return {new_token, index + (unsigned int)match[0].length()};
        }

        return {std::make_shared<token::token>(), index};
    }

    std::vector<lexer_ptr_t> assemble_lexers() {
        std::vector<lexer_ptr_t> lexers;

        //  Literals:
        //  ---------
        lexers.emplace_back(std::make_shared<float_lexer>(float_lexer()));    //  Floating-point literals.
        lexers.emplace_back(std::make_shared<int_lexer>(int_lexer()));        //  Integer literals.
        lexers.emplace_back(std::make_shared<string_lexer>(string_lexer()));  //  String literals.

        //  Characters:
        //  -----------
        lexers.emplace_back(construct_lexer("^ ", token::token_type::SPACE));      //  The character ' '.
        lexers.emplace_back(construct_lexer("^\n", token::token_type::NEWLINE));   //  The character '\n'.
        lexers.emplace_back(construct_lexer("^:", token::token_type::COLON));      //  The character ':'.
        lexers.emplace_back(construct_lexer("^,", token::token_type::COMMA));      //  The character ','.
        lexers.emplace_back(construct_lexer("^=", token::token_type::EQUALS));     //  The character '='.
        lexers.emplace_back(construct_lexer("^\\{", token::token_type::LCURLY));   //  The character '{'.
        lexers.emplace_back(construct_lexer("^\\}", token::token_type::RCURLY));   //  The character '}'.
        lexers.emplace_back(construct_lexer("^\\(", token::token_type::LPAREN));   //  The character '('.
        lexers.emplace_back(construct_lexer("^\\)", token::token_type::RPAREN));   //  The character ')'.
        lexers.emplace_back(construct_lexer("^\\[", token::token_type::LSQUARE));  //  The character '['.
        lexers.emplace_back(construct_lexer("^\\]", token::token_type::RSQUARE));  //  The character ']'.

        //  Variables:
        //  ----------
        //  The regular expression for this one is a little involved.
        std::stringstream variable_regex;
        //  First off, it must not be a keyword:
        variable_regex << "(?!^(";
        for (const std::string keyword :
             {"array", "assert", "bool",   "else", "false", "float", "fn",   "if", "image", "int", "let",
              "print", "read",   "return", "show", "sum",   "then",  "time", "to", "true",  "type"}) {
            variable_regex << keyword << "|";
        }
        variable_regex << "write)"
                       << R"(([^\w\.]|$)))";
        //  Next, the first character must be a letter:
        variable_regex << "^[a-zA-Z]";
        //  After that, any sequence of letters, numbers, underscores, and periods:
        variable_regex << R"([\w\.]*)";
        lexers.emplace_back(construct_lexer(variable_regex.str(), token::token_type::VARIABLE));  //  Variables.

        //  Keywords:
        //  ---------
        lexers.emplace_back(construct_lexer("^array", token::token_type::ARRAY));    //  The keyword "array".
        lexers.emplace_back(construct_lexer("^assert", token::token_type::ASSERT));  //  The keyword "assert".
        lexers.emplace_back(construct_lexer("^bool", token::token_type::BOOL));      //  The keyword "bool".
        lexers.emplace_back(construct_lexer("^else", token::token_type::ELSE));      //  The keyword "else".
        lexers.emplace_back(construct_lexer("^false", token::token_type::FALSE));    //  The keyword "false".
        lexers.emplace_back(construct_lexer("^float", token::token_type::FLOAT));    //  The keyword "float".
        lexers.emplace_back(construct_lexer("^fn", token::token_type::FN));          //  The keyword "fn".
        lexers.emplace_back(construct_lexer("^if", token::token_type::IF));          //  The keyword "if".
        lexers.emplace_back(construct_lexer("^image", token::token_type::IMAGE));    //  The keyword "image".
        lexers.emplace_back(construct_lexer("^int", token::token_type::INT));        //  The keyword "int".
        lexers.emplace_back(construct_lexer("^let", token::token_type::LET));        //  The keyword "let".
        lexers.emplace_back(construct_lexer("^print", token::token_type::PRINT));    //  The keyword "print".
        lexers.emplace_back(construct_lexer("^read", token::token_type::READ));      //  The keyword "read".
        lexers.emplace_back(construct_lexer("^return", token::token_type::RETURN));  //  The keyword "return".
        lexers.emplace_back(construct_lexer("^show", token::token_type::SHOW));      //  The keyword "show".
        lexers.emplace_back(construct_lexer("^sum", token::token_type::SUM));        //  The keyword "sum".
        lexers.emplace_back(construct_lexer("^then", token::token_type::THEN));      //  The keyword "then".
        lexers.emplace_back(construct_lexer("^time", token::token_type::TIME));      //  The keyword "time".
        lexers.emplace_back(construct_lexer("^to", token::token_type::TO));          //  The keyword "to".
        lexers.emplace_back(construct_lexer("^true", token::token_type::TRUE));      //  The keyword "true".
        lexers.emplace_back(construct_lexer("^type", token::token_type::TYPE));      //  The keyword "type".
        lexers.emplace_back(construct_lexer("^write", token::token_type::WRITE));    //  The keyword "write".

        return lexers;
    }

    token_list_t lex_all(const std::string& input) {
        //  First, assemble the set of lexers in the correct order.
        //  These will be used one by one.
        const std::vector<lexer_ptr_t> lexers = assemble_lexers();

        token_list_t tokens;

        unsigned int start = 0;
        bool valid_token;
        while (start < input.size()) {
            valid_token = false;
            for (const lexer_ptr_t& one_lexer : lexers) {
                result_t result = (*one_lexer)(input, start);
                const unsigned int new_start = std::get<1>(result);
                if (new_start != start) {
                    //  The lexer succeeded.
                    //  Add the new token to the vector, unless the token is a space.
                    const token_ptr_t new_token = std::get<0>(result);
                    if (new_token->type != token::token_type::SPACE) tokens.push_back(new_token);

                    valid_token = true;
                    start = new_start;
                    break;
                }
                //  Otherwise, try the next lexer.
            }
            //  If we tried every lexer and weren't able to move on, then we need to throw an exception.
            const unsigned int token_length = input.length() - start;
            std::string invalid_token;
            if (token_length > lexing_exception::max_token_snippet_length) {
                invalid_token = input.substr(start, lexing_exception::max_token_snippet_length) + "...";
            } else {
                invalid_token = input.substr(start, token_length);
            }
            //  TODO: Extract the actual line and column.
            if (!valid_token) { throw lexing_exception(invalid_token, 0, 0); }
        }

        return tokens;
    }
}  //  namespace lexer
