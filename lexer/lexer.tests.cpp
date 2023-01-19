/**
 * @file lexer.tests.cpp
 * @package Assignment 2.
 * @author Cayden Lund (u1182408)
 * @brief Contains tests for the lexer.
 *
 */

#define STRLEN(x) ((unsigned int)std::string(x).length())

#include "lexer.hpp"
#include "tests/tests.hpp"

namespace tests::lexer_tests {
    /**
     * @brief Ensures that the `lexer` constructor does not throw an exception.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string constructor_no_fail() {
        try {
            const lexer::lexer empty_pattern("", token::token_type::ARRAY);
            const lexer::lexer valid_regex("^abc.*", token::token_type::ARRAY);
            const lexer::lexer invalid_regex(R"(\a\b\c\d)", token::token_type::ARRAY);
        } catch (std::exception& e) { return "Lexer constructor threw exception '" + std::string(e.what()) + "'"; }

        return "";
    }

    /**
     * @brief Ensures that a lexer does not match an input that it should not match.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lexer_no_match() {
        try {
            const lexer::lexer array_matcher("^array", token::token_type::ARRAY);
            lexer::result_t result = array_matcher("other phrase", 0);

            if (std::get<1>(result) != 0) return "Lexer matched an input that it should not match";
        } catch (std::exception& e) { return "Lexer threw exception '" + std::string(e.what()) + "'"; }

        return "";
    }

    /**
     * @brief Ensures that a lexer does match an input that it should match.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lexer_match() {
        try {
            const lexer::lexer array_matcher("^array", token::token_type::ARRAY);
            lexer::result_t result = array_matcher("array other phrase", 0);

            if (std::get<1>(result) != std::string("array").length())
                return "Lexer did not match an input that it should match";

            const token::token actual = std::get<0>(result);
            const token::token expected {0, "array", token::token_type::ARRAY};
            if (actual != expected) return "The returned token was not correct.";
        } catch (std::exception& e) { return "Lexer threw exception '" + std::string(e.what()) + "'"; }

        return "";
    }

    /**
     * @brief Ensures that the `lexer::assemble_lexers` method does not throw an exception.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string assemble_lexers_no_fail() {
        try {
            lexer::assemble_lexers();
        } catch (std::exception& e) {
            return "`lexer::assemble_lexers` threw exception '" + std::string(e.what()) + "'";
        }

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method can correctly lex a series of keywords.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_keywords() {
        try {
            std::vector<token::token> actual = lexer::lex_all("array bool assert else");

            std::vector<token::token> expected;
            token::token tok = {0, "array", token::token_type::ARRAY};
            expected.push_back(tok);
            tok = {STRLEN("array "), "bool", token::token_type::BOOL};
            expected.push_back(tok);
            tok = {STRLEN("array bool "), "assert", token::token_type::ASSERT};
            expected.push_back(tok);
            tok = {STRLEN("array bool assert "), "else", token::token_type::ELSE};
            expected.push_back(tok);

            if (actual.size() != expected.size()) return "Lexer did not return the correct number of tokens";
            for (unsigned int i = 0; i < actual.size(); i++) {
                if (actual[i] != expected[i]) return "Token " + std::to_string(i) + " was not correct";
            }

        } catch (std::exception& e) { return "`lexer::lex_all` threw exception '" + std::string(e.what()) + "'"; }

        return "";
    }

    /**
     * @brief Ensures that the `lexer::lex_all` method correctly lexes all punctuation marks.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lex_all_punctuation() {
        try {
            std::vector<token::token> actual = lexer::lex_all(": , ( {{");

            std::vector<token::token> expected;
            token::token tok = {0, ":", token::token_type::COLON};
            expected.push_back(tok);
            tok = {STRLEN(": "), ",", token::token_type::COMMA};
            expected.push_back(tok);
            tok = {STRLEN(": , "), "(", token::token_type::LPAREN};
            expected.push_back(tok);
            tok = {STRLEN(": , ( "), "{", token::token_type::LCURLY};
            expected.push_back(tok);
            tok = {STRLEN(": , ( {"), "{", token::token_type::LCURLY};
            expected.push_back(tok);

            if (actual.size() != expected.size()) return "Lexer did not return the correct number of tokens";
            for (unsigned int i = 0; i < actual.size(); i++) {
                if (actual[i] != expected[i]) return "Token " + std::to_string(i) + " was not correct";
            }

        } catch (std::exception& e) { return "`lexer::lex_all` threw exception '" + std::string(e.what()) + "'"; }

            return "";
        }

    /**
     * @brief Assembles the set of all lexer unit tests.
     *
     * @return The set of all lexer unit tests.
     */
    std::vector<test_t> get_all_tests() {
        std::vector<test_t> tests;
        tests.emplace_back(constructor_no_fail, "Lexer constructor no-fail");
        tests.emplace_back(lexer_no_match, "Lexer no-match");
        tests.emplace_back(lexer_match, "Lexer match");
        tests.emplace_back(assemble_lexers_no_fail, "Lexer `assemble_lexer` no-fail");
        tests.emplace_back(lex_all_keywords, "Lexer `lex_all` keywords");
        tests.emplace_back(lex_all_punctuation, "Lexer `lex_all` punctuation");
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
