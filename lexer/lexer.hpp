/**
 * @file lexer.hpp
 * @package Assignment 2
 * @author Cayden Lund (u1182408)
 * @brief Defines the lexing functions.
 *
 */

#ifndef LEXER_HPP
#define LEXER_HPP

#include <functional>
#include <regex>
#include <tuple>
#include <utility>

#include "token/token.hpp"

namespace lexer {
    /**
     * @brief Defines the type of a pointer to a token.
     * @details Uses a reference-counting smart pointer.
     *
     */
    using token_ptr_t = std::shared_ptr<token::token>;

    /**
     * @brief Defines the result of a lexer call.
     * @details A tuple of the lexed token and the new starting position.
     *
     */
    using result_t = std::tuple<token_ptr_t, unsigned int>;

    /**
     * @brief Defines the type of a list of tokens.
     * @details The list of tokens is a vector of smart pointers, to allow for subclasses in the list.
     *
     */
    using token_list_t = std::vector<token_ptr_t>;

    /**
     * @brief Defines the `lexer` functor class.
     * @details An instance is used to lex a particular case of input.
     *
     */
    class lexer {
    private:
        /**
         * @brief The regular expression used to match input.
         *
         */
        const std::regex pattern;

        /**
         * @brief The token type to match.
         *
         */
        const token::token_type type;

    public:
        /**
         * @brief Constructor.
         *
         * @param pattern The regular expression used to match input.
         */
        lexer(const std::string& pattern, token::token_type type);

        /**
         * @brief Function call operator.
         * @details Call the instance as a function.
         *
         * @param input The input string to read.
         * @param index The starting position from which to read.
         * @return A pair of the lexed token and the new starting position.
         */
        result_t operator()(const std::string& input, unsigned int index) const;
    };

    /**
     * @brief Assembles the set of lexers in the correct order.
     *
     * @return The set of lexers to use, in the correct order.
     */
    std::vector<lexer> assemble_lexers();

    /**
     * @brief Given the input string, lexes it in its entirety.
     *
     * @param input The input string.
     * @return The series of tokens composing the input string.
     */
    token_list_t lex_all(const std::string& input);
}  //  namespace lexer

#endif
