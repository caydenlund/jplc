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
        unsigned int index = 0;
        std::vector<node_ptr_t> nodes;

        while (index < tokens.size() - 1) {
            parser_return_t result = apply_parsers(tokens, {parse_cmd}, index);
            nodes.push_back(std::get<0>(result));
            index = std::get<1>(result);
        }

        return nodes;
    }

    parser_return_t apply_parsers(token_vec_t tokens, const std::vector<parser_t>& parsers, unsigned int index) {
        //  Try all the parsers, one by one.
        for (const parser_t& parser : parsers) {
            try {
                return parser(tokens, index);
            } catch (const parser_error_recoverable& err) {
                //  Ignore recoverable errors and try the next parser.
            }
        }

        //  No parsers were successful.
        throw parser_error_unrecoverable(tokens[index]->start);
    }

    /*
    ==============================
    ||  AST Node Superclasses:  ||
    ==============================
    */
    parser_return_t parse_argument(token_vec_t tokens, unsigned int index) {
        return apply_parsers(tokens, {parse_argument_variable}, index);
    }

    parser_return_t parse_cmd(token_vec_t tokens, unsigned int index) {
        return apply_parsers(tokens,
                             {parse_cmd_assert, parse_cmd_let, parse_cmd_print, parse_cmd_read, parse_cmd_show,
                              parse_cmd_type, parse_cmd_write},
                             index);
    }

    parser_return_t parse_expr(token_vec_t tokens, unsigned int index) {
        return apply_parsers(
                tokens, {parse_expr_false, parse_expr_float, parse_expr_integer, parse_expr_true, parse_expr_variable},
                index);
    }

    parser_return_t parse_lvalue(token_vec_t tokens, unsigned int index) {
        return apply_parsers(tokens, {parse_lvalue_argument}, index);
    }

    parser_return_t parse_type(token_vec_t tokens, unsigned int index) {
        return apply_parsers(tokens, {parse_type_bool, parse_type_float, parse_type_int, parse_type_variable}, index);
    }

    /*
    ============================
    ||  AST Node Subclasses:  ||
    ============================
    */
    //  Arguments:
    //  ----------
    parser_return_t parse_argument_variable(token_vec_t tokens, unsigned int index) {
        try {
            return {std::make_shared<ast_node::variable_argument_node>(*tokens[index]), index + 1};
        } catch (const std::runtime_error& err) { throw parser_error_recoverable(); }
    }

    //  Commands:
    //  ---------
    parser_return_t parse_cmd_assert(token_vec_t tokens, unsigned int index) {
        //  1: `"assert"`.
        if (tokens[index]->type != token::token_type::ASSERT) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `<expr>`.
        parser_return_t result = parse_expr(tokens, index);
        std::shared_ptr<ast_node::expr_node> expr = std::reinterpret_pointer_cast<ast_node::expr_node>(
                std::get<0>(result));

        //  Increment and check.
        index = std::get<1>(result);
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  3: `","`.
        if (tokens[index]->type != token::token_type::COMMA)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::COMMA);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  4: `<string>`.
        if (tokens[index]->type != token::token_type::STRING)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::STRING);
        token::string_token string_tok = *std::reinterpret_pointer_cast<token::string_token>(tokens[index]);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type != token::token_type::NEWLINE) throw parser_error_trailing_token(tokens[index]->start);

        //  Return.
        return {std::make_shared<ast_node::assert_cmd_node>(expr, string_tok), index + 1};
    }

    parser_return_t parse_cmd_let(token_vec_t tokens, unsigned int index) {
        //  1: `"let"`.
        if (tokens[index]->type != token::token_type::LET) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `<lvalue>`.
        parser_return_t result = parse_lvalue(tokens, index);
        std::shared_ptr<ast_node::lvalue_node> lvalue = std::reinterpret_pointer_cast<ast_node::lvalue_node>(
                std::get<0>(result));

        //  Increment and check.
        index = std::get<1>(result);
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  3: `"="`.
        if (tokens[index]->type != token::token_type::EQUALS)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::EQUALS);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  4: `<expr>`.
        result = parse_expr(tokens, index);
        std::shared_ptr<ast_node::expr_node> expr = std::reinterpret_pointer_cast<ast_node::expr_node>(
                std::get<0>(result));

        //  Increment and check.
        index = std::get<1>(result);
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type != token::token_type::NEWLINE) throw parser_error_trailing_token(tokens[index]->start);

        return {std::make_shared<ast_node::let_cmd_node>(lvalue, expr), index + 1};
    }

    parser_return_t parse_cmd_print(token_vec_t tokens, unsigned int index) {
        //  1: `"print"`.
        if (tokens[index]->type != token::token_type::PRINT) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `<string>`.
        if (tokens[index]->type != token::token_type::STRING)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::STRING);
        token::string_token string_tok = *std::reinterpret_pointer_cast<token::string_token>(tokens[index]);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type != token::token_type::NEWLINE) throw parser_error_trailing_token(tokens[index]->start);

        return {std::make_shared<ast_node::print_cmd_node>(string_tok), index + 1};
    }

    parser_return_t parse_cmd_read(token_vec_t tokens, unsigned int index) {
        //  1: `"read"`.
        if (tokens[index]->type != token::token_type::READ) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `"image"`.
        if (tokens[index]->type != token::token_type::IMAGE) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  3: `<string>`.
        if (tokens[index]->type != token::token_type::STRING)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::STRING);
        token::string_token string_tok = *std::reinterpret_pointer_cast<token::string_token>(tokens[index]);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  4: `"to"`.
        if (tokens[index]->type != token::token_type::TO)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::EQUALS);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  5: `<argument>`.
        parser_return_t result = parse_argument(tokens, index);
        std::shared_ptr<ast_node::argument_node> argument = std::reinterpret_pointer_cast<ast_node::argument_node>(
                std::get<0>(result));

        //  Increment and check.
        index = std::get<1>(result);
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type != token::token_type::NEWLINE) throw parser_error_trailing_token(tokens[index]->start);

        return {std::make_shared<ast_node::read_cmd_node>(string_tok, argument), index + 1};
    }

    parser_return_t parse_cmd_show(token_vec_t tokens, unsigned int index) {
        //  1: `"show"`.
        if (tokens[index]->type != token::token_type::SHOW) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `<expr>`.
        parser_return_t result = parse_expr(tokens, index);
        std::shared_ptr<ast_node::expr_node> expr = std::reinterpret_pointer_cast<ast_node::expr_node>(
                std::get<0>(result));

        //  Increment and check.
        index = std::get<1>(result);
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type != token::token_type::NEWLINE) throw parser_error_trailing_token(tokens[index]->start);

        return {std::make_shared<ast_node::show_cmd_node>(expr), index + 1};
    }

    parser_return_t parse_cmd_type(token_vec_t tokens, unsigned int index) {
        //  1: `"type"`.
        if (tokens[index]->type != token::token_type::TYPE) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `<variable>`.
        token::token variable = *tokens[index];
        if (tokens[index]->type != token::token_type::VARIABLE)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::VARIABLE);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  3: `"="`.
        if (tokens[index]->type != token::token_type::EQUALS)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::EQUALS);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  4: `<type>`.
        parser_return_t result = parse_type(tokens, index);
        std::shared_ptr<ast_node::type_node> type = std::reinterpret_pointer_cast<ast_node::type_node>(
                std::get<0>(result));

        //  Increment and check.
        index = std::get<1>(result);
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type != token::token_type::NEWLINE) throw parser_error_trailing_token(tokens[index]->start);

        return {std::make_shared<ast_node::type_cmd_node>(variable, type), index + 1};
    }

    parser_return_t parse_cmd_write(token_vec_t tokens, unsigned int index) {
        //  1: `"write"`.
        if (tokens[index]->type != token::token_type::WRITE) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `"image"`.
        if (tokens[index]->type != token::token_type::IMAGE)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::IMAGE);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  3: `<expr>`.
        parser_return_t result = parse_expr(tokens, index);
        std::shared_ptr<ast_node::expr_node> expr = std::reinterpret_pointer_cast<ast_node::expr_node>(
                std::get<0>(result));

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  4: `"to"`.
        if (tokens[index]->type != token::token_type::TO)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::EQUALS);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  5: `<string>`.
        if (tokens[index]->type != token::token_type::STRING)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::STRING);
        token::string_token string_tok = *std::reinterpret_pointer_cast<token::string_token>(tokens[index]);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type != token::token_type::NEWLINE) throw parser_error_trailing_token(tokens[index]->start);

        return {std::make_shared<ast_node::write_cmd_node>(expr, string_tok), index + 1};
    }

    //  Expressions:
    //  ------------
    parser_return_t parse_expr_false(token_vec_t tokens, unsigned int index) {
        //  1: `"false"`.
        if (tokens[index]->type != token::token_type::FALSE) throw parser_error_recoverable();

        return {std::make_shared<ast_node::false_expr_node>(), index + 1};
    }

    parser_return_t parse_expr_float(token_vec_t tokens, unsigned int index) {
        //  1: `<float>`.
        if (tokens[index]->type != token::token_type::FLOATVAL) throw parser_error_recoverable();
        std::shared_ptr<token::float_token> float_val = std::reinterpret_pointer_cast<token::float_token>(
                tokens[index]);

        return {std::make_shared<ast_node::float_expr_node>(*float_val), index + 1};
    }

    parser_return_t parse_expr_integer(token_vec_t tokens, unsigned int index) {
        //  1: `<integer>`.
        if (tokens[index]->type != token::token_type::INTVAL) throw parser_error_recoverable();
        std::shared_ptr<token::int_token> integer = std::reinterpret_pointer_cast<token::int_token>(tokens[index]);

        return {std::make_shared<ast_node::integer_expr_node>(*integer), index + 1};
    }

    parser_return_t parse_expr_true(token_vec_t tokens, unsigned int index) {
        //  1: `"true"`.
        if (tokens[index]->type != token::token_type::TRUE) throw parser_error_recoverable();

        return {std::make_shared<ast_node::true_expr_node>(), index + 1};
    }

    parser_return_t parse_expr_variable(token_vec_t tokens, unsigned int index) {
        //  1: `<variable>`.
        if (tokens[index]->type != token::token_type::VARIABLE) throw parser_error_recoverable();

        return {std::make_shared<ast_node::variable_expr_node>(*tokens[index]), index + 1};
    }

    //  Lvalues:
    //  --------
    parser_return_t parse_lvalue_argument(token_vec_t tokens, unsigned int index) {
        try {
            parser_return_t result = parse_argument(tokens, index);
            return {std::make_shared<ast_node::argument_lvalue_node>(
                            std::reinterpret_pointer_cast<ast_node::argument_node>(std::get<0>(result))),
                    std::get<1>(result)};
        } catch (const parser_error_unrecoverable& err) {
            //  This is recoverable, even if it's not an argument.
            throw parser_error_recoverable();
        }
    }

    //  Types:
    //  ------
    parser_return_t parse_type_bool(token_vec_t tokens, unsigned int index) {
        //  1: `"bool"`.
        if (tokens[index]->type != token::token_type::BOOL) throw parser_error_recoverable();

        return {std::make_shared<ast_node::bool_type_node>(), index + 1};
    }

    parser_return_t parse_type_float(token_vec_t tokens, unsigned int index) {
        //  1: `"float"`.
        if (tokens[index]->type != token::token_type::FLOAT) throw parser_error_recoverable();

        return {std::make_shared<ast_node::float_type_node>(), index + 1};
    }

    parser_return_t parse_type_int(token_vec_t tokens, unsigned int index) {
        //  1: `"int"`.
        if (tokens[index]->type != token::token_type::INT) throw parser_error_recoverable();

        return {std::make_shared<ast_node::int_type_node>(), index + 1};
    }

    parser_return_t parse_type_variable(token_vec_t tokens, unsigned int index) {
        //  1: `<variable>`.
        if (tokens[index]->type != token::token_type::VARIABLE) throw parser_error_recoverable();

        return {std::make_shared<ast_node::variable_type_node>(*tokens[index]), index + 1};
    }
}  //  namespace parser
