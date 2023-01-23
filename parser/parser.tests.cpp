/**
 * @file parser.tests.cpp
 * @package Assignment 3.
 * @author Cayden Lund (u1182408)
 * @brief Contains tests for the parser.
 *
 */

#include "parser.hpp"
#include "tests/tests.hpp"

namespace tests::parser_tests {
    /**
     * @brief Ensures that TEST
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string TEST() {
        //  TEST . . .

        return "";
    }

    /**
     * @brief Assembles the set of all parser unit tests.
     *
     * @return The set of all parser unit tests.
     */
    std::vector<test_t> get_all_tests() {
        std::vector<test_t> tests;
        //  tests.emplace_back(TEST, "TEST");

        return tests;
    }
}  //  namespace tests::parser_tests

/**
 * @brief Parser test program entry point.
 *
 */
int main() {
    std::vector<tests::test_t> tests = tests::parser_tests::get_all_tests();
    tests::run_tests(tests);

    return 0;
}
