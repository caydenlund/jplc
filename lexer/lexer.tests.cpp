/**
 * @file lexer.tests.cpp
 * @package Assignment 2.
 * @author Cayden Lund (u1182408)
 * @brief Contains tests for the lexer.
 *
 */

#include "lexer.hpp"
#include "tests/tests.hpp"

namespace tests::lexer_tests {
    /**
     * @brief Returns the length of the given string as an `unsigned int`.
     * @details This function is explicitly inline.
     *
     * @param input The input string.
     * @return The length of the input string.
     */
    inline unsigned int strlen(const std::string& input) { return (unsigned int)input.length(); }

    /**
     * @brief Ensures that the `lexer` constructor does not throw an exception.
     *
     * @return The empty string if successful; an exception is thrown otherwise.
     */
    std::string constructor_no_fail() {
        const lexer::lexer empty_pattern("", token::token_type::ARRAY);
        const lexer::lexer valid_regex("^abc.*", token::token_type::ARRAY);
        const lexer::lexer invalid_regex(R"(\a\b\c\d)", token::token_type::ARRAY);

        return "";
    }

    /**
     * @brief Ensures that a lexer does not match an input that it should not match.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lexer_no_match() {
        const lexer::lexer array_matcher("^array", token::token_type::ARRAY);
        lexer::result_t result = array_matcher("other phrase", 0);

        if (std::get<1>(result) != 0) return "Lexer matched an input that it should not match";

        return "";
    }

    /**
     * @brief Ensures that a lexer does match an input that it should match.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lexer_match() {
        const lexer::lexer array_matcher("^array", token::token_type::ARRAY);
        lexer::result_t result = array_matcher("array other phrase", 0);

        if (std::get<1>(result) != strlen("array")) return "Lexer did not match an input that it should match";

        const token::token actual = *std::get<0>(result);
        const token::token expected {0, "array", token::token_type::ARRAY};
        if (actual != expected) return "The returned token was not correct.";

        return "";
    }

    /**
     * @brief Ensures that the `lexer::assemble_lexers` method does not throw an exception.
     *
     * @return The empty string if successful; an exception is thrown otherwise.
     */
    std::string assemble_lexers_no_fail() {
        lexer::assemble_lexers();

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method can correctly lex a series of keywords.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_keywords() {
        lexer::token_list_t tokens = lexer::lex_all("array bool assert else");

        std::vector<token::token> expected;
        token::token tok = {0, "array", token::token_type::ARRAY};
        expected.push_back(tok);
        tok = {strlen("array "), "bool", token::token_type::BOOL};
        expected.push_back(tok);
        tok = {strlen("array bool "), "assert", token::token_type::ASSERT};
        expected.push_back(tok);
        tok = {strlen("array bool assert "), "else", token::token_type::ELSE};
        expected.push_back(tok);

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";
        for (unsigned int i = 0; i < tokens.size(); i++) {
            if (*tokens[i] != expected[i]) return "Token " + std::to_string(i) + " was not correct";
        }

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method correctly lexes all punctuation marks.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_punctuation() {
        lexer::token_list_t tokens = lexer::lex_all(": , ( {{");

        std::vector<token::token> expected;
        token::token tok = {0, ":", token::token_type::COLON};
        expected.push_back(tok);
        tok = {strlen(": "), ",", token::token_type::COMMA};
        expected.push_back(tok);
        tok = {strlen(": , "), "(", token::token_type::LPAREN};
        expected.push_back(tok);
        tok = {strlen(": , ( "), "{", token::token_type::LCURLY};
        expected.push_back(tok);
        tok = {strlen(": , ( {"), "{", token::token_type::LCURLY};
        expected.push_back(tok);

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";
        for (unsigned int i = 0; i < tokens.size(); i++) {
            if (*tokens[i] != expected[i]) return "Token " + std::to_string(i) + " was not correct";
        }

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method correctly parses integer literals.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_integers() {
        const lexer::token_list_t tokens = lexer::lex_all("1 2 345 -6 -7");
        std::vector<token::int_token> expected;
        std::stringstream seen_so_far;
        for (const std::string number : {"1", "2", "345", "-6", "-7"}) {
            expected.push_back(token::int_token {
                    token::token {strlen(seen_so_far.str()), number, token::token_type::INTVAL}, std::stol(number)});
            seen_so_far << number << " ";
        }

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";
        for (unsigned int index = 0; index < (unsigned int)tokens.size(); index++) {
            if (*tokens[index] != expected[index]) return "Lexer did not correctly identify an integer literal.";
            if (std::static_pointer_cast<token::int_token>(tokens[index])->value != expected[index].value)
                return "Lexer did not correctly get the integer value of '" + std::to_string(expected[index].value)
                     + "'";
        }

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method correctly parses integer literals.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_floats() {
        const lexer::token_list_t tokens = lexer::lex_all("0.1 2. .3 -4.5 -6. -.7");
        std::vector<token::float_token> expected;
        std::stringstream seen_so_far;
        for (const std::string number : {"0.1", "2.", ".3", "-4.5", "-6.", "-.7"}) {
            expected.push_back(token::float_token {
                    token::token {strlen(seen_so_far.str()), number, token::token_type::FLOATVAL}, std::stod(number)});
            seen_so_far << number << " ";
        }

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";
        for (unsigned int index = 0; index < (unsigned int)tokens.size(); index++) {
            if (*tokens[index] != expected[index]) return "Lexer did not correctly identify a floating-point literal.";
            if (std::static_pointer_cast<token::float_token>(tokens[index])->value != expected[index].value)
                return "Lexer did not correctly get the floating-point value of '"
                     + std::to_string(expected[index].value) + "'";
        }

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method correctly parses string literals.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_strings() {
        const lexer::token_list_t tokens = lexer::lex_all("read image \"sample.png\" to");

        if (tokens.size() != 4) {  //  NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            return "Lexer did not return the correct number of tokens";
        }

        if (*tokens[0] != token::token {0, "read", token::token_type::READ})
            return "Lexer did not correctly lex the token 'read.'";
        if (*tokens[1] != token::token {strlen("read "), "image", token::token_type::IMAGE})
            return "Lexer did not correctly lex the token 'image.'";
        if ((*tokens[2] != token::token {strlen("read image "), "\"sample.png\"", token::token_type::STRING})
            || (std::static_pointer_cast<token::string_token>(tokens[2])->value != "sample.png"))
            return "Lexer did not correctly lex the token 'sample.png'";
        if (*tokens[3] != token::token {strlen("read image \"sample.png\" "), "to", token::token_type::TO})
            return "Lexer did not correctly lex the token 'to.'";

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method correctly parses string literals.
     * @details Tests with multiple strings on one line.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_strings_multi() {
        const lexer::token_list_t tokens = lexer::lex_all(R"("string 1" "string 2")");

        if (tokens.size() != 2) return "Lexer did not return the correct number of tokens";

        if ((*tokens[0] != token::token {0, "\"string 1\"", token::token_type::STRING})
            || (std::static_pointer_cast<token::string_token>(tokens[0])->value != "string 1"))
            return "Lexer did not correctly lex the token 'string 1'";
        if ((*tokens[1] != token::token {strlen("\"string 1\" "), "\"string 2\"", token::token_type::STRING})
            || (std::static_pointer_cast<token::string_token>(tokens[1])->value != "string 2"))
            return "Lexer did not correctly lex the token 'string 1'";

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method correctly parses string literals.
     * @details Tests with multiple lines.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_strings_multi_line() {
        const lexer::token_list_t tokens = lexer::lex_all("\"string 1\"\n\"string 2\"");

        if (tokens.size() != 3) return "Lexer did not return the correct number of tokens";

        if ((*tokens[0] != token::token {0, "\"string 1\"", token::token_type::STRING})
            || (std::static_pointer_cast<token::string_token>(tokens[0])->value != "string 1"))
            return "Lexer did not correctly lex the token 'string 1'";
        if (*tokens[1] != token::token {strlen("\"string 1\""), "\n", token::token_type::NEWLINE})
            return "Lexer did not correctly lex the newline";
        if ((*tokens[2] != token::token {strlen("\"string 1\"\n"), "\"string 2\"", token::token_type::STRING})
            || (std::static_pointer_cast<token::string_token>(tokens[2])->value != "string 2"))
            return "Lexer did not correctly lex the token 'string 1'";

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method correctly identifies variables that start with keywords.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_variables_with_keywords() {
        const lexer::token_list_t tokens = lexer::lex_all("float float_variable");
        std::vector<token::token> expected;
        token::token tok {0, "float", token::token_type::FLOAT};
        expected.emplace_back(tok);
        tok = {strlen("float "), "float_variable", token::token_type::VARIABLE};
        expected.emplace_back(tok);

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";
        if (*tokens[0] != expected[0]) return "Keyword token was not correct";
        if (*tokens[1] != expected[1]) return "Variable token was not correct";

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method correctly ignores single-line comments.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_single_line_comments() {
        const lexer::token_list_t tokens = lexer::lex_all("float  //  comment \n int");
        const std::vector<token::token> expected {
                token::token {0, "float", token::token_type::FLOAT},
                token::token {strlen("float  //  comment "), "\n", token::token_type::NEWLINE},
                token::token {strlen("float  //  comment \n "), "int", token::token_type::INT}};

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";

        for (unsigned int i = 0; i < (unsigned int)tokens.size(); i++) {
            if (*tokens[i] != expected[i]) return "Token " + std::to_string(i) + " was not correct";
        }

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method correctly ignores a newline at the end of a single-line comment
     *     when the single-line comment ends with a backslash.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_single_line_comments_escaped_line() {
        const lexer::token_list_t tokens = lexer::lex_all("float  //  comment \\\n int");
        const std::vector<token::token> expected {
                token::token {0, "float", token::token_type::FLOAT},
                token::token {strlen("float  //  comment \\\n "), "int", token::token_type::INT}};

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";

        for (unsigned int i = 0; i < (unsigned int)tokens.size(); i++) {
            if (*tokens[i] != expected[i]) return "Token " + std::to_string(i) + " was not correct";
        }

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method correctly ignores a backtick in the middle of a single-line
     *     comment.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_single_line_comments_backslash() {
        const lexer::token_list_t tokens = lexer::lex_all("float  //  comment \\ comment \n int");
        const std::vector<token::token> expected {
                token::token {0, "float", token::token_type::FLOAT},
                token::token {strlen("float  //  comment \\ comment "), "\n", token::token_type::NEWLINE},
                token::token {strlen("float  //  comment \\ comment \n "), "int", token::token_type::INT}};

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";

        for (unsigned int i = 0; i < (unsigned int)tokens.size(); i++) {
            if (*tokens[i] != expected[i]) return "Token " + std::to_string(i) + " was not correct";
        }

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method correctly ignores multi-line comments.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_multi_line_comments() {
        const lexer::token_list_t tokens = lexer::lex_all("float /* \n */ int");
        const std::vector<token::token> expected {
                token::token {0, "float", token::token_type::FLOAT},
                token::token {strlen("float /* \n */ "), "int", token::token_type::INT}};

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";

        for (unsigned int i = 0; i < (unsigned int)tokens.size(); i++) {
            if (*tokens[i] != expected[i]) return "Token " + std::to_string(i) + " was not correct";
        }

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method correctly doesn't ignore fake "comments" inside strings.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_string_comments() {
        const lexer::token_list_t tokens = lexer::lex_all(R"(" //  not a comment " " /* not a comment */ ")");
        const std::vector<token::string_token> expected {
                token::string_token {{0, R"(" //  not a comment ")", token::token_type::STRING}, " //  not a comment "},
                token::string_token {
                        {strlen(R"(" //  not a comment " )"), R"(" /* not a comment */ ")", token::token_type::STRING},
                        " /* not a comment */ "}};

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";

        for (unsigned int index = 0; index < (unsigned int)tokens.size(); index++) {
            if (*tokens[index] != expected[index]) return "Token " + std::to_string(index) + " was not correct";
            if (std::static_pointer_cast<token::string_token>(tokens[index])->value != expected[index].value)
                return "Lexer did not correctly lex the string " + expected[index].text;
        }

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method correctly ignores escaped newlines.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_escaped_newline() {
        const lexer::token_list_t tokens = lexer::lex_all("int \\\n float");
        const std::vector<token::token> expected {
                token::token {0, "int", token::token_type::INT},
                token::token {strlen("int \\\n "), "float", token::token_type::FLOAT}};

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";

        for (unsigned int i = 0; i < (unsigned int)tokens.size(); i++) {
            if (*tokens[i] != expected[i]) return "Token " + std::to_string(i) + " was not correct";
        }

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method correctly throws an exception when it encounters an illegal
     *     character.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_exception() {
        try {
            lexer::lex_all("bool \r bool");
        } catch (const lexer::lexing_exception& e) { return ""; }

        return "Lexer did not throw an exception upon an illegal character";
    }

    /**
     * @brief Comprehensive test for the lexer.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string comprehensive1() {
        std::stringstream input;
        input << "/**" << std::endl
              << " * @brief Adds two pixel values together." << std::endl
              << " *" << std::endl
              << " * @param left The first pixel to add." << std::endl
              << " * @param right The second pixel to add." << std::endl
              << " * @return The sum of the two pixels." << std::endl
              << " */" << std::endl
              << "fn add(pixel_one : color, pixel_two : color) : color {" << std::endl
              << "    return {                          \\" << std::endl
              << "        pixel_one{0} + pixel_two{0},  \\" << std::endl
              << "                pixel_one{1} + pixel_two{1},  \\" << std::endl
              << "                pixel_one{2} + pixel_two{2},  \\" << std::endl
              << "                pixel_one{3} + pixel_two{3}   \\" << std::endl
              << "    }" << std::endl
              << "}" << std::endl;

        const lexer::token_list_t tokens = lexer::lex_all(input.str());
        //  This lambda defines a shorthand for making a token from just a string and type.
        auto tok = [](const std::string& text, token::token_type type) {
            return std::make_shared<token::token>(token::token {0, text, type});
        };
        //  This lambda is the same as above, for an integer token.
        auto int_tok = [](unsigned int value) {
            return std::make_shared<token::int_token>(
                    token::int_token {{0, std::to_string(value), token::token_type::INTVAL}, value});
        };
        const std::vector<lexer::token_ptr_t> expected {
                tok("\n", token::NEWLINE),
                tok("fn", token::FN),
                tok("add", token::VARIABLE),
                tok("(", token::LPAREN),
                tok("pixel_one", token::VARIABLE),
                tok(":", token::COLON),
                tok("color", token::VARIABLE),
                tok(",", token::COMMA),
                tok("pixel_two", token::VARIABLE),
                tok(":", token::COLON),
                tok("color", token::VARIABLE),
                tok(")", token::RPAREN),
                tok(":", token::COLON),
                tok("color", token::VARIABLE),
                tok("{", token::LCURLY),
                tok("\n", token::NEWLINE),
                tok("return", token::RETURN),
                tok("{", token::LCURLY),
                tok("pixel_one", token::VARIABLE),
                tok("{", token::LCURLY),
                int_tok(0),
                tok("}", token::RCURLY),
                tok("+", token::OP),
                tok("pixel_two", token::VARIABLE),
                tok("{", token::LCURLY),
                int_tok(0),
                tok("}", token::RCURLY),
                tok(",", token::COMMA),
                tok("pixel_one", token::VARIABLE),
                tok("{", token::LCURLY),
                int_tok(1),
                tok("}", token::RCURLY),
                tok("+", token::OP),
                tok("pixel_two", token::VARIABLE),
                tok("{", token::LCURLY),
                int_tok(1),
                tok("}", token::RCURLY),
                tok(",", token::COMMA),
                tok("pixel_one", token::VARIABLE),
                tok("{", token::LCURLY),
                int_tok(2),
                tok("}", token::RCURLY),
                tok("+", token::OP),
                tok("pixel_two", token::VARIABLE),
                tok("{", token::LCURLY),
                int_tok(2),
                tok("}", token::RCURLY),
                tok(",", token::COMMA),
                tok("pixel_one", token::VARIABLE),
                tok("{", token::LCURLY),
                int_tok(3),
                tok("}", token::RCURLY),
                tok("+", token::OP),
                tok("pixel_two", token::VARIABLE),
                tok("{", token::LCURLY),
                int_tok(3),
                tok("}", token::RCURLY),
                tok("}", token::RCURLY),
                tok("\n", token::NEWLINE),
                tok("}", token::RCURLY),
                tok("\n", token::NEWLINE),
        };

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";

        for (unsigned int index = 0; index < (unsigned int)tokens.size(); index++) {
            if (tokens[index]->text != expected[index]->text)
                return "Token " + std::to_string(index) + " had the wrong text: '" + tokens[index]->text
                     + "' instead of '" + expected[index]->text + "'";

            if (tokens[index]->type != expected[index]->type)
                return "Token " + std::to_string(index)
                     + " had the wrong type: " + token::token_type_to_string(tokens[index]->type) + " instead of "
                     + token::token_type_to_string(expected[index]->type);

            if (tokens[index]->type == token::token_type::INTVAL) {
                const unsigned int actual_value = std::static_pointer_cast<token::int_token>(tokens[index])->value;
                const unsigned int expected_value = std::static_pointer_cast<token::int_token>(expected[index])->value;
                if (actual_value != expected_value)
                    return "Token " + std::to_string(index) + " had the wrong value: " + std::to_string(actual_value)
                         + " instead of " + std::to_string(expected_value);
            }
        }

        return "";
    }

    /**
     * @brief Comprehensive test for the lexer.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string comprehensive2() {
        std::stringstream input;
        input << "/**" << std::endl
              << " * @brief Blurs the given image." << std::endl
              << " *" << std::endl
              << " * @param img The image to blur." << std::endl
              << " * @return The blurred image." << std::endl
              << " */" << std::endl
              << "fn blur(img[H, W] : pict) : pict {" << std::endl
              << "    return array[y : H, x : W] blur_one(img, x, y)" << std::endl
              << "}" << std::endl
              << std::endl
              << "read image \"sample.png\" to img" << std::endl
              << "write image blur(img) to \"sample-blurry.png\"" << std::endl;

        const lexer::token_list_t tokens = lexer::lex_all(input.str());
        //  This lambda defines a shorthand for making a token from just a string and type.
        auto tok = [](const std::string& text, token::token_type type) {
            return std::make_shared<token::token>(token::token {0, text, type});
        };
        //  This lambda is the same as above, for a string token.
        auto str_tok = [](const std::string& value) {
            return std::make_shared<token::string_token>(
                    token::string_token {{0, "\"" + value + "\"", token::token_type::STRING}, value});
        };
        const std::vector<lexer::token_ptr_t> expected {tok("\n", token::NEWLINE),
                                                        tok("fn", token::FN),
                                                        tok("blur", token::VARIABLE),
                                                        tok("(", token::LPAREN),
                                                        tok("img", token::VARIABLE),
                                                        tok("[", token::LSQUARE),
                                                        tok("H", token::VARIABLE),
                                                        tok(",", token::COMMA),
                                                        tok("W", token::VARIABLE),
                                                        tok("]", token::RSQUARE),
                                                        tok(":", token::COLON),
                                                        tok("pict", token::VARIABLE),
                                                        tok(")", token::RPAREN),
                                                        tok(":", token::COLON),
                                                        tok("pict", token::VARIABLE),
                                                        tok("{", token::LCURLY),
                                                        tok("\n", token::NEWLINE),
                                                        tok("return", token::RETURN),
                                                        tok("array", token::ARRAY),
                                                        tok("[", token::LSQUARE),
                                                        tok("y", token::VARIABLE),
                                                        tok(":", token::COLON),
                                                        tok("H", token::VARIABLE),
                                                        tok(",", token::COMMA),
                                                        tok("x", token::VARIABLE),
                                                        tok(":", token::COLON),
                                                        tok("W", token::VARIABLE),
                                                        tok("]", token::RSQUARE),
                                                        tok("blur_one", token::VARIABLE),
                                                        tok("(", token::LPAREN),
                                                        tok("img", token::VARIABLE),
                                                        tok(",", token::COMMA),
                                                        tok("x", token::VARIABLE),
                                                        tok(",", token::COMMA),
                                                        tok("y", token::VARIABLE),
                                                        tok(")", token::RPAREN),
                                                        tok("\n", token::NEWLINE),
                                                        tok("}", token::RCURLY),
                                                        tok("\n", token::NEWLINE),
                                                        tok("\n", token::NEWLINE),
                                                        tok("read", token::READ),
                                                        tok("image", token::IMAGE),
                                                        str_tok("sample.png"),
                                                        tok("to", token::TO),
                                                        tok("img", token::VARIABLE),
                                                        tok("\n", token::NEWLINE),
                                                        tok("write", token::WRITE),
                                                        tok("image", token::IMAGE),
                                                        tok("blur", token::VARIABLE),
                                                        tok("(", token::LPAREN),
                                                        tok("img", token::VARIABLE),
                                                        tok(")", token::RPAREN),
                                                        tok("to", token::TO),
                                                        str_tok("sample-blurry.png"),
                                                        tok("\n", token::NEWLINE)};

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";

        for (unsigned int index = 0; index < (unsigned int)tokens.size(); index++) {
            if (tokens[index]->text != expected[index]->text)
                return "Token " + std::to_string(index) + " had the wrong text: '" + tokens[index]->text
                     + "' instead of '" + expected[index]->text + "'";

            if (tokens[index]->type != expected[index]->type)
                return "Token " + std::to_string(index)
                     + " had the wrong type: " + token::token_type_to_string(tokens[index]->type) + " instead of "
                     + token::token_type_to_string(expected[index]->type);

            if (tokens[index]->type == token::token_type::INTVAL) {
                const unsigned int actual_value = std::static_pointer_cast<token::int_token>(tokens[index])->value;
                const unsigned int expected_value = std::static_pointer_cast<token::int_token>(expected[index])->value;
                if (actual_value != expected_value)
                    return "Token " + std::to_string(index) + " had the wrong value: " + std::to_string(actual_value)
                         + " instead of " + std::to_string(expected_value);
            }
        }

        return "";
    }

    /**
     * @brief Assembles the set of all lexer unit tests.
     *
     * @return The set of all lexer unit tests.
     */
    std::vector<test_t> get_all_tests() {
        std::vector<test_t> tests;
        tests.emplace_back(constructor_no_fail, "Lexer constructor: no-fail");
        tests.emplace_back(lexer_no_match, "Lexer no-match");
        tests.emplace_back(lexer_match, "Lexer match");
        tests.emplace_back(assemble_lexers_no_fail, "Lexer `assemble_lexers`: no-fail");
        tests.emplace_back(lex_all_keywords, "Lexer `lex_all`: keywords");
        tests.emplace_back(lex_all_punctuation, "Lexer `lex_all`: punctuation");
        tests.emplace_back(lex_all_integers, "Lexer `lex_all`: integers");
        tests.emplace_back(lex_all_floats, "Lexer `lex_all`: floats");
        tests.emplace_back(lex_all_strings, "Lexer `lex_all`: strings");
        tests.emplace_back(lex_all_strings_multi, "Lexer `lex_all`: multiple strings");
        tests.emplace_back(lex_all_strings_multi_line, "Lexer `lex_all`: multi-line strings");
        tests.emplace_back(lex_all_variables_with_keywords, "Lexer `lex_all`: variables with keywords");
        tests.emplace_back(lex_all_single_line_comments, "Lexer `lex_all`: single-line comments");
        tests.emplace_back(lex_all_single_line_comments_escaped_line,
                           "Lexer `lex_all`: single-line comments with escaped newline");
        tests.emplace_back(lex_all_single_line_comments_backslash,
                           "Lexer `lex_all`: single-line comments with an interior backslash");
        tests.emplace_back(lex_all_multi_line_comments, "Lexer `lex_all`: multi-line comments");
        tests.emplace_back(lex_all_string_comments, "Lexer `lex_all`: comments inside strings");
        tests.emplace_back(lex_all_escaped_newline, "Lexer `lex_all`: escaped newline");
        tests.emplace_back(lex_all_exception, "Lexer `lex_all`: throwing an exception");
        tests.emplace_back(comprehensive1, "Lexer: comprehensive 1");
        tests.emplace_back(comprehensive2, "Lexer: comprehensive 2");
        return tests;
    }
}  //  namespace tests::lexer_tests

/**
 * @brief `lexer` test program entry point.
 *
 */
int main() {
    std::vector<tests::test_t> tests = tests::lexer_tests::get_all_tests();
    tests::run_tests(tests);

    return 0;
}
