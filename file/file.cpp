/**
 * @file file.cpp
 * @package Assignment 2
 * @author Cayden Lund (u1182408)
 * @brief Implements the file-related functions.
 * @details See the header file for documentation.
 *
 */

#include "file.hpp"

namespace file {
    std::string read_file(const std::string& filename) {
        lines = std::vector<unsigned int> {0};

        std::ifstream file;
        file.open(filename);
        if (file.is_open()) {
            std::string input;
            std::stringstream output;

            while (std::getline(file, input)) {
                output << input;
                lines.push_back(lines[lines.size() - 1] + input.length());
            }

            return output.str();
        }

        //  TODO: Handle filesystem errors.
        throw std::runtime_error("Filesystem error!");
    }

    unsigned int get_line(unsigned int byte_position) {
        for (unsigned int index = 1; index < lines.size(); index++) {
            if (lines[index] > byte_position) return index;
        }
        return lines.size();
    }

    unsigned int get_column(unsigned int byte_position) {
        for (unsigned int index = 1; index < lines.size(); index++) {
            if (lines[index] > byte_position) return byte_position - lines[index - 1] + 1;
        }
        return byte_position - lines[lines.size() - 1] + 1;
    }
}  //  namespace file
