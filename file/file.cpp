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
    /**
     * @brief Holds the line numbers of the read file.
     * @details Line 0 starts at byte 0, etc.
     *     TODO: Find a more elegant way to avoid a global variable.
     *
     */
    std::vector<unsigned int>  //  NOLINT(fuchsia-statically-constructed-objects)
            lines {0};         //  NOLINT(cppcoreguidelines-avoid-non-const-global-variables,cert-err58-cpp)

    std::string read_file(const std::string& filename) {
        lines = std::vector<unsigned int> {0};

        std::ifstream file;
        file.open(filename);
        if (file.is_open()) {
            std::string input;
            std::stringstream output;

            while (std::getline(file, input, ' ')) { output << input << " "; }

            const std::string result = output.str();
            for (unsigned int index = 0; index < (unsigned int)result.size(); index++) {
                if (result[index] == '\n') lines.emplace_back(index);
            }
        }

        throw std::runtime_error("Filesystem error: file \"" + filename + "\" could not be opened for reading");
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
