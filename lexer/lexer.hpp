/**
 * @file lexer.hpp
 * @package Assignment 2
 * @author Cayden Lund (u1182408)
 * @brief Defines the lexing functions.
 *
 */

#ifndef LEXER_HPP
#define LEXER_HPP

#include <exception>
#include <functional>
#include <regex>
#include <tuple>
#include <utility>

#include "file/file.hpp"
#include "token/token.hpp"

namespace lexer {
    /**
     * @brief Defines the type of a list of tokens.
     * @details The list of tokens is a vector of smart pointers, to allow for subclasses in the list.
     *
     */
    using token_list_t = std::vector<token::token>;

    /**
     * @brief Defines the result of a lexer call.
     * @details A tuple of the lexed token and the new starting position.
     *
     */
    using result_t = std::tuple<token::token, unsigned int>;

    /**
     * @brief This exception is thrown when there is not a lexer that can lex the input string.
     * @details This happens when there's an invalid character in the file, or a series of characters that doesn't form
     *     a valid token.
     *
     */
    class lexing_exception : public std::runtime_error {
    private:
        /**
         * @brief The error message to print.
         *
         */
        const std::string message;

    public:
        /**
         * @brief The maximum length of a token snippet.
         *
         */
        constexpr static unsigned int max_token_snippet_length = 7;

        /**
         * @brief Constructor.
         *
         * @param invalid_input The invalid character sequence causing the exception.
         * @param index The byte position in the invalid input string.
         */
        lexing_exception(const std::string& invalid_input, unsigned int index);
    };

    /**
     * @brief Defines a single lexer.
     * @details A lexer is a functor that accepts an input string and an index in that string, and returns a tuple of
     *     the corresponding token and the updated index.
     *
     */
    class lexer {
    public:
        /**
         * @brief Class destructor.
         *
         */
        virtual ~lexer() = default;

        /**
         * @brief Function call operator.
         *
         * @param input The input string.
         * @param index The index in the input string.
         * @return A tuple of a new token and the new index.
         */
        virtual result_t operator()(const std::string& input, unsigned int index);

        /**
         * @brief Reports whether the given byte is a valid character in JPL.
         * @details Valid characters include integer value 10 ('\n') and 32-126, inclusive.
         *
         * @param byte The character to test.
         * @return True when the given byte is a valid character.
         */
        static bool is_valid_char(char byte);
    };

    /**
     * @brief A lexer for comments.
     *
     */
    class comment_lexer : public lexer {
        /**
         * @brief Function call operator.
         *
         * @param input The input string.
         * @param index The index in the input string.
         * @return A tuple of a new token and the new index.
         */
        result_t operator()(const std::string& input, unsigned int index) override;
    };

    /**
     * @brief A lexer for integer and floating-point literals.
     *
     */
    class literal_lexer : public lexer {
        /**
         * @brief Function call operator.
         *
         * @param input The input string.
         * @param index The index in the input string.
         * @return A tuple of a new token and the new index.
         */
        result_t operator()(const std::string& input, unsigned int index) override;
    };

    /**
     * @brief A lexer for newlines.
     *
     */
    class newline_lexer : public lexer {
        /**
         * @brief Function call operator.
         *
         * @param input The input string.
         * @param index The index in the input string.
         * @return A tuple of a new token and the new index.
         */
        result_t operator()(const std::string& input, unsigned int index) override;
    };

    /**
     * @brief A lexer for operators.
     *
     */
    class operator_lexer : public lexer {
        /**
         * @brief Function call operator.
         *
         * @param input The input string.
         * @param index The index in the input string.
         * @return A tuple of a new token and the new index.
         */
        result_t operator()(const std::string& input, unsigned int index) override;
    };

    /**
     * @brief A lexer for punctuation.
     *
     */
    class punctuation_lexer : public lexer {
        /**
         * @brief Function call operator.
         *
         * @param input The input string.
         * @param index The index in the input string.
         * @return A tuple of a new token and the new index.
         */
        result_t operator()(const std::string& input, unsigned int index) override;
    };

    /**
     * @brief A lexer for strings.
     *
     */
    class string_lexer : public lexer {
        /**
         * @brief Function call operator.
         *
         * @param input The input string.
         * @param index The index in the input string.
         * @return A tuple of a new token and the new index.
         */
        result_t operator()(const std::string& input, unsigned int index) override;
    };

    /**
     * @brief A lexer for variables and keywords.
     *
     */
    class variable_keyword_lexer : public lexer {
        /**
         * @brief Function call operator.
         *
         * @param input The input string.
         * @param index The index in the input string.
         * @return A tuple of a new token and the new index.
         */
        result_t operator()(const std::string& input, unsigned int index) override;
    };

    /**
     * @brief This type is a smart pointer to a lexer.
     *
     */
    using lexer_ptr_t = std::shared_ptr<lexer>;

    /**
     * @brief Assembles the set of lexers in the correct order.
     *
     * @return The set of lexers to use, in the correct order.
     */
    std::vector<lexer_ptr_t> assemble_lexers();

    /**
     * @brief Given the input string, lexes it in its entirety.
     *
     * @param input The input string.
     * @return The series of tokens composing the input string.
     */
    token_list_t lex_all(const std::string& input);
}  //  namespace lexer

#endif
