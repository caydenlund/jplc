/**
 * @file lexer.cpp
 * @package Assignment 2
 * @author Cayden Lund (u1182408)
 * @brief Implements the lexing functions.
 * @details See the header file for documentation.
 *
 */

#include "lexer.hpp"

namespace lexer {
    lexing_exception::lexing_exception(const std::string& invalid_input, unsigned int index)
        : runtime_error("There was invalid character sequence: \""
                        + ((invalid_input.size() - index > max_token_snippet_length)
                                   ? invalid_input.substr(index, max_token_snippet_length)
                                   : invalid_input.substr(index, invalid_input.size() - index))
                        + "\" at " + std::to_string(file::get_line(index)) + ":"
                        + std::to_string(file::get_column(index)) + ".") {}

    result_t lexer::operator()(const std::string&, unsigned int) { return {}; }

    bool lexer::is_valid_char(char byte) { return (byte == '\n' || (' ' <= byte && byte <= '~')); }

    result_t comment_lexer::operator()(const std::string& input, unsigned int index) {
        enum state_t {
            START,                   //  Initial state.
            ONE_SLASH,               //  We found a single "/" character.
            IN_SINGLE_LINE_COMMENT,  //  We are inside a single-line comment.
            IN_MULTI_LINE_COMMENT,   //  We are inside a multi-line comment.
            STAR                     //  We are inside a multi-line comment, and we found a "*" character.
        };

        state_t state = START;
        unsigned int current_index = index;
        const unsigned int input_size = input.size();

        while (current_index < input_size) {
            const char current_char = input[current_index];
            if (!this->is_valid_char(current_char)) throw lexing_exception(input, current_index);

            switch (state) {
                case START:
                    if (current_char == '/') {
                        state = ONE_SLASH;
                        current_index++;
                        continue;
                    }
                    break;
                case ONE_SLASH:
                    if (current_char == '/') {
                        state = IN_SINGLE_LINE_COMMENT;
                        current_index++;
                        continue;
                    }
                    if (current_char == '*') {
                        state = IN_MULTI_LINE_COMMENT;
                        current_index++;
                        continue;
                    }
                    break;
                case IN_SINGLE_LINE_COMMENT:
                    if (current_char == '\n') return {token::token {}, current_index};
                    current_index++;
                    continue;
                case IN_MULTI_LINE_COMMENT:
                    if (current_char == '*') { state = STAR; }
                    current_index++;
                    continue;
                case STAR:
                    if (current_char == '/') return {token::token {}, current_index + 1};
                    state = IN_MULTI_LINE_COMMENT;
                    current_index++;
                    continue;
            }
            return {token::token {}, index};
        }

        throw lexing_exception(input, index);
    }

    result_t literal_lexer::operator()(const std::string& input, unsigned int index) {
        enum state_t {
            START,              //  Initial state.
            DIGIT,              //  We have seen a digit.
            DIGIT_AND_DECIMAL,  //  We have seen both a digit and a period (not necessarily in that order).
            DECIMAL             //  We have seen a period, but no digit.
        };

        state_t state = START;
        unsigned int current_index = index;
        const unsigned int input_size = input.size();

        while (current_index < input_size) {
            const char current_char = input[current_index];
            if (!this->is_valid_char(current_char)) throw lexing_exception(input, current_index);

            const bool char_is_digit = ('0' <= current_char && current_char <= '9');
            switch (state) {
                case START:
                    if (char_is_digit) {
                        state = DIGIT;
                        current_index++;
                        continue;
                    }
                    if (current_char == '.') {
                        state = DECIMAL;
                        current_index++;
                        continue;
                    }
                    break;
                case DIGIT:
                    if (char_is_digit) {
                        current_index++;
                        continue;
                    }
                    if (current_char == '.') {
                        state = DIGIT_AND_DECIMAL;
                        current_index++;
                        continue;
                    }
                    return {{index, std::string(input.begin() + index, input.begin() + current_index),
                             token::token_type::INTVAL},
                            current_index};
                case DIGIT_AND_DECIMAL:
                    if (char_is_digit) {
                        current_index++;
                        continue;
                    }
                    return {{index, std::string(input.begin() + index, input.begin() + current_index),
                             token::token_type::FLOATVAL},
                            current_index};
                case DECIMAL:
                    if (char_is_digit) {
                        state = DIGIT_AND_DECIMAL;
                        current_index++;
                        continue;
                    }
                    break;
            }
            return {token::token {}, index};
        }

        return {token::token {}, index};
    }

