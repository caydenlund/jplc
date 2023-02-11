/**
 * @file parser.hpp
 * @package Assignment 3-4.
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
     * @brief A list of AST node pointers.
     *
     */
    using node_list_t = std::vector<node_ptr_t>;

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
    node_list_t parse(token_vec_t tokens);

    /**
     * @brief Parses a sequence of like AST nodes.
     *
     * @param parser_function The specific parser function to use.
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A set of parsed AST nodes.
     */
    std::tuple<node_list_t, unsigned int> parse_sequence(const parser_t& parser_function, token_vec_t tokens,
                                                         unsigned int index);

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

    /**
     * @brief Given a sequence of tokens and a starting index, returns all the expression-compatible tokens separated
     * into sub-expressions of operators and non-operators.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index from which to read tokens.
     * @return A vector of vectors of tokens.
     */
    std::vector<std::vector<token::token>> split_tokens(token_vec_t tokens, unsigned int index);

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
     * @brief Parses a single binding.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_binding(token_vec_t tokens, unsigned int index);

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
     * @brief Parses a single statement.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_statement(token_vec_t tokens, unsigned int index);

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
     * @brief Parses a single `<variable>[<variable>, ...]` argument.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_argument_array(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `<variable>` argument.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_argument_variable(token_vec_t tokens, unsigned int index);

    //  Bindings:
    //  ---------
    /**
     * @brief Parses a single tuple binding.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_binding_tuple(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `<variable>` binding.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_binding_variable(token_vec_t tokens, unsigned int index);

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
     * @brief Parses a single `fn <variable> (<binding>, ...) : <type> {; <stmt> ; ... ;}` command.
     * @details Throws an exception if unsuccessful.
     *     In the above specification, the `;` stands for a newline.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_cmd_fn(token_vec_t tokens, unsigned int index);

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
     * @brief Parses a single `time` command.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_cmd_time(token_vec_t tokens, unsigned int index);

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
     * @brief Parses a single `<expr>[<expr>, ...]` expression.
     * @details Throws an exception if unsuccessful.
     *
     * @param array A pointer to the main `<expr>` array node.
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_expr_array_index(const std::shared_ptr<ast_node::expr_node> array, token_vec_t tokens,
                                           unsigned int index);

    /**
     * @brief Parses a single `[<expr>, ...]` expression.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_expr_array_literal(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `array[<variable> : <expr>, ...] <expr>` expression.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_expr_array_loop(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `<variable>(<expr>, ...)` expression.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_expr_call(token_vec_t tokens, unsigned int index);

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
     * @brief Parses a single `if <expr> then <expr> else <expr>` expression.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_expr_if(token_vec_t tokens, unsigned int index);

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
     * @brief Parses a single `(<expr>)` expression.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_expr_parentheses(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `sum[<variable> : <expr>, ...] <expr>` expression.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_expr_sum_loop(token_vec_t tokens, unsigned int index);

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
     * @brief Parses a single `<expr>{<integer>}` expression.
     * @details Throws an exception if unsuccessful.
     *
     * @param tuple A pointer to the main `<expr>` tuple node.
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_expr_tuple_index(std::shared_ptr<ast_node::expr_node> tuple, token_vec_t tokens,
                                           unsigned int index);

    /**
     * @brief Parses a single `{<expr>, ...}` expression.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_expr_tuple_literal(token_vec_t tokens, unsigned int index);

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

    /**
     * @brief Parses a single `{<lvalue>, ...}` lvalue.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_lvalue_tuple(token_vec_t tokens, unsigned int index);

    //  Statements:
    //  -----------
    /**
     * @brief Parses a single `assert` statement.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_statement_assert(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `let` statement.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_statement_let(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `return` statement.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_statement_return(token_vec_t tokens, unsigned int index);

    //  Types:
    //  ------
    /**
     * @brief Parses a single `<type>[, ...]` type.
     * @details Throws an exception if unsuccessful.
     *
     * @param type A pointer to the main `<type>` of the array.
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_type_array(const std::shared_ptr<ast_node::type_node> type, token_vec_t tokens,
                                     unsigned int index);

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
     * @brief Parses a single `{<type>, ...}` type.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_type_tuple(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `<variable>` type.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    parser_return_t parse_type_variable(token_vec_t tokens, unsigned int index);

    /*
    ==================
    ||  Constants:  ||
    ==================
    */
    /**
     * @brief The set of argument parsers.
     *
     */
    const std::vector<parser_t> argument_parsers {parse_argument_array, parse_argument_variable};

    /**
     * @brief The set of binding parsers.
     *
     */
    const std::vector<parser_t> binding_parsers {parse_binding_tuple, parse_binding_variable};

    /**
     * @brief The set of command parsers.
     *
     */
    const std::vector<parser_t> command_parsers {parse_cmd_assert, parse_cmd_fn,   parse_cmd_let,
                                                 parse_cmd_print,  parse_cmd_read, parse_cmd_show,
                                                 parse_cmd_time,   parse_cmd_type, parse_cmd_write};

    /**
     * @brief The set of expression parsers.
     *
     */
    const std::vector<parser_t> expression_parsers {
            parse_expr_array_literal, parse_expr_array_loop, parse_expr_call,          parse_expr_false,
            parse_expr_float,         parse_expr_if,         parse_expr_integer,       parse_expr_parentheses,
            parse_expr_sum_loop,      parse_expr_true,       parse_expr_tuple_literal, parse_expr_variable};

    /**
     * @brief The set of lvalue parsers.
     *
     */
    const std::vector<parser_t> lvalue_parsers {parse_lvalue_argument, parse_lvalue_tuple};

    /**
     * @brief The set of statement parsers.
     *
     */
    const std::vector<parser_t> statement_parsers {parse_statement_assert, parse_statement_let, parse_statement_return};

    /**
     * @brief The set of type parsers.
     *
     */
    const std::vector<parser_t> type_parsers {parse_type_bool, parse_type_int, parse_type_float, parse_type_tuple,
                                              parse_type_variable};
}  //  namespace parser

#endif
