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

#include "token/token.hpp"

namespace lexer {
    /**
     * @brief Defines the type of a pointer to a token.
     * @details Uses a reference-counting smart pointer.
     *
     */
    using token_ptr_t = std::shared_ptr<token::token>;

    /**
     * @brief Defines the type of a list of tokens.
     * @details The list of tokens is a vector of smart pointers, to allow for subclasses in the list.
     *
     */
    using token_list_t = std::vector<token_ptr_t>;

    /**
     * @brief Defines the result of a lexer call.
     * @details A tuple of the lexed token and the new starting position.
     *
     */
    using result_t = std::tuple<token_ptr_t, unsigned int>;

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
         * @param invalid_token The invalid character sequence causing the exception.
         * @param line The line number of the invalid character sequence.
         * @param column The column number of the invalid character sequence.
         */
        lexing_exception(const std::string& invalid_token, unsigned int line, unsigned int column);

        /**
         * @brief Returns a C-style string describing the exception cause.
         *
         * @return A C-style string describing the exception cause.
         */
        [[nodiscard]] const char* what() const noexcept override;
    };

    /**
     * @brief Defines the `lexer` functor class.
     * @details An instance is used to lex a particular case of input.
     *
     */
    class lexer {
    protected:
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
         * @brief Class constructor.
         *
         * @param pattern The regular expression used to match input.
         * @param type The token type that the lexer matches.
         */
        lexer(const std::string& pattern, token::token_type type);

        /**
         * @brief Class destructor.
         *
         */
        virtual ~lexer() = default;

        /**
         * @brief Copy constructor.
         *
         * @param other The other lexer to copy.
         */
        lexer(const lexer& other) = default;

        /**
         * @brief Move constructor.
         *
         * @param other The other lexer to move.
         */
        lexer(lexer&& other) = default;

        /**
         * @brief Assignment operator.
         *
         * @param other The other lexer to move.
         */
        lexer& operator=(const lexer& other) = delete;

        /**
         * @brief Move assignment operator.
         *
         * @param other The other lexer to move.
         */
        lexer& operator=(lexer&& other) = delete;

        /**
         * @brief Function call operator.
         * @details Call the instance as a function.
         *
         * @param input The input string to read.
         * @param index The starting position from which to read.
         * @return A pair of the lexed token and the new starting position.
         */
        virtual result_t operator()(const std::string& input, unsigned int index) const;
    };

    /**
     * @brief A lexer that can parse floating-point literals.
     *
     */
    class float_lexer : public lexer {
    public:
        /**
         * @brief Class constructor.
         *
         */
        float_lexer();

        /**
         * @brief Class destructor.
         *
         */
        ~float_lexer() override = default;

        /**
         * @brief Copy constructor.
         *
         * @param other The other lexer to copy.
         */
        float_lexer(const float_lexer& other) = default;

        /**
         * @brief Move constructor.
         *
         * @param other The other lexer to move.
         */
        float_lexer(float_lexer&& other) = default;

        /**
         * @brief Assignment operator.
         *
         * @param other The other lexer to move.
         */
        float_lexer& operator=(const float_lexer& other) = delete;

        /**
         * @brief Move assignment operator.
         *
         * @param other The other lexer to move.
         */
        float_lexer& operator=(float_lexer&& other) = delete;

        /**
         * @brief Function call operator.
         * @details Call the instance as a function.
         *
         * @param input The input string to read.
         * @param index The starting position from which to read.
         * @return A pair of the lexed token and the new starting position.
         */
        result_t operator()(const std::string& input, unsigned int index) const override;
    };

    /**
     * @brief A lexer that can parse integer literals.
     *
     */
    class int_lexer : public lexer {
    public:
        /**
         * @brief Class constructor.
         *
         */
        int_lexer();

        /**
         * @brief Class destructor.
         *
         */
        ~int_lexer() override = default;

        /**
         * @brief Copy constructor.
         *
         * @param other The other lexer to copy.
         */
        int_lexer(const int_lexer& other) = default;

        /**
         * @brief Move constructor.
         *
         * @param other The other lexer to move.
         */
        int_lexer(int_lexer&& other) = default;

        /**
         * @brief Assignment operator.
         *
         * @param other The other lexer to move.
         */
        int_lexer& operator=(const int_lexer& other) = delete;

        /**
         * @brief Move assignment operator.
         *
         * @param other The other lexer to move.
         */
        int_lexer& operator=(int_lexer&& other) = delete;

        /**
         * @brief Function call operator.
         * @details Call the instance as a function.
         *
         * @param input The input string to read.
         * @param index The starting position from which to read.
         * @return A pair of the lexed token and the new starting position.
         */
        result_t operator()(const std::string& input, unsigned int index) const override;
    };

    /**
     * @brief A lexer that can parse string literals.
     *
     */
    class string_lexer : public lexer {
    public:
        /**
         * @brief Class constructor.
         *
         */
        string_lexer();

        /**
         * @brief Class destructor.
         *
         */
        ~string_lexer() override = default;

        /**
         * @brief Copy constructor.
         *
         * @param other The other lexer to copy.
         */
        string_lexer(const string_lexer& other) = default;

        /**
         * @brief Move constructor.
         *
         * @param other The other lexer to move.
         */
        string_lexer(string_lexer&& other) = default;

        /**
         * @brief Assignment operator.
         *
         * @param other The other lexer to move.
         */
        string_lexer& operator=(const string_lexer& other) = delete;

        /**
         * @brief Move assignment operator.
         *
         * @param other The other lexer to move.
         */
        string_lexer& operator=(string_lexer&& other) = delete;

        /**
         * @brief Function call operator.
         * @details Call the instance as a function.
         *
         * @param input The input string to read.
         * @param index The starting position from which to read.
         * @return A pair of the lexed token and the new starting position.
         */
        result_t operator()(const std::string& input, unsigned int index) const override;
    };

    /**
     * @brief Defines the type of a pointer to a lexer.
     * @details Uses a reference-counting smart pointer.
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
     * @brief Given a regular expression and a token type, returns a smart pointer to a new `lexer` object.
     *
     * @param expression The regular expression to use.
     * @param type The token type to use.
     * @return A smart pointer to a new lexer.
     */
    inline lexer_ptr_t construct_lexer(const std::string& expression, const token::token_type& type) {
        return std::make_shared<lexer>(expression, type);
    }

    /**
     * @brief Given the input string, lexes it in its entirety.
     *
     * @param input The input string.
     * @return The series of tokens composing the input string.
     */
    token_list_t lex_all(const std::string& input);
}  //  namespace lexer

#endif