    result_t newline_lexer::operator()(const std::string& input, unsigned int index) {
        enum state_t {
            START,     //  Initial state.
            BACKSLASH  //  We saw the '\' character.
        };

        state_t state = START;
        unsigned int current_index = index;
        const unsigned int input_size = input.size();

        while (current_index < input_size) {
            const char current_char = input[current_index];
            if (!this->is_valid_char(current_char)) throw lexing_exception(input, current_index);

            switch (state) {
                case START:
                    if (current_char == '\\') {
                        state = BACKSLASH;
                        current_index++;
                        continue;
                    }
                    if (current_char == '\n') {
                        return {{current_index, "\n", token::token_type::NEWLINE}, current_index + 1};
                    }
                    break;
                case BACKSLASH:
                    if (current_char == '\n') { return {token::token {}, current_index + 1}; }
                    break;
            }
            return {token::token {}, index};
        }

        return {token::token {}, index};
    }

    result_t operator_lexer::operator()(const std::string& input, unsigned int index) {
        enum state_t {
            START,        //  Initial state.
            LT,           //  We saw the '<' character.
            GT,           //  We saw the '>' character.
            EXCLAMATION,  //  We saw the '!' character.
            EQ,           //  We saw the '=' character.
            AMP,          //  We saw the '&' character.
            PIPE          //  We saw the '|' character.
        };

        state_t state = START;
        unsigned int current_index = index;
        const unsigned int input_size = input.size();

        while (current_index < input_size) {
            const char current_char = input[current_index];
            if (!this->is_valid_char(current_char)) throw lexing_exception(input, current_index);

            switch (state) {
                case START:
                    switch (current_char) {
                        case '+':
                            return {{index, "+", token::token_type::OP}, current_index + 1};
                        case '-':
                            return {{index, "-", token::token_type::OP}, current_index + 1};
                        case '*':
                            return {{index, "*", token::token_type::OP}, current_index + 1};
                        case '/':
                            return {{index, "/", token::token_type::OP}, current_index + 1};
                        case '%':
                            return {{index, "%", token::token_type::OP}, current_index + 1};
                        case '<':
                            state = LT;
                            current_index++;
                            continue;
                        case '>':
                            state = GT;
                            current_index++;
                            continue;
                        case '!':
                            state = EXCLAMATION;
                            current_index++;
                            continue;
                        case '=':
                            state = EQ;
                            current_index++;
                            continue;
                        case '&':
                            state = AMP;
                            current_index++;
                            continue;
                        case '|':
                            state = PIPE;
                            current_index++;
                            continue;
                        default:
                            break;
                    }
                    break;
                case LT:
                    if (current_char == '=') return {{index, "<=", token::token_type::OP}, current_index + 1};
                    return {{index, "<", token::token_type::OP}, current_index};
                case GT:
                    if (current_char == '=') return {{index, ">=", token::token_type::OP}, current_index + 1};
                    return {{index, ">", token::token_type::OP}, current_index};
                case EXCLAMATION:
                    if (current_char == '=') return {{index, "!=", token::token_type::OP}, current_index + 1};
                    return {{index, "!", token::token_type::OP}, current_index};
                case EQ:
                    if (current_char == '=') return {{index, "==", token::token_type::OP}, current_index + 1};
                    break;
                case AMP:
                    if (current_char == '&') return {{index, "&&", token::token_type::OP}, current_index + 1};
                    break;
                case PIPE:
                    if (current_char == '|') return {{index, "||", token::token_type::OP}, current_index + 1};
                    break;
            }
            return {token::token {}, index};
        }

        return {token::token {}, index};
    }

