/**
 * @file parser.hpp
 * @package Assignment 3
 * @author Cayden Lund (u1182408)
 * @brief Defines the parser functions.
 *
 */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <functional>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <vector>

#include "ast_node/ast_node.hpp"
#include "file/file.hpp"
#include "token/token.hpp"

namespace parser {
    /*
    ==============
    ||  Types:  ||
    ==============
    */
    /**
     * @brief A const reference to a vector of smart pointers to tokens.
     *
     */
    using token_vec_t = const std::vector<std::shared_ptr<token::token>>&;

    /**
     * @brief A pointer to an AST node.
     *
     */
    using node_ptr_t = std::shared_ptr<ast_node::ast_node>;

    /**
     * @brief The return type of any single parser.
     * @details A pair of a pointer to a new AST node and the new index in the tokens vector.
     *
     */
    using parser_return_t = std::tuple<node_ptr_t, unsigned int>;

    /**
     * @brief The type of a single parser.
     * @details I.e., a function that takes a vector of tokens and an index and returns a `parser_return_t`.
     *
     */
    using parser_t = std::function<parser_return_t(token_vec_t, unsigned int)>;

    /**
     * @brief The exception that is thrown when a parser fails to parse the sequence of tokens.
     *
     */
    struct parser_error : public std::runtime_error {
        /**
         * @brief Class constructor.
         *
         * @param message The message to pass on to `std::runtime_error`.
         */
        parser_error(const std::string& message) : std::runtime_error(message) {}
    };

    struct parser_error_recoverable : public parser_error {
        /**
         * @brief Class constructor.
         *
         */
        parser_error_recoverable() : parser_error("") {}
    };

    struct parser_error_unrecoverable : public parser_error {
        /**
         * @brief Class constructor.
         *
         * @param byte_position The byte index in the file where the error originated.
         */
        parser_error_unrecoverable(unsigned int byte_position)
            : parser_error("The parser failed to parse the sequence of tokens at "
                           + std::to_string(file::get_line(byte_position)) + ":"
                           + std::to_string(file::get_column(byte_position))) {}

        /**
         * @brief Class constructor.
         *
         * @param message The message to pass on to `std::runtime_error`.
         */
        parser_error_unrecoverable(const std::string& message) : parser_error(message) {}
    };

    /**
     * @brief The exception that is thrown when a parser fails to parse the sequence of tokens.
     * @details This one is thrown when the file ends unexpectedly.
     *
     */
    struct parser_error_eof : public parser_error_unrecoverable {
        /**
         * @brief Class constructor.
         *
         */
        parser_error_eof() : parser_error_unrecoverable("The file ended unexpectedly") {}
    };

    /**
     * @brief The exception that is thrown when a parser fails to parse the sequence of tokens.
     * @details This one is thrown when the line ends unexpectedly.
     *
     */
    struct parser_error_newline : public parser_error_unrecoverable {
        /**
         * @brief Class constructor.
         *
         * @param byte_position The byte index in the file where the error originated.
         */
        parser_error_newline(unsigned int byte_position)
            : parser_error_unrecoverable("Line " + std::to_string(file::get_line(byte_position))
                                         + " ended unexpectedly") {}
    };

    /**
     * @brief The exception that is thrown when a parser fails to parse the sequence of tokens.
     * @details This one is thrown when the given token was not the expected token.
     *
     */
    struct parser_error_token_mismatch : public parser_error_unrecoverable {
        /**
         * @brief Class constructor.
         *
         * @param byte_position The byte index in the file where the error originated.
         */
        parser_error_token_mismatch(unsigned int byte_position, token::token_type actual_type,
                                    token::token_type expected_type)
            : parser_error_unrecoverable("Token at " + std::to_string(file::get_line(byte_position)) + ":"
                                         + std::to_string(file::get_column(byte_position))
                                         + " was not the expected type (" + token::token_type_to_string(actual_type)
                                         + " instead of " + token::token_type_to_string(expected_type) + ")") {}
    };

    /**
     * @brief The exception that is thrown when a parser fails to parse the sequence of tokens.
     * @details This one is thrown when one line contains more than a single command.
     *
     */
    struct parser_error_trailing_token : public parser_error_unrecoverable {
        /**
         * @brief Class constructor.
         *
         * @param byte_position The byte index in the file where the error originated.
         */
        parser_error_trailing_token(unsigned int byte_position)
            : parser_error_unrecoverable("Extra token at " + std::to_string(file::get_line(byte_position)) + ":"
                                         + std::to_string(file::get_column(byte_position))
                                         + " exists after the end of a command") {}
    };

    /*
    ==================
    ||  Functions:  ||
    ==================
    */
    /**
     * @brief The main parser routine.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @return A set of parsed AST nodes.
     */
    std::vector<node_ptr_t> parse(token_vec_t tokens);

    /**
     * @brief Applies the given set of parsers to the given set of tokens.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param parsers The set of parsers to apply.
     * @param index The index on which to apply the parsers.
     * @return A parsed AST node.
     */
    parser_return_t apply_parsers(token_vec_t tokens, const std::vector<parser_t>& parsers, unsigned int index);

    /*
    ==============================
    ||  AST Node Superclasses:  ||
    ==============================
    */
    /**
     * @brief Parses a single argument.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_argument(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single command.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_cmd(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single expression.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_expr(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single lvalue.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_lvalue(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single type.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_type(token_vec_t tokens, unsigned int index);

    /*
    ============================
    ||  AST Node Subclasses:  ||
    ============================
    */
    //  Arguments:
    //  ----------
    /**
     * @brief Parses a single `<variable>` argument.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_argument_variable(token_vec_t tokens, unsigned int index);

    //  Commands:
    //  ---------
    /**
     * @brief Parses a single `assert` command.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_cmd_assert(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `let` command.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_cmd_let(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `print` command.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_cmd_print(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `read` command.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_cmd_read(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `show` command.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_cmd_show(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `type` command.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_cmd_type(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `write` command.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_cmd_write(token_vec_t tokens, unsigned int index);

    //  Expressions:
    //  ------------
    /**
     * @brief Parses a single `false` expression.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_expr_false(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `<float>` expression.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_expr_float(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `<integer>` expression.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_expr_integer(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `true` expression.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_expr_true(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `<variable>` expression.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_expr_variable(token_vec_t tokens, unsigned int index);

    //  Lvalues:
    //  --------
    /**
     * @brief Parses a single `<argument>` lvalue.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_lvalue_argument(token_vec_t tokens, unsigned int index);

    //  Types:
    //  ------
    /**
     * @brief Parses a single `bool` type.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_type_bool(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `float` type.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_type_float(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `int` type.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_type_int(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `<variable>` type.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_type_variable(token_vec_t tokens, unsigned int index);
}  //  namespace parser

#endif
