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
        for (unsigned int index = start; index < (unsigned int)tokens.size(); index++) {
            if (tokens[index]->type == type) { return index; }
        }
        return (unsigned int)tokens.size();
    }

    /*
    ==============================
    ||  AST Node Superclasses:  ||
    ==============================
    */
    parser_return_t parse_argument(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        const std::vector<parser_t> argument_parsers {parse_argument_variable};
        return {};
    }

    parser_return_t parse_cmd(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        const std::vector<parser_t> cmd_parsers {parse_cmd_assert, parse_cmd_let,  parse_cmd_print, parse_cmd_read,
                                                 parse_cmd_show,   parse_cmd_type, parse_cmd_write};
        return {};
    }

    parser_return_t parse_expr(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        const std::vector<parser_t> expr_parsers {parse_expr_false, parse_expr_float, parse_expr_integer,
                                                  parse_expr_true, parse_expr_variable};
        return {};
    }

    parser_return_t parse_lvalue(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        const std::vector<parser_t> lvalue_parsers {parse_lvalue_argument};
        return {};
    }

    parser_return_t parse_type(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        const std::vector<parser_t> type_parsers {parse_type_bool, parse_type_float, parse_type_int,
                                                  parse_type_variable};
        return {};
    }

    /*
    ============================
    ||  AST Node Subclasses:  ||
    ============================
    */
    //  Arguments:
    //  ----------
    parser_return_t parse_argument_variable(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    //  Commands:
    //  ---------
    parser_return_t parse_cmd_assert(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    parser_return_t parse_cmd_let(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    parser_return_t parse_cmd_print(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    parser_return_t parse_cmd_read(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    parser_return_t parse_cmd_show(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    parser_return_t parse_cmd_type(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    parser_return_t parse_cmd_write(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    //  Expressions:
    //  ------------
    parser_return_t parse_expr_false(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    parser_return_t parse_expr_float(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    parser_return_t parse_expr_integer(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    parser_return_t parse_expr_true(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    parser_return_t parse_expr_variable(token_vec_t tokens, unsigned int index) {
        //  TODO: Implement.
        return {};
    }

    //  Lvalues:
    //  --------
    parser_return_t parse_lvalue_argument(token_vec_t tokens, unsigned int index) {
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
