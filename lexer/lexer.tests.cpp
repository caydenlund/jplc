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
     * @brief Returns a smart pointer to a new token.
     *
     * @param start The start index of the token.
     * @param text The text of the token.
     * @param type The type of the token.
     * @return A smart pointer to the new token.
     */
    inline std::shared_ptr<token::token> tok(unsigned int start, const std::string& text, token::token_type type) {
        return std::make_shared<token::token>(token::token {start, text, type});
    }

    /**
     * @brief Returns a smart pointer to a new floating-point token.
     *
     * @param start The start index of the token.
     * @param text The text of the token.
     * @param value The value of the token.
     * @return A smart pointer to the new token.
     */
    inline std::shared_ptr<token::float_token> float_tok(unsigned int start, const std::string& text, double value) {
        return std::make_shared<token::float_token>(
                token::float_token {{start, text, token::token_type::FLOATVAL}, value});
    }

    /**
     * @brief Returns a smart pointer to a new integer token.
     *
     * @param start The start index of the token.
     * @param value The value of the token.
     * @return A smart pointer to the new token.
     */
    std::shared_ptr<token::int_token> int_tok(unsigned int start, long value) {
        return std::make_shared<token::int_token>(
                token::int_token {{start, std::to_string(value), token::token_type::INTVAL}, value});
    }

    /**
     * @brief Returns a smart pointer to a new string token.
     *
     * @param start The start index of the token.
     * @param value The value of the token.
     * @return A smart pointer to the new token.
     */
    std::shared_ptr<token::string_token> str_tok(unsigned int start, const std::string& value) {
        return std::make_shared<token::string_token>(
                token::string_token {{start, "\"" + value + "\"", token::token_type::STRING}, value});
    }

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

        const std::vector<token::token> expected {
                {0, "array", token::token_type::ARRAY},
                {strlen("array "), "bool", token::token_type::BOOL},
                {strlen("array bool "), "assert", token::token_type::ASSERT},
                {strlen("array bool assert "), "else", token::token_type::ELSE},
                {strlen("array bool assert else"), "", token::token_type::END_OF_FILE}};

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

        const std::vector<token::token> expected {
                token::token {0, ":", token::token_type::COLON},
                token::token {strlen(": "), ",", token::token_type::COMMA},
                token::token {strlen(": , "), "(", token::token_type::LPAREN},
                token::token {strlen(": , ( "), "{", token::token_type::LCURLY},
                token::token {strlen(": , ( {"), "{", token::token_type::LCURLY},
                token::token {strlen(": , ( {{"), "", token::token_type::END_OF_FILE}};

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

        const lexer::token_list_t expected {int_tok(0, 1),
                                            int_tok(strlen("1 "), 2),
                                            int_tok(strlen("1 2 "), 345),
                                            tok(strlen("1 2 345 "), "-", token::token_type::OP),
                                            int_tok(strlen("1 2 345 -"), 6),
                                            tok(strlen("1 2 345 -6 "), "-", token::token_type::OP),
                                            int_tok(strlen("1 2 345 -6 -"), 7),
                                            tok(strlen("1 2 345 -6 -7"), "", token::token_type::END_OF_FILE)};

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";

        for (unsigned int index = 0; index < (unsigned int)tokens.size(); index++) {
            if (*tokens[index] != *expected[index]) return "Lexer did not correctly identify an integer literal";

            if (expected[index]->type == token::token_type::INTVAL) {
                const long actual_value = std::static_pointer_cast<token::int_token>(tokens[index])->value;
                const long expected_value = std::static_pointer_cast<token::int_token>(expected[index])->value;
                if (actual_value != expected_value)
                    return "Lexer did not correctly identify the integer value of " + std::to_string(expected_value);
            }
        }

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method correctly parses floating-point literals.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_floats() {
        const lexer::token_list_t tokens = lexer::lex_all("0.1 2. .3 -4.5 -6. -.7");

        const lexer::token_list_t expected {float_tok(0, "0.1", 0.1),
                                            float_tok(strlen("0.1 "), "2.", 2.0),
                                            float_tok(strlen("0.1 2. "), ".3", 0.3),
                                            tok(strlen("0.1 2. .3 "), "-", token::token_type::OP),
                                            float_tok(strlen("0.1 2. .3 -"), "4.5", 4.5),
                                            tok(strlen("0.1 2. .3 -4.5 "), "-", token::token_type::OP),
                                            float_tok(strlen("0.1 2. .3 -4.5 -"), "6.", 6.0),
                                            tok(strlen("0.1 2. .3 -4.5 -6. "), "-", token::token_type::OP),
                                            float_tok(strlen("0.1 2. .3 -4.5 -6. -"), ".7", 0.7),
                                            tok(strlen("0.1 2. .3 -4.5 -6. -.7"), "", token::token_type::END_OF_FILE)};

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";

        for (unsigned int index = 0; index < (unsigned int)tokens.size(); index++) {
            if (*tokens[index] != *expected[index]) return "Lexer did not correctly identify a floating-point literal.";

            if (expected[index]->type == token::token_type::FLOATVAL) {
                const double actual_value = std::static_pointer_cast<token::float_token>(tokens[index])->value;
                const double expected_value = std::static_pointer_cast<token::float_token>(expected[index])->value;
                if (actual_value != expected_value)
                    return "Lexer did not correctly identify the double value of '" + std::to_string(expected_value)
                         + "'";
            }
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

        const lexer::token_list_t expected {
                tok(0, "read", token::token_type::READ), tok(strlen("read "), "image", token::token_type::IMAGE),
                str_tok(strlen("read image "), "sample.png"),
                tok(strlen("read image \"sample.png\" "), "to", token::token_type::TO),
                tok(strlen("read image \"sample.png\" to"), "", token::token_type::END_OF_FILE)};

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";

        for (unsigned int i = 0; i < tokens.size(); i++) {
            if (*tokens[i] != *expected[i]) return "Token " + std::to_string(i) + " was not correct";

            if (expected[i]->type == token::token_type::STRING) {
                const std::string actual_value = std::static_pointer_cast<token::string_token>(tokens[i])->value;
                const std::string expected_value = std::static_pointer_cast<token::string_token>(expected[i])->value;
                if (actual_value != expected_value)
                    return "Lexer did not correctly identify the string value of '" + expected_value + "'";
            }
        }

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

        const lexer::token_list_t expected {
                str_tok(0, "string 1"), str_tok(strlen("\"string 1\" "), "string 2"),
                tok(strlen(R"("string 1" "string 2")"), "", token::token_type::END_OF_FILE)};

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";

        for (unsigned int i = 0; i < tokens.size(); i++) {
            if (*tokens[i] != *expected[i]) return "Token " + std::to_string(i) + " was not correct";
            if (expected[i]->type == token::token_type::STRING) {
                const std::string actual_value = std::static_pointer_cast<token::string_token>(tokens[i])->value;
                const std::string expected_value = std::static_pointer_cast<token::string_token>(expected[i])->value;
                if (actual_value != expected_value)
                    return "Lexer did not correctly identify the string value of '" + expected_value + "'";
            }
        }

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

        const lexer::token_list_t expected {
                str_tok(0, "string 1"), tok(strlen("\"string 1\""), "\n", token::token_type::NEWLINE),
                str_tok(strlen("\"string 1\"\n"), "string 2"),
                tok(strlen("\"string 1\"\n\"string 2\""), "", token::token_type::END_OF_FILE)};

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";

        for (unsigned int i = 0; i < tokens.size(); i++) {
            if (*tokens[i] != *expected[i]) return "Token " + std::to_string(i) + " was not correct";

            if (expected[i]->type == token::token_type::STRING) {
                const std::string actual_value = std::static_pointer_cast<token::string_token>(tokens[i])->value;
                const std::string expected_value = std::static_pointer_cast<token::string_token>(expected[i])->value;
                if (actual_value != expected_value)
                    return "Lexer did not correctly identify the string value of '" + expected_value + "'";
            }
        }

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method correctly identifies variables that start with keywords.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_variables_with_keywords() {
        const lexer::token_list_t tokens = lexer::lex_all("float float_variable");

        const lexer::token_list_t expected {tok(0, "float", token::token_type::FLOAT),
                                            tok(strlen("float "), "float_variable", token::token_type::VARIABLE),
                                            tok(strlen("float float_variable"), "", token::token_type::END_OF_FILE)};

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";

        for (unsigned int i = 0; i < tokens.size(); i++) {
            if (*tokens[i] != *expected[i]) return "Token " + std::to_string(i) + " was not correct";
        }

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method correctly ignores single-line comments.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_single_line_comments() {
        const lexer::token_list_t tokens = lexer::lex_all("float  //  comment \n int");
        const lexer::token_list_t expected {
                tok(0, "float", token::token_type::FLOAT),
                tok(strlen("float  //  comment "), "\n", token::token_type::NEWLINE),
                tok(strlen("float  //  comment \n "), "int", token::token_type::INT),
                tok(strlen("float  //  comment \n int"), "", token::token_type::END_OF_FILE)};

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";

        for (unsigned int i = 0; i < (unsigned int)tokens.size(); i++) {
            if (*tokens[i] != *expected[i]) return "Token " + std::to_string(i) + " was not correct";
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
        const lexer::token_list_t expected {
                tok(0, "float", token::token_type::FLOAT),
                tok(strlen("float  //  comment \\\n "), "int", token::token_type::INT),
                tok(strlen("float  //  comment \\\n int"), "", token::token_type::END_OF_FILE)};

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";

        for (unsigned int i = 0; i < (unsigned int)tokens.size(); i++) {
            if (*tokens[i] != *expected[i]) return "Token " + std::to_string(i) + " was not correct";
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
        const lexer::token_list_t expected {
                tok(0, "float", token::token_type::FLOAT),
                tok(strlen("float  //  comment \\ comment "), "\n", token::token_type::NEWLINE),
                tok(strlen("float  //  comment \\ comment \n "), "int", token::token_type::INT),
                tok(strlen("float  //  comment \\ comment \n int"), "", token::token_type::END_OF_FILE)};

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";

        for (unsigned int i = 0; i < (unsigned int)tokens.size(); i++) {
            if (*tokens[i] != *expected[i]) return "Token " + std::to_string(i) + " was not correct";
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
        const lexer::token_list_t expected {tok(0, "float", token::token_type::FLOAT),
                                            tok(strlen("float /* \n */ "), "int", token::token_type::INT),
                                            tok(strlen("float /* \n */ int"), "", token::token_type::END_OF_FILE)};

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";

        for (unsigned int i = 0; i < (unsigned int)tokens.size(); i++) {
            if (*tokens[i] != *expected[i]) return "Token " + std::to_string(i) + " was not correct";
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
        lexer::token_list_t expected {
                str_tok(0, " //  not a comment "),
                str_tok(strlen(R"(" //  not a comment " )"), " /* not a comment */ "),
                tok(strlen(R"(" //  not a comment " " /* not a comment */ ")"), "", token::token_type::END_OF_FILE)};

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";

        for (unsigned int index = 0; index < (unsigned int)tokens.size(); index++) {
            if (*tokens[index] != *expected[index]) return "Token " + std::to_string(index) + " was not correct";

            if (expected[index]->type == token::token_type::STRING) {
                const std::string actual_value = std::static_pointer_cast<token::string_token>(tokens[index])->value;
                const std::string expected_value
                        = std::static_pointer_cast<token::string_token>(expected[index])->value;
                if (actual_value != expected_value)
                    return "Lexer did not correctly identify the string value of '" + expected_value + "'";
            }
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
                token::token {strlen("int \\\n "), "float", token::token_type::FLOAT},
                token::token {strlen("int \\\n float"), "", token::token_type::END_OF_FILE}};

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

        const std::vector<lexer::token_ptr_t> expected {tok(0, "\n", token::token_type::NEWLINE),
                                                        tok(0, "fn", token::token_type::FN),
                                                        tok(0, "add", token::token_type::VARIABLE),
                                                        tok(0, "(", token::token_type::LPAREN),
                                                        tok(0, "pixel_one", token::token_type::VARIABLE),
                                                        tok(0, ":", token::token_type::COLON),
                                                        tok(0, "color", token::token_type::VARIABLE),
                                                        tok(0, ",", token::token_type::COMMA),
                                                        tok(0, "pixel_two", token::token_type::VARIABLE),
                                                        tok(0, ":", token::token_type::COLON),
                                                        tok(0, "color", token::token_type::VARIABLE),
                                                        tok(0, ")", token::token_type::RPAREN),
                                                        tok(0, ":", token::token_type::COLON),
                                                        tok(0, "color", token::token_type::VARIABLE),
                                                        tok(0, "{", token::token_type::LCURLY),
                                                        tok(0, "\n", token::token_type::NEWLINE),
                                                        tok(0, "return", token::token_type::RETURN),
                                                        tok(0, "{", token::token_type::LCURLY),
                                                        tok(0, "pixel_one", token::token_type::VARIABLE),
                                                        tok(0, "{", token::token_type::LCURLY),
                                                        int_tok(0, 0),
                                                        tok(0, "}", token::token_type::RCURLY),
                                                        tok(0, "+", token::token_type::OP),
                                                        tok(0, "pixel_two", token::token_type::VARIABLE),
                                                        tok(0, "{", token::token_type::LCURLY),
                                                        int_tok(0, 0),
                                                        tok(0, "}", token::token_type::RCURLY),
                                                        tok(0, ",", token::token_type::COMMA),
                                                        tok(0, "pixel_one", token::token_type::VARIABLE),
                                                        tok(0, "{", token::token_type::LCURLY),
                                                        int_tok(0, 1),
                                                        tok(0, "}", token::token_type::RCURLY),
                                                        tok(0, "+", token::token_type::OP),
                                                        tok(0, "pixel_two", token::token_type::VARIABLE),
                                                        tok(0, "{", token::token_type::LCURLY),
                                                        int_tok(0, 1),
                                                        tok(0, "}", token::token_type::RCURLY),
                                                        tok(0, ",", token::token_type::COMMA),
                                                        tok(0, "pixel_one", token::token_type::VARIABLE),
                                                        tok(0, "{", token::token_type::LCURLY),
                                                        int_tok(0, 2),
                                                        tok(0, "}", token::token_type::RCURLY),
                                                        tok(0, "+", token::token_type::OP),
                                                        tok(0, "pixel_two", token::token_type::VARIABLE),
                                                        tok(0, "{", token::token_type::LCURLY),
                                                        int_tok(0, 2),
                                                        tok(0, "}", token::token_type::RCURLY),
                                                        tok(0, ",", token::token_type::COMMA),
                                                        tok(0, "pixel_one", token::token_type::VARIABLE),
                                                        tok(0, "{", token::token_type::LCURLY),
                                                        int_tok(0, 3),
                                                        tok(0, "}", token::token_type::RCURLY),
                                                        tok(0, "+", token::token_type::OP),
                                                        tok(0, "pixel_two", token::token_type::VARIABLE),
                                                        tok(0, "{", token::token_type::LCURLY),
                                                        int_tok(0, 3),
                                                        tok(0, "}", token::token_type::RCURLY),
                                                        tok(0, "}", token::token_type::RCURLY),
                                                        tok(0, "\n", token::token_type::NEWLINE),
                                                        tok(0, "}", token::token_type::RCURLY),
                                                        tok(0, "\n", token::token_type::NEWLINE),
                                                        tok(0, "", token::token_type::END_OF_FILE)};

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

        const std::vector<lexer::token_ptr_t> expected {tok(0, "\n", token::token_type::NEWLINE),
                                                        tok(0, "fn", token::token_type::FN),
                                                        tok(0, "blur", token::token_type::VARIABLE),
                                                        tok(0, "(", token::token_type::LPAREN),
                                                        tok(0, "img", token::token_type::VARIABLE),
                                                        tok(0, "[", token::token_type::LSQUARE),
                                                        tok(0, "H", token::token_type::VARIABLE),
                                                        tok(0, ",", token::token_type::COMMA),
                                                        tok(0, "W", token::token_type::VARIABLE),
                                                        tok(0, "]", token::token_type::RSQUARE),
                                                        tok(0, ":", token::token_type::COLON),
                                                        tok(0, "pict", token::token_type::VARIABLE),
                                                        tok(0, ")", token::token_type::RPAREN),
                                                        tok(0, ":", token::token_type::COLON),
                                                        tok(0, "pict", token::token_type::VARIABLE),
                                                        tok(0, "{", token::token_type::LCURLY),
                                                        tok(0, "\n", token::token_type::NEWLINE),
                                                        tok(0, "return", token::token_type::RETURN),
                                                        tok(0, "array", token::token_type::ARRAY),
                                                        tok(0, "[", token::token_type::LSQUARE),
                                                        tok(0, "y", token::token_type::VARIABLE),
                                                        tok(0, ":", token::token_type::COLON),
                                                        tok(0, "H", token::token_type::VARIABLE),
                                                        tok(0, ",", token::token_type::COMMA),
                                                        tok(0, "x", token::token_type::VARIABLE),
                                                        tok(0, ":", token::token_type::COLON),
                                                        tok(0, "W", token::token_type::VARIABLE),
                                                        tok(0, "]", token::token_type::RSQUARE),
                                                        tok(0, "blur_one", token::token_type::VARIABLE),
                                                        tok(0, "(", token::token_type::LPAREN),
                                                        tok(0, "img", token::token_type::VARIABLE),
                                                        tok(0, ",", token::token_type::COMMA),
                                                        tok(0, "x", token::token_type::VARIABLE),
                                                        tok(0, ",", token::token_type::COMMA),
                                                        tok(0, "y", token::token_type::VARIABLE),
                                                        tok(0, ")", token::token_type::RPAREN),
                                                        tok(0, "\n", token::token_type::NEWLINE),
                                                        tok(0, "}", token::token_type::RCURLY),
                                                        tok(0, "\n", token::token_type::NEWLINE),
                                                        tok(0, "read", token::token_type::READ),
                                                        tok(0, "image", token::token_type::IMAGE),
                                                        str_tok(0, "sample.png"),
                                                        tok(0, "to", token::token_type::TO),
                                                        tok(0, "img", token::token_type::VARIABLE),
                                                        tok(0, "\n", token::token_type::NEWLINE),
                                                        tok(0, "write", token::token_type::WRITE),
                                                        tok(0, "image", token::token_type::IMAGE),
                                                        tok(0, "blur", token::token_type::VARIABLE),
                                                        tok(0, "(", token::token_type::LPAREN),
                                                        tok(0, "img", token::token_type::VARIABLE),
                                                        tok(0, ")", token::token_type::RPAREN),
                                                        tok(0, "to", token::token_type::TO),
                                                        str_tok(0, "sample-blurry.png"),
                                                        tok(0, "\n", token::token_type::NEWLINE),
                                                        tok(0, "", token::token_type::END_OF_FILE)};

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
