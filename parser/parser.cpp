/**
 * @file parser.cpp
 * @package Assignment 3-4.
 * @author Cayden Lund (u1182408)
 * @brief Implements the parser functions.
 * @details See header file for documentation.
 *
 */

#include "parser.hpp"

namespace parser {
    node_list_t parse(token_vec_t tokens) {
        const std::vector<std::shared_ptr<token::token>> vec(tokens.begin() + 1, tokens.end());
        if (!tokens.empty() && tokens[0]->type == token::token_type::NEWLINE) return (parse(vec));

        unsigned int index = 0;
        node_list_t nodes;

        while (index < tokens.size() - 1) {
            parser_return_t result = apply_parsers(tokens, {parse_cmd}, index);
            nodes.push_back(std::get<0>(result));
            index = std::get<1>(result);
        }

        return nodes;
    }

    std::tuple<node_list_t, unsigned int> parse_sequence(const parser_t& parser_function, token_vec_t tokens,
                                                         unsigned int index) {
        node_list_t node_list;

        try {
            parser_return_t result = parser_function(tokens, index);
            node_list.emplace_back(std::get<0>(result));
            index = std::get<1>(result);
            while (tokens[index]->type == token::token_type::COMMA) {
                index++;
                result = parser_function(tokens, index);
                node_list.emplace_back(std::get<0>(result));
                index = std::get<1>(result);
            }
        } catch (const parser_error_unrecoverable& err) {}

        return {node_list, index};
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
        return apply_parsers(tokens, argument_parsers, index);
    }

    parser_return_t parse_binding(token_vec_t tokens, unsigned int index) {
        return apply_parsers(tokens, binding_parsers, index);
    }

    parser_return_t parse_cmd(token_vec_t tokens, unsigned int index) {
        return apply_parsers(tokens, command_parsers, index);
    }

    parser_return_t parse_expr(token_vec_t tokens, unsigned int index) {
        parser_return_t result = apply_parsers(tokens, expression_parsers, index);
        std::shared_ptr<ast_node::expr_node> expr = std::reinterpret_pointer_cast<ast_node::expr_node>(
                std::get<0>(result));
        index = std::get<1>(result);
        while (index < tokens.size()
               && (tokens[index]->type == token::token_type::LSQUARE
                   || tokens[index]->type == token::token_type::LCURLY)) {
            if (tokens[index]->type == token::token_type::LSQUARE) {
                result = parse_expr_array_index(expr, tokens, index);
                expr = std::reinterpret_pointer_cast<ast_node::expr_node>(std::get<0>(result));
                index = std::get<1>(result);
            } else {
                result = parse_expr_tuple_index(expr, tokens, index);
                expr = std::reinterpret_pointer_cast<ast_node::expr_node>(std::get<0>(result));
                index = std::get<1>(result);
            }
        }

        return {expr, index};
    }

    parser_return_t parse_lvalue(token_vec_t tokens, unsigned int index) {
        return apply_parsers(tokens, lvalue_parsers, index);
    }

    parser_return_t parse_statement(token_vec_t tokens, unsigned int index) {
        return apply_parsers(tokens, statement_parsers, index);
    }

    parser_return_t parse_type(token_vec_t tokens, unsigned int index) {
        parser_return_t result = apply_parsers(tokens, type_parsers, index);
        std::shared_ptr<ast_node::type_node> type = std::reinterpret_pointer_cast<ast_node::type_node>(
                std::get<0>(result));
        index = std::get<1>(result);
        while (index < tokens.size() && tokens[index]->type == token::token_type::LSQUARE) {
            result = parse_type_array(type, tokens, index);
            type = std::reinterpret_pointer_cast<ast_node::type_node>(std::get<0>(result));
            index = std::get<1>(result);
        }

        return {type, index};
    }

