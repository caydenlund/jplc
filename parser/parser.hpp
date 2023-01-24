/**
 * @file parser.hpp
 * @package Assignment 3
 * @author Cayden Lund (u1182408)
 * @brief Defines the parser functions.
 *
 */

#include <memory>
#include <tuple>
#include <vector>

#include "ast_node/ast_node.hpp"
#include "token/token.hpp"

#ifndef PARSER_HPP
#define PARSER_HPP

namespace parser {
    /**
     * @brief A const reference to a vector of tokens.
     *
     */
    using token_vec_t = const std::vector<token::token>&;

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
     * @brief The main parser routine.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @return A set of parsed AST nodes.
     */
    std::vector<node_ptr_t> parse(token_vec_t tokens);

    /**
     * @brief Finds the index of the next token of the given type in the given tokens vector.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param start The starting index from which to search.
     * @param type The type of token for which to search.
     * @return The index of the next token of the given type, or `tokens.size()` if no such token exists.
     */
    unsigned int find_next(token_vec_t tokens, unsigned int start, token::token_type type);

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
    node_ptr_t parse_argument_variable(token_vec_t tokens, unsigned int index);

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
    node_ptr_t parse_cmd_assert(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `let` command.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    node_ptr_t parse_cmd_let(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `print` command.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    node_ptr_t parse_cmd_print(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `read` command.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    node_ptr_t parse_cmd_read(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `show` command.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    node_ptr_t parse_cmd_show(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `type` command.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    node_ptr_t parse_cmd_type(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `write` command.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    node_ptr_t parse_cmd_write(token_vec_t tokens, unsigned int index);

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
    node_ptr_t parse_expr_false(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `<float>` expression.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    node_ptr_t parse_expr_float(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `<integer>` expression.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    node_ptr_t parse_expr_integer(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `true` expression.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    node_ptr_t parse_expr_true(token_vec_t tokens, unsigned int index);

    /**
     * @brief Parses a single `<variable>` expression.
     * @details Throws an exception if unsuccessful.
     *
     * @param tokens A sequence of tokens, generated by the lexer.
     * @param index The index in the tokens vector from which to parse.
     * @return A parsed AST node.
     */
    node_ptr_t parse_expr_variable(token_vec_t tokens, unsigned int index);

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
    node_ptr_t parse_lvalue_argument(token_vec_t tokens, unsigned int index);

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
    parser_return_t parse_type_bool(token_vec_t tokens);

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
