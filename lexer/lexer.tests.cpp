/**
 * @file lexer.tests.cpp
 * @package Assignment 2.
 * @author Cayden Lund (u1182408)
 * @brief Contains tests for the lexer.
 *
 */

//  TODO: Test for negative numbers.

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
        const lexer::token_list_t tokens = lexer::lex_all("1 2 345");

        if (tokens.size() != 3) return "Lexer did not return the correct number of tokens";
        for (const lexer::token_ptr_t& token : tokens) {
            if (token->type != token::token_type::INTVAL) {
                return "Lexer did not correctly identify an integer literal.";
            }
        }

        if (std::static_pointer_cast<token::int_token>(tokens[0])->value != 1)
            return "Lexer did not correctly get the integer value of '1'";
        if (std::static_pointer_cast<token::int_token>(tokens[1])->value != 2)
            return "Lexer did not correctly get the integer value of '2'";
        if (std::static_pointer_cast<token::int_token>(tokens[2])->value
            != 345)  //  NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            return "Lexer did not correctly get the integer value of '345'";

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method correctly parses floating-point literals.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_floats() {
        const lexer::token_list_t tokens = lexer::lex_all("1. .2 3.4");

        if (tokens.size() != 3) return "Lexer did not return the correct number of tokens";
        for (const lexer::token_ptr_t& token : tokens) {
            if (token->type != token::token_type::FLOATVAL) {
                return "Lexer did not correctly identify a floating-point literal.";
            }
        }

        if (std::static_pointer_cast<token::float_token>(tokens[0])->value != 1.0)
            return "Lexer did not correctly get the floating-point value of '1.'";
        if (std::static_pointer_cast<token::float_token>(tokens[1])->value
            != 0.2)  //  NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            return "Lexer did not correctly get the floating-point value of '.2'";
        if (std::static_pointer_cast<token::float_token>(tokens[2])->value
            != 3.4)  //  NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            return "Lexer did not correctly get the floating-point value of '3.4'";

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method correctly parses string literals.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_strings() {
        const lexer::token_list_t tokens = lexer::lex_all("read image \"sample.png\" to img");

        if (tokens.size() != 5) {  //  NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
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
        if (*tokens[4] != token::token {strlen("read image \"sample.png\" to "), "img", token::token_type::VARIABLE})
            return "Lexer did not correctly lex the token 'img.'";

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
        token::token tok {0, "float", token::token_type::FLOATVAL};
        expected.emplace_back(tok);
        tok = {strlen("float "), "float_variable", token::token_type::VARIABLE};
        expected.emplace_back(tok);

        if (tokens.size() != expected.size()) return "Lexer did not return the correct number of tokens";
        if (*tokens[0] != expected[0]) return "Keyword token was not correct";
        if (*tokens[1] != expected[1]) return "Variable token was not correct";

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
        tests.emplace_back(lex_all_variables_with_keywords, "Lexer `lex_all`: variables with keywords");
        tests.emplace_back(lex_all_exception, "Lexer `lex_all`: throwing an exception");
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