    result_t punctuation_lexer::operator()(const std::string& input, unsigned int index) {
        const char current_char = input[index];
        if (!this->is_valid_char(current_char)) throw lexing_exception(input, index);

        switch (current_char) {
            case ' ':
                return {token::token {}, index + 1};
            case ':':
                return {{index, ":", token::token_type::COLON}, index + 1};
            case '{':
                return {{index, "{", token::token_type::LCURLY}, index + 1};
            case '}':
                return {{index, "}", token::token_type::RCURLY}, index + 1};
            case '(':
                return {{index, "(", token::token_type::LPAREN}, index + 1};
            case ')':
                return {{index, ")", token::token_type::RPAREN}, index + 1};
            case ',':
                return {{index, ",", token::token_type::COMMA}, index + 1};
            case '[':
                return {{index, "[", token::token_type::LSQUARE}, index + 1};
            case ']':
                return {{index, "]", token::token_type::RSQUARE}, index + 1};
            case '=':
                return {{index, "=", token::token_type::EQUALS}, index + 1};
            default:
                break;
        }

        return {token::token {}, index};
    }

    result_t string_lexer::operator()(const std::string& input, unsigned int index) {
        enum state_t {
            START,     //  Initial state.
            IN_STRING  //  The first character was '"'; this is a string.
        };

        state_t state = START;
        unsigned int current_index = index;
        const unsigned int input_size = input.size();

        while (current_index < input_size) {
            const char current_char = input[current_index];
            if (!this->is_valid_char(current_char)) throw lexing_exception(input, current_index);

            switch (state) {
                case START:
                    if (current_char == '"') {
                        state = IN_STRING;
                        current_index++;
                        continue;
                    }
                    break;
                case IN_STRING:
                    if (current_char == '"') {
                        return {{index, std::string(input.begin() + index, input.begin() + current_index + 1),
                                 token::token_type::STRING},
                                current_index + 1};
                    }
                    if (current_char == '\n') return {token::token {}, index};
                    current_index++;
                    continue;
            }
            return {token::token {}, index};
        }

        return {token::token {}, index};
    }