    /*
    ============================
    ||  AST Node Subclasses:  ||
    ============================
    */
    //  Arguments:
    //  ----------
    parser_return_t parse_argument_array(token_vec_t tokens, unsigned int index) {
        //  1: `<variable>`.
        if (tokens[index]->type != token::token_type::VARIABLE) throw parser_error_recoverable();
        const token::token variable_name = *tokens[index];

        //  Increment and check.
        index++;
        if (index >= tokens.size()) throw parser_error_recoverable();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_recoverable();

        //  2: `"["`.
        if (tokens[index]->type != token::token_type::LSQUARE) throw parser_error_recoverable();

        //  3: `<variable>, ...`.
        //  If we've made it this far, then this must be an array argument.
        //  Any errors from here on are unrecoverable.
        std::vector<token::token> variables;
        index++;
        while (tokens[index]->type == token::token_type::VARIABLE) {
            variables.emplace_back(*tokens[index]);

            //  Increment and check.
            index++;
            if (index >= tokens.size()) throw parser_error_eof();
            if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);
            if (tokens[index]->type != token::token_type::COMMA) break;

            //  Increment and check (again).
            index++;
            if (index >= tokens.size()) throw parser_error_eof();
            if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);
        }

        //  4: `"]"`.
        if (tokens[index]->type != token::token_type::RSQUARE)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::RSQUARE);

        return {std::make_shared<ast_node::array_argument_node>(variable_name, variables), index + 1};
    }

    parser_return_t parse_argument_variable(token_vec_t tokens, unsigned int index) {
        try {
            return {std::make_shared<ast_node::variable_argument_node>(*tokens[index]), index + 1};
        } catch (const std::runtime_error& err) { throw parser_error_recoverable(); }
    }

    //  Bindings:
    //  ---------
    parser_return_t parse_binding_tuple(token_vec_t tokens, unsigned int index) {
        //  1: `"{"`.
        if (tokens[index]->type != token::token_type::LCURLY) throw parser_error_recoverable();

        //  2: Increment and check.
        index++;
        if (index >= tokens.size()) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  3: `<binding>, ...`.
        const std::tuple<node_list_t, unsigned int> result = parse_sequence(parse_binding, tokens, index);
        const node_list_t nodes = std::get<0>(result);
        std::vector<std::shared_ptr<ast_node::binding_node>> bindings;
        for (const node_ptr_t& node : nodes) {
            bindings.emplace_back(std::reinterpret_pointer_cast<ast_node::binding_node>(node));
        }

        //  4: Increment and check.
        index = std::get<1>(result);
        if (index >= tokens.size()) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  5: `"}"`.
        if (tokens[index]->type != token::token_type::RCURLY)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::RCURLY);

        return {std::make_shared<ast_node::tuple_binding_node>(bindings), index + 1};
    }

    parser_return_t parse_binding_variable(token_vec_t tokens, unsigned int index) {
        //  1: `<argument>`.
        parser_return_t result;
        std::shared_ptr<ast_node::argument_node> argument;
        try {
            result = parse_argument(tokens, index);
            argument = std::reinterpret_pointer_cast<ast_node::argument_node>(std::get<0>(result));
        } catch (const parser_error& err) { throw parser_error_recoverable(); }
        const token::token variable = *tokens[index];

        //  Increment and check.
        index = std::get<1>(result);
        if (index >= tokens.size()) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `":"`.
        if (tokens[index]->type != token::token_type::COLON)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::COLON);

        //  Increment and check.
        index++;
        if (index >= tokens.size()) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  3: `<type>`.
        result = parse_type(tokens, index);
        const std::shared_ptr<ast_node::type_node> type = std::reinterpret_pointer_cast<ast_node::type_node>(
                std::get<0>(result));

        return {std::make_shared<ast_node::var_binding_node>(argument, type), std::get<1>(result)};
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

    parser_return_t parse_cmd_fn(token_vec_t tokens, unsigned int index) {
        //  1: `"fn"`.
        if (tokens[index]->type != token::token_type::FN) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `<variable>`.
        if (tokens[index]->type != token::token_type::VARIABLE)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::VARIABLE);
        const token::token name = *tokens[index];

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  3: `"("`.
        if (tokens[index]->type != token::token_type::LPAREN)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::LPAREN);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  4: `<binding>, ...`.
        std::tuple<std::vector<node_ptr_t>, unsigned int> result = parse_sequence(parse_binding, tokens, index);
        std::vector<std::shared_ptr<ast_node::binding_node>> bindings;
        for (const std::shared_ptr<ast_node::ast_node>& node : std::get<0>(result)) {
            bindings.emplace_back(std::reinterpret_pointer_cast<ast_node::binding_node>(node));
        }

        //  Increment and check.
        index = std::get<1>(result);
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  5: `")"`.
        if (tokens[index]->type != token::token_type::RPAREN)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::RPAREN);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  6: `":"`.
        if (tokens[index]->type != token::token_type::COLON)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::COLON);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  7: `<type>`.
        parser_return_t type_result = parse_type(tokens, index);
        const std::shared_ptr<ast_node::type_node> return_type = std::reinterpret_pointer_cast<ast_node::type_node>(
                std::get<0>(type_result));

        //  Increment and check.
        index = std::get<1>(type_result);
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  7: `"{"`.
        if (tokens[index]->type != token::token_type::LCURLY)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::LCURLY);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type != token::token_type::NEWLINE) throw parser_error_trailing_token(tokens[index]->start);
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();

        //  8: `"<stmt>; ..."`.
        std::vector<std::shared_ptr<ast_node::stmt_node>> statements;
        while (index < tokens.size() && tokens[index]->type != token::token_type::RCURLY) {
            parser_return_t statement_result = parse_statement(tokens, index);
            statements.emplace_back(std::reinterpret_pointer_cast<ast_node::stmt_node>(std::get<0>(statement_result)));
            index = std::get<1>(statement_result);
        }

        //  Increment and check.
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  9: `"}"`.
        if (tokens[index]->type != token::token_type::RCURLY)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::RCURLY);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type != token::token_type::NEWLINE) throw parser_error_trailing_token(tokens[index]->start);

        return {std::make_shared<ast_node::fn_cmd_node>(name, bindings, return_type, statements), index + 1};
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

        return {std::make_shared<ast_node::print_cmd_node>(string_tok.value), index + 1};
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

    parser_return_t parse_cmd_time(token_vec_t tokens, unsigned int index) {
        //  1: `"time"`.
        if (tokens[index]->type != token::token_type::TIME) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `<cmd>`.
        const parser_return_t result = parse_cmd(tokens, index);
        std::shared_ptr<ast_node::cmd_node> cmd = std::reinterpret_pointer_cast<ast_node::cmd_node>(
                std::get<0>(result));

        return {std::make_shared<ast_node::time_cmd_node>(cmd), std::get<1>(result)};
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
        index = std::get<1>(result);
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  4: `"to"`.
        if (tokens[index]->type != token::token_type::TO)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::TO);

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
    parser_return_t parse_expr_array_index(const std::shared_ptr<ast_node::expr_node> array, token_vec_t tokens,
                                           unsigned int index) {
        std::vector<std::shared_ptr<ast_node::expr_node>> expressions;
        std::shared_ptr<ast_node::expr_node> new_array = array;

        //  1: `"["`.
        if (tokens[index]->type != token::token_type::LSQUARE) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `<expr>, ...`.
        while (tokens[index]->type != token::token_type::RSQUARE) {
            const parser_return_t result = parse_expr(tokens, index);
            expressions.emplace_back(std::reinterpret_pointer_cast<ast_node::expr_node>(std::get<0>(result)));

            //  Increment and check.
            index = std::get<1>(result);
            if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
            if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

            //  3: `","` or `"]"`.
            if (tokens[index]->type != token::token_type::COMMA && tokens[index]->type != token::token_type::RSQUARE)
                throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::COMMA);

            //  Increment and check.
            if (tokens[index]->type == token::token_type::COMMA) index++;
            if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
            if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);
        }

        new_array = std::make_shared<ast_node::array_index_expr_node>(new_array, expressions);

        return {new_array, index + 1};
    }

    parser_return_t parse_expr_array_literal(token_vec_t tokens, unsigned int index) {
        //  1: `"["`.
        if (tokens[index]->type != token::token_type::LSQUARE) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `<expr>, ...`.
        const std::tuple<node_list_t, unsigned int> result = parse_sequence(parse_expr, tokens, index);
        std::vector<std::shared_ptr<ast_node::expr_node>> expr_list;
        for (const node_ptr_t& node : std::get<0>(result)) {
            expr_list.emplace_back(std::reinterpret_pointer_cast<ast_node::expr_node>(node));
        }

        //  Increment and check.
        index = std::get<1>(result);
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  3: `"]"`.
        if (tokens[index]->type != token::token_type::RSQUARE)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::RSQUARE);

        return {std::make_shared<ast_node::array_literal_expr_node>(expr_list), index + 1};
    }

    parser_return_t parse_expr_array_loop(token_vec_t tokens, unsigned int index) {
        //  1: `"array"`.
        if (tokens[index]->type != token::token_type::ARRAY) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `"["`.
        if (tokens[index]->type != token::token_type::LSQUARE)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::LSQUARE);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  3: `<variable> : <expr>, ...`.
        std::vector<ast_node::array_loop_expr_node::binding_pair_t> bindings;
        while (tokens[index]->type != token::token_type::RSQUARE) {
            //  3.1: `<variable>`.
            if (tokens[index]->type != token::token_type::VARIABLE)
                throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type,
                                                  token::token_type::VARIABLE);
            const token::token variable = *tokens[index];

            //  Increment and check.
            index++;
            if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
            if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

            //  3.2: `":"`.
            if (tokens[index]->type != token::token_type::COLON)
                throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::COLON);

            //  Increment and check.
            index++;
            if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
            if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

            //  3.3: `<expr>`.
            const parser_return_t result = parse_expr(tokens, index);
            const std::shared_ptr<ast_node::expr_node> expression = std::reinterpret_pointer_cast<ast_node::expr_node>(
                    std::get<0>(result));

            //  Add it to the end of the list of bindings.
            bindings.emplace_back(variable, expression);

            //  Increment and check.
            index = std::get<1>(result);
            if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
            if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);
            if (tokens[index]->type != token::token_type::COMMA && tokens[index]->type != token::token_type::RSQUARE)
                throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::COMMA);
            if (tokens[index]->type == token::token_type::COMMA) {
                index++;
                if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
                if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);
                if (tokens[index]->type != token::token_type::VARIABLE)
                    throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type,
                                                      token::token_type::VARIABLE);
            }
        }

        //  4: `"]"`.
        if (tokens[index]->type != token::token_type::RSQUARE)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::RSQUARE);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  5: `<expr>`.
        parser_return_t result = parse_expr(tokens, index);
        const std::shared_ptr<ast_node::expr_node> expression = std::reinterpret_pointer_cast<ast_node::expr_node>(
                std::get<0>(result));

        //  Increment and check.
        index = std::get<1>(result);
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();

        return {std::make_shared<ast_node::array_loop_expr_node>(bindings, expression), index};
    }

    parser_return_t parse_expr_call(token_vec_t tokens, unsigned int index) {
        //  1: `<variable>`.
        if (tokens[index]->type != token::token_type::VARIABLE) throw parser_error_recoverable();
        const token::token name = *tokens[index];

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_recoverable();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_recoverable();

        //  2: `"("`.
        if (tokens[index]->type != token::token_type::LPAREN) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `<expr>, ...`.
        const std::tuple<node_list_t, unsigned int> result = parse_sequence(parse_expr, tokens, index);
        std::vector<std::shared_ptr<ast_node::expr_node>> expr_list;
        for (const node_ptr_t& node : std::get<0>(result)) {
            expr_list.emplace_back(std::reinterpret_pointer_cast<ast_node::expr_node>(node));
        }

        //  Increment and check.
        index = std::get<1>(result);
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  3: `")"`.
        if (tokens[index]->type != token::token_type::RPAREN)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::RSQUARE);

        return {std::make_shared<ast_node::call_expr_node>(name, expr_list), index + 1};
    }

    parser_return_t parse_expr_false(token_vec_t tokens, unsigned int index) {
        //  1: `"false"`.
        if (tokens[index]->type != token::token_type::FALSE) throw parser_error_recoverable();

        return {std::make_shared<ast_node::false_expr_node>(), index + 1};
    }

    parser_return_t parse_expr_float(token_vec_t tokens, unsigned int index) {
        //  1: `<float>`.
        if (tokens[index]->type != token::token_type::FLOATVAL) throw parser_error_recoverable();

        return {std::make_shared<ast_node::float_expr_node>(*tokens[index]), index + 1};
    }

    parser_return_t parse_expr_if(token_vec_t tokens, unsigned int index) {
        //  1: `"if"`.
        if (tokens[index]->type != token::token_type::IF) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `<expr>`.
        parser_return_t result = parse_expr(tokens, index);
        const std::shared_ptr<ast_node::expr_node> conditional_expression
                = std::reinterpret_pointer_cast<ast_node::expr_node>(std::get<0>(result));

        //  Increment and check.
        index = std::get<1>(result);
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  3: `"then"`.
        if (tokens[index]->type != token::token_type::THEN)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::THEN);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  4: `<expr>`.
        result = parse_expr(tokens, index);
        const std::shared_ptr<ast_node::expr_node> affirmative_expression
                = std::reinterpret_pointer_cast<ast_node::expr_node>(std::get<0>(result));

        //  Increment and check.
        index = std::get<1>(result);
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  5: `"else"`.
        if (tokens[index]->type != token::token_type::ELSE)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::ELSE);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  6: `<expr>`.
        result = parse_expr(tokens, index);
        const std::shared_ptr<ast_node::expr_node> negative_expression
                = std::reinterpret_pointer_cast<ast_node::expr_node>(std::get<0>(result));

        //  Increment and check.
        index = std::get<1>(result);
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();

        return {std::make_shared<ast_node::if_expr_node>(conditional_expression, affirmative_expression,
                                                         negative_expression),
                index + 1};
    }

    parser_return_t parse_expr_integer(token_vec_t tokens, unsigned int index) {
        //  1: `<integer>`.
        if (tokens[index]->type != token::token_type::INTVAL) throw parser_error_recoverable();

        return {std::make_shared<ast_node::integer_expr_node>(*tokens[index]), index + 1};
    }

    parser_return_t parse_expr_parentheses(token_vec_t tokens, unsigned int index) {
        //  1: `"("`.
        if (tokens[index]->type != token::token_type::LPAREN) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `<expr>`.
        const parser_return_t result = parse_expr(tokens, index);
        const std::shared_ptr<ast_node::expr_node> expr = std::reinterpret_pointer_cast<ast_node::expr_node>(
                std::get<0>(result));

        //  Increment and check.
        index = std::get<1>(result);
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  3: `")"`.
        if (tokens[index]->type != token::token_type::RPAREN)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::RPAREN);

        return {expr, index + 1};
    }

    parser_return_t parse_expr_sum_loop(token_vec_t tokens, unsigned int index) {
        //  1: `"sum"`.
        if (tokens[index]->type != token::token_type::SUM) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `"["`.
        if (tokens[index]->type != token::token_type::LSQUARE)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::LSQUARE);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  3: `<variable> : <expr>, ...`.
        std::vector<ast_node::sum_loop_expr_node::binding_pair_t> bindings;
        while (tokens[index]->type != token::token_type::RSQUARE) {
            //  3.1: `<variable>`.
            if (tokens[index]->type != token::token_type::VARIABLE)
                throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type,
                                                  token::token_type::VARIABLE);
            const token::token variable = *tokens[index];

            //  Increment and check.
            index++;
            if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
            if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

            //  3.2: `":"`.
            if (tokens[index]->type != token::token_type::COLON)
                throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::COLON);

            //  Increment and check.
            index++;
            if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
            if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

            //  3.3: `<expr>`.
            const parser_return_t result = parse_expr(tokens, index);
            const std::shared_ptr<ast_node::expr_node> expression = std::reinterpret_pointer_cast<ast_node::expr_node>(
                    std::get<0>(result));

            //  Add it to the end of the list of bindings.
            bindings.emplace_back(variable, expression);

            //  Increment and check.
            index = std::get<1>(result);
            if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
            if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);
            if (tokens[index]->type != token::token_type::COMMA && tokens[index]->type != token::token_type::RSQUARE)
                throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::COMMA);
            if (tokens[index]->type == token::token_type::COMMA) {
                index++;
                if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
                if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);
                if (tokens[index]->type != token::token_type::VARIABLE)
                    throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type,
                                                      token::token_type::VARIABLE);
            }
        }

        //  4: `"]"`.
        if (tokens[index]->type != token::token_type::RSQUARE)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::RSQUARE);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  5: `<expr>`.
        parser_return_t result = parse_expr(tokens, index);
        const std::shared_ptr<ast_node::expr_node> expression = std::reinterpret_pointer_cast<ast_node::expr_node>(
                std::get<0>(result));

        //  Increment and check.
        index = std::get<1>(result);
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();

        return {std::make_shared<ast_node::sum_loop_expr_node>(bindings, expression), index};
    }

    parser_return_t parse_expr_true(token_vec_t tokens, unsigned int index) {
        //  1: `"true"`.
        if (tokens[index]->type != token::token_type::TRUE) throw parser_error_recoverable();

        return {std::make_shared<ast_node::true_expr_node>(), index + 1};
    }

    parser_return_t parse_expr_tuple_index(std::shared_ptr<ast_node::expr_node> tuple, token_vec_t tokens,
                                           unsigned int index) {
        //  1: `"{"`.
        if (tokens[index]->type != token::token_type::LCURLY) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->type);

        //  2: `<integer>`
        if (tokens[index]->type != token::token_type::INTVAL)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::INTVAL);
        const std::shared_ptr<ast_node::integer_expr_node> index_expr = std::make_shared<ast_node::integer_expr_node>(
                *tokens[index]);

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->type);

        //  3: `"}"`
        if (tokens[index]->type != token::token_type::RCURLY)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::RCURLY);

        return {std::make_shared<ast_node::tuple_index_expr_node>(tuple, index_expr), index + 1};
    }

    parser_return_t parse_expr_tuple_literal(token_vec_t tokens, unsigned int index) {
        //  1: `"{"`.
        if (tokens[index]->type != token::token_type::LCURLY) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `<expr>, ...`.
        const std::tuple<node_list_t, unsigned int> result = parse_sequence(parse_expr, tokens, index);
        std::vector<std::shared_ptr<ast_node::expr_node>> expr_list;
        for (const node_ptr_t& node : std::get<0>(result)) {
            expr_list.emplace_back(std::reinterpret_pointer_cast<ast_node::expr_node>(node));
        }

        //  Increment and check.
        index = std::get<1>(result);
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  3: `"}"`.
        if (tokens[index]->type != token::token_type::RCURLY)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::RCURLY);

        return {std::make_shared<ast_node::tuple_literal_expr_node>(expr_list), index + 1};
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

    parser_return_t parse_lvalue_tuple(token_vec_t tokens, unsigned int index) {
        //  1: `"{"`.
        if (tokens[index]->type != token::token_type::LCURLY) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `<lvalue>, ...`.
        const std::tuple<node_list_t, unsigned int> result = parse_sequence(parse_lvalue, tokens, index);
        std::vector<std::shared_ptr<ast_node::lvalue_node>> lvalue_list;
        for (const node_ptr_t& node : std::get<0>(result)) {
            lvalue_list.emplace_back(std::reinterpret_pointer_cast<ast_node::lvalue_node>(node));
        }

        //  Increment and check.
        index = std::get<1>(result);
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  3: `"}"`.
        if (tokens[index]->type != token::token_type::RCURLY)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::RCURLY);

        return {std::make_shared<ast_node::tuple_lvalue_node>(lvalue_list), index + 1};
    }

    //  Statements:
    //  -----------
    parser_return_t parse_statement_assert(token_vec_t tokens, unsigned int index) {
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
        return {std::make_shared<ast_node::assert_stmt_node>(expr, string_tok), index + 1};
    }

    parser_return_t parse_statement_let(token_vec_t tokens, unsigned int index) {
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

        return {std::make_shared<ast_node::let_stmt_node>(lvalue, expr), index + 1};
    }

    parser_return_t parse_statement_return(token_vec_t tokens, unsigned int index) {
        //  1: `"return"`.
        if (tokens[index]->type != token::token_type::RETURN) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `<expr>`.
        const parser_return_t result = parse_expr(tokens, index);
        std::shared_ptr<ast_node::expr_node> expr = std::reinterpret_pointer_cast<ast_node::expr_node>(
                std::get<0>(result));

        //  Increment and check.
        index = std::get<1>(result);
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type != token::token_type::NEWLINE) throw parser_error_trailing_token(tokens[index]->start);

        return {std::make_shared<ast_node::return_stmt_node>(expr), index + 1};
    }

    //  Types:
    //  ------
    parser_return_t parse_type_array(const std::shared_ptr<ast_node::type_node> type, token_vec_t tokens,
                                     unsigned int index) {
        //  1: `"["`.
        if (tokens[index]->type != token::token_type::LSQUARE) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `, ...`.
        unsigned long num_commas = 0;
        while (tokens[index++]->type == token::token_type::COMMA) num_commas++;
        index--;

        //  Increment and check.
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  3: `"]"`.
        if (tokens[index]->type != token::token_type::RSQUARE)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::RSQUARE);

        std::shared_ptr<ast_node::array_type_node> node = std::make_shared<ast_node::array_type_node>(type,
                                                                                                      num_commas + 1);

        return {node, index + 1};
    }

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

    parser_return_t parse_type_tuple(token_vec_t tokens, unsigned int index) {
        //  1: `"{"`.
        if (tokens[index]->type != token::token_type::LCURLY) throw parser_error_recoverable();

        //  Increment and check.
        index++;
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  2: `<type>, ...`.
        const std::tuple<node_list_t, unsigned int> result = parse_sequence(parse_type, tokens, index);
        std::vector<std::shared_ptr<ast_node::type_node>> type_list;
        for (const node_ptr_t& node : std::get<0>(result)) {
            type_list.emplace_back(std::reinterpret_pointer_cast<ast_node::type_node>(node));
        }

        //  Increment and check.
        index = std::get<1>(result);
        if (index >= (unsigned int)(tokens.size())) throw parser_error_eof();
        if (tokens[index]->type == token::token_type::NEWLINE) throw parser_error_newline(tokens[index]->start);

        //  3: `"}"`.
        if (tokens[index]->type != token::token_type::RCURLY)
            throw parser_error_token_mismatch(tokens[index]->start, tokens[index]->type, token::token_type::RCURLY);

        return {std::make_shared<ast_node::tuple_type_node>(type_list), index + 1};
    }

    parser_return_t parse_type_variable(token_vec_t tokens, unsigned int index) {
        //  1: `<variable>`.
        if (tokens[index]->type != token::token_type::VARIABLE) throw parser_error_recoverable();

        return {std::make_shared<ast_node::variable_type_node>(*tokens[index]), index + 1};
    }
}  //  namespace parser
