/**
 * @file tests.cpp
 * @author Cayden Lund (u1182408)
 * @brief Contains utilities for writing and running unit tests.
 * @details See header file for documentation.
 *
 */

#include "tests.hpp"

namespace tests {
    void run_tests(std::vector<test_t>& tests) {
        std::cout << "Running " << tests.size() << " tests." << std::endl;

        unsigned int num_passed = 0;

        for (const test_t& test : tests) {
            const std::string name = std::get<1>(test);
            std::cout << "Running test \"" << name << "\"." << std::endl << "    ";
            std::string result;
            try {
                result = std::get<0>(test)();
            } catch (const std::exception& e) { result = "Test threw an exception: '" + std::string(e.what()) + "'"; }

            if (result.empty()) {
                num_passed++;
                std::cout << "PASSED." << std::endl;
            } else {
                std::cout << "FAILED WITH MESSAGE: \"" << result << "\"." << std::endl;
            }
        }

        std::cout << num_passed << "/" << tests.size() << " tests passed." << std::endl;
    }
}  //  namespace tests
