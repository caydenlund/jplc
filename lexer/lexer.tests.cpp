/**
 * @file lexer.tests.cpp
 * @package TODO: Fill in assignment.
 * @author Cayden Lund (u1182408)
 * @brief Contains tests for the lexer.
 *
 */

#include "lexer.hpp"
#include "tests/tests.hpp"

namespace tests::lexer_tests {
    /**
     * @brief Ensures that the `lexer` constructor does not throw an exception.
     *
     */
    std::string constructor_no_fail() {
        try {
            lexer::lexer empty_pattern("", token::token_type::ARRAY);
            lexer::lexer valid_regex("^abc.*", token::token_type::ARRAY);
            lexer::lexer invalid_regex(R"(\a\b\c\d)", token::token_type::ARRAY);
        } catch (std::exception& e) { return "Lexer constructor threw exception '" + std::string(e.what()) + "'."; }

        return "";
    }

    /**
     * @brief Returns the set of all lexer unit tests.
     *
     * @return The set of all lexer unit tests.
     */
    std::vector<test_t> get_all_tests() {
        std::vector<test_t> tests;
        tests.emplace_back(constructor_no_fail, "Lexer constructor no-fail");
        return tests;
    }
}  //  namespace tests::lexer_tests

int main() {
    std::vector<tests::test_t> tests = tests::lexer_tests::get_all_tests();
    tests::run_tests(tests);
    return 0;
}
