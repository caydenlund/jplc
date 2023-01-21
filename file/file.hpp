/**
 * @file file.hpp
 * @package Assignment 2
 * @author Cayden Lund (u1182408)
 * @brief Defines the file-related functions.
 *
 */

#ifndef FILE_HPP
#define FILE_HPP

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace file {
    /**
     * @brief Holds the line numbers of the read file.
     * @details Line 0 starts at byte 0, etc.
     *     TODO: Find a more elegant way to avoid a global variable.
     *
     */
    std::vector<unsigned int>  //  NOLINT(fuchsia-statically-constructed-objects)
            lines {0};         //  NOLINT(cppcoreguidelines-avoid-non-const-global-variables,cert-err58-cpp)

    /**
     * @brief Given a filename, reads it into memory.
     *
     * @param filename The name of the file to read.
     * @return The read file.
     */
    std::string read_file(const std::string& filename);

    /**
     * @brief Given a byte position in a file, returns the line.
     *
     */
    unsigned int get_line(unsigned int byte_position);

    /**
     * @brief Given a byte position in a file, returns the column.
     *
     */
    unsigned int get_column(unsigned int byte_position);

}  //  namespace file

#endif
