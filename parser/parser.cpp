/**
 * @file parser.cpp
 * @package Assignment 3
 * @author Cayden Lund (u1182408)
 * @brief Implements the parser functions.
 * @details See header file for documentation.
 *
 */

#include "parser.hpp"

namespace parser {
    std::vector<node_ptr_t> parse(token_vec_t tokens) {
        //  TODO: Implement.
        return {};
    }

    unsigned int find_next(token_vec_t tokens, unsigned int start, token::token_type type) {
        //  TODO: Implement.
        return {};
    }

    /*
    ==============================
    ||  AST Node Superclasses:  ||
    ==============================
    */
    parser_return_t parse_argument(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    parser_return_t parse_cmd(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    parser_return_t parse_expr(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    parser_return_t parse_lvalue(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    parser_return_t parse_type(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    /*
    ============================
    ||  AST Node Subclasses:  ||
    ============================
    */
    //  Arguments:
    //  ----------
    node_ptr_t parse_argument_variable(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    //  Commands:
    //  ---------
    node_ptr_t parse_cmd_assert(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    node_ptr_t parse_cmd_let(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    node_ptr_t parse_cmd_print(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    node_ptr_t parse_cmd_read(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    node_ptr_t parse_cmd_show(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    node_ptr_t parse_cmd_type(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    node_ptr_t parse_cmd_write(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    //  Expressions:
    //  ------------
    node_ptr_t parse_expr_false(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    node_ptr_t parse_expr_float(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    node_ptr_t parse_expr_integer(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    node_ptr_t parse_expr_true(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    node_ptr_t parse_expr_variable(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    //  Lvalues:
    //  --------
    node_ptr_t parse_lvalue_argument(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    //  Types:
    //  ------
    parser_return_t parse_type_bool(token_vec_t tokens) {
        //  TODO: Implement.
        return {};
    }

    parser_return_t parse_type_float(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    parser_return_t parse_type_int(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    parser_return_t parse_type_variable(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }
}  //  namespace parser