    result_t variable_keyword_lexer::operator()(const std::string& input, unsigned int index) {
        enum state_t {
            START,  //  Initial state.
            LETTER  //  The first character was a letter; this is a valid variable or keyword.
        };

        state_t state = START;
        unsigned int current_index = index;
        const unsigned int input_size = input.size();

        while (current_index < input_size) {
            const char current_char = input[current_index];
            if (!this->is_valid_char(current_char)) throw lexing_exception(input, current_index);

            const bool current_is_letter = (('a' <= current_char && current_char <= 'z')
                                            || ('A' <= current_char && current_char <= 'Z'));
            const bool current_is_valid_char = current_is_letter || ('0' <= current_char && current_char <= '9')
                                            || current_char == '_' || current_char == '.';
            switch (state) {
                case START:
                    if (current_is_letter) {
                        state = LETTER;
                        current_index++;
                        continue;
                    }
                    break;
                case LETTER:
                    if (current_is_valid_char) {
                        current_index++;
                        continue;
                    }
                    //  Otherwise, this is the end of the keyword or variable.
                    const std::string whole_word(input.begin() + index, input.begin() + current_index);
                    token::token_type word_type = token::token_type::VARIABLE;
                    if (whole_word == "array") {
                        word_type = token::token_type::ARRAY;
                    } else if (whole_word == "assert") {
                        word_type = token::token_type::ASSERT;
                    } else if (whole_word == "bool") {
                        word_type = token::token_type::BOOL;
                    } else if (whole_word == "else") {
                        word_type = token::token_type::ELSE;
                    } else if (whole_word == "false") {
                        word_type = token::token_type::FALSE;
                    } else if (whole_word == "float") {
                        word_type = token::token_type::FLOAT;
                    } else if (whole_word == "fn") {
                        word_type = token::token_type::FN;
                    } else if (whole_word == "if") {
                        word_type = token::token_type::IF;
                    } else if (whole_word == "image") {
                        word_type = token::token_type::IMAGE;
                    } else if (whole_word == "int") {
                        word_type = token::token_type::INT;
                    } else if (whole_word == "let") {
                        word_type = token::token_type::LET;
                    } else if (whole_word == "print") {
                        word_type = token::token_type::PRINT;
                    } else if (whole_word == "read") {
                        word_type = token::token_type::READ;
                    } else if (whole_word == "return") {
                        word_type = token::token_type::RETURN;
                    } else if (whole_word == "show") {
                        word_type = token::token_type::SHOW;
                    } else if (whole_word == "sum") {
                        word_type = token::token_type::SUM;
                    } else if (whole_word == "then") {
                        word_type = token::token_type::THEN;
                    } else if (whole_word == "time") {
                        word_type = token::token_type::TIME;
                    } else if (whole_word == "to") {
                        word_type = token::token_type::TO;
                    } else if (whole_word == "true") {
                        word_type = token::token_type::TRUE;
                    } else if (whole_word == "type") {
                        word_type = token::token_type::TYPE;
                    } else if (whole_word == "write") {
                        word_type = token::token_type::WRITE;
                    }
                    return {{index, whole_word, word_type}, current_index};
            }
            return {token::token {}, index};
        }

        return {token::token {}, index};
    }

    std::vector<lexer_ptr_t> assemble_lexers() {
        std::vector<lexer_ptr_t> lexers;

        //  Strings.
        lexers.emplace_back(std::make_shared<string_lexer>(string_lexer()));

        //  Newlines.
        lexers.emplace_back(std::make_shared<newline_lexer>(newline_lexer()));

        //  Comments.
        lexers.emplace_back(std::make_shared<comment_lexer>(comment_lexer()));

        //  Operators.
        lexers.emplace_back(std::make_shared<operator_lexer>(operator_lexer()));

        //  Punctuation.
        lexers.emplace_back(std::make_shared<punctuation_lexer>(punctuation_lexer()));

        //  Integer and floating-point literals.
        lexers.emplace_back(std::make_shared<literal_lexer>(literal_lexer()));

        //  Variables and keywords.
        lexers.emplace_back(std::make_shared<variable_keyword_lexer>(variable_keyword_lexer()));

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

            //  Try every lexer, one after the other, in order, until we find one that finds a match.
            for (const lexer_ptr_t& one_lexer : lexers) {
                result_t result = (*one_lexer)(input, start);
                const unsigned int new_start = std::get<1>(result);
                if (new_start != start) {
                    //  The lexer succeeded.
                    //  Add the new token to the vector, unless the token is empty.
                    //  Also, don't add a second consecutive newline.
                    const token::token new_token = std::get<0>(result);
                    if (!new_token.text.empty()
                        && (new_token.type != token::token_type::NEWLINE || tokens.empty()
                            || tokens[tokens.size() - 1].type != token::token_type::NEWLINE))
                        tokens.push_back(new_token);

                    valid_token = true;
                    start = new_start;
                    break;
                }
                //  Otherwise, try the next lexer.
            }

            //  If we tried every lexer and weren't able to move on, then we need to throw an exception.
            if (!valid_token) throw lexing_exception(input, start);
        }

        //  Add the final token:
        tokens.push_back({start, "", token::token_type::END_OF_FILE});

        return tokens;
    }
}  //  namespace lexer
