/**
 * @file tests.hpp
 * @author Cayden Lund (u1182408)
 * @brief Defines utilities for writing and running unit tests.
 *
 */

#ifndef TESTS_HPP
#define TESTS_HPP

#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

namespace tests {
    /**
     * @brief Defines the method signature for a single unit test.
     * @details The function returns the empty string if the test passes, and an
     *          error message otherwise.
     *          The string in the tuple is the name of the test.
     *
     */
    typedef std::tuple<std::function<std::string()>, std::string> test_t;

    /**
     * @brief Runs the given sequence of unit tests.
     *
     * @param tests The sequence of unit tests to run.
     */
    void run_tests(std::vector<test_t>& tests);
}  //  namespace tests

#endif
