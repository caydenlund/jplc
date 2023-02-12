/**
 * @file parser.tests.cpp
 * @package Assignments 3-5
 * @author Cayden Lund (u1182408)
 * @brief Contains tests for the parser.
 *
 */

#include <sstream>

#include "lexer/lexer.hpp"
#include "parser.hpp"
#include "tests/tests.hpp"

namespace tests::parser_tests {
    /**
     * @brief Compares two vectors of AST nodes.
     * @details This function is explicitly inline.
     *
     * @param actual The actual vector of nodes.
     * @param expected The expected vector of nodes.
     * @return The empty string if correct, or an error message.
     */
    inline std::string nodes_cmp(const parser::node_ptr_t& actual, const parser::node_ptr_t& expected) {
        const std::string actual_s_expr = actual->s_expression();
        const std::string expected_s_expr = expected->s_expression();
        if (actual_s_expr != expected_s_expr)
            return "S-expression was incorrect (" + actual_s_expr + " instead of " + expected_s_expr + ")";

        return "";
    }

    /**
     * @brief Ensures that the parser can correctly parse arguments.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string parse_arguments() {
        //  `array_argument_node`:
        //  ----------------------
        const token::token main_var {0, "xyz", token::token_type::VARIABLE};
        const std::vector<token::token> vars {{0, "a", token::token_type::VARIABLE},
                                              {0, "b", token::token_type::VARIABLE},
                                              {0, "c", token::token_type::VARIABLE}};
        const std::shared_ptr<ast_node::array_argument_node> array_argument
                = std::make_shared<ast_node::array_argument_node>(main_var, vars);

        lexer::token_list_t tokens = lexer::lex_all("xyz[a, b, c]");
        parser::node_ptr_t node = std::get<0>(parser::parse_argument(tokens, 0));

        std::string result = nodes_cmp(node, array_argument);
        if (result != "") return result;

        //  `variable_argument_node`:
        //  -------------------------
        const token::token variable {0, "abc", token::token_type::VARIABLE};
        const std::shared_ptr<ast_node::variable_argument_node> variable_argument
                = std::make_shared<ast_node::variable_argument_node>(variable);

        tokens = lexer::lex_all("abc");
        node = std::get<0>(parser::parse_argument(tokens, 0));

        return nodes_cmp(node, variable_argument);
    }

    /**
     * @brief Ensures that the parser can correctly parse bindings.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string parse_bindings() {
        //  `var_binding_node`:
        //  -------------------
        const token::token variable_xyz {0, "xyz", token::token_type::VARIABLE};
        const std::shared_ptr<ast_node::argument_node> argument_xyz
                = std::make_shared<ast_node::variable_argument_node>(variable_xyz);
        const std::shared_ptr<ast_node::type_node> int_type = std::make_shared<ast_node::int_type_node>();
        const std::shared_ptr<ast_node::var_binding_node> var_binding = std::make_shared<ast_node::var_binding_node>(
                argument_xyz, int_type);

        lexer::token_list_t tokens = lexer::lex_all("xyz : int");
        parser::node_ptr_t node = std::get<0>(parser::parse_binding(tokens, 0));

        std::string result = nodes_cmp(node, var_binding);
        if (result != "") return result;

        //  `tuple_binding_node`:
        //  ---------------------
        const token::token variable_abc {0, "abc", token::token_type::VARIABLE};
        const std::shared_ptr<ast_node::argument_node> argument_abc
                = std::make_shared<ast_node::variable_argument_node>(variable_abc);
        const std::shared_ptr<ast_node::binding_node> binding_abc = std::make_shared<ast_node::var_binding_node>(
                argument_abc, int_type);
        const std::shared_ptr<ast_node::type_node> float_type = std::make_shared<ast_node::float_type_node>();
        const std::shared_ptr<ast_node::binding_node> binding_xyz = std::make_shared<ast_node::var_binding_node>(
                argument_xyz, float_type);
        const std::vector<std::shared_ptr<ast_node::binding_node>> bindings {binding_abc, binding_xyz};
        const std::shared_ptr<ast_node::tuple_binding_node> tuple_binding
                = std::make_shared<ast_node::tuple_binding_node>(bindings);

        tokens = lexer::lex_all("{abc: int, xyz: float}");
        node = std::get<0>(parser::parse_binding(tokens, 0));

        return nodes_cmp(node, tuple_binding);
    }

    /**
     * @brief Ensures that the parser can correctly parse commands.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string parse_commands() {
        //  `assert_cmd_node`:
        //  ------------------
        const token::token variable_xyz {0, "xyz", token::token_type::VARIABLE};
        const std::shared_ptr<ast_node::expr_node> expr_xyz = std::make_shared<ast_node::variable_expr_node>(
                variable_xyz);
        const token::string_token string_abc {{0, "\"abc\"", token::token_type::STRING}, "abc"};
        const std::shared_ptr<ast_node::assert_cmd_node> assert_cmd = std::make_shared<ast_node::assert_cmd_node>(
                expr_xyz, string_abc);

        lexer::token_list_t tokens = lexer::lex_all("assert xyz, \"abc\"\n");
        parser::node_ptr_t node = std::get<0>(parser::parse_cmd(tokens, 0));

        std::string result = nodes_cmp(node, assert_cmd);
        if (result != "") return result;

        //  `fn_cmd_node`:
        //  --------------
        const token::token variable_abc {0, "abc", token::token_type::VARIABLE};
        const std::shared_ptr<ast_node::argument_node> argument_abc
                = std::make_shared<ast_node::variable_argument_node>(variable_abc);
        const std::shared_ptr<ast_node::type_node> type_int = std::make_shared<ast_node::int_type_node>();
        const std::shared_ptr<ast_node::binding_node> binding_abc = std::make_shared<ast_node::var_binding_node>(
                argument_abc, type_int);
        const std::vector<std::shared_ptr<ast_node::binding_node>> bindings {binding_abc};
        const std::shared_ptr<ast_node::type_node> type_bool = std::make_shared<ast_node::bool_type_node>();
        const std::shared_ptr<ast_node::expr_node> expr_true = std::make_shared<ast_node::true_expr_node>();
        const std::shared_ptr<ast_node::stmt_node> stmt_return = std::make_shared<ast_node::return_stmt_node>(
                expr_true);
        const std::vector<std::shared_ptr<ast_node::stmt_node>> statements {stmt_return};
        const std::shared_ptr<ast_node::fn_cmd_node> fn_cmd = std::make_shared<ast_node::fn_cmd_node>(
                variable_xyz, bindings, type_bool, statements);

        std::stringstream fn_input;
        fn_input << "fn xyz(abc : int) : bool {\n"
                 << "    return true\n"
                 << "}\n";
        tokens = lexer::lex_all(fn_input.str());
        node = std::get<0>(parser::parse_cmd(tokens, 0));

        result = nodes_cmp(node, fn_cmd);
        if (result != "") return result;

        //  `let_cmd_node`:
        //  ---------------
        const std::shared_ptr<ast_node::argument_node> argument_xyz
                = std::make_shared<ast_node::variable_argument_node>(variable_xyz);
        const std::shared_ptr<ast_node::lvalue_node> lvalue_xyz = std::make_shared<ast_node::argument_lvalue_node>(
                argument_xyz);
        const std::shared_ptr<ast_node::let_cmd_node> let_cmd = std::make_shared<ast_node::let_cmd_node>(lvalue_xyz,
                                                                                                         expr_true);
        tokens = lexer::lex_all("let xyz = true\n");
        node = std::get<0>(parser::parse_cmd(tokens, 0));

        result = nodes_cmp(node, let_cmd);
        if (result != "") return result;

        //  `print_cmd_node`:
        //  -----------------
        const std::shared_ptr<ast_node::print_cmd_node> print_cmd = std::make_shared<ast_node::print_cmd_node>("abc");

        tokens = lexer::lex_all("print \"abc\"\n");
        node = std::get<0>(parser::parse_cmd(tokens, 0));

        result = nodes_cmp(node, print_cmd);
        if (result != "") return result;

        //  `read_cmd_node`:
        //  ----------------
        const std::shared_ptr<ast_node::read_cmd_node> read_cmd = std::make_shared<ast_node::read_cmd_node>(
                string_abc, argument_xyz);

        tokens = lexer::lex_all("read image \"abc\" to xyz\n");
        node = std::get<0>(parser::parse_cmd(tokens, 0));

        result = nodes_cmp(node, read_cmd);
        if (result != "") return result;

        //  `show_cmd_node`:
        //  ----------------
        const std::shared_ptr<ast_node::show_cmd_node> show_cmd = std::make_shared<ast_node::show_cmd_node>(expr_xyz);

        tokens = lexer::lex_all("show xyz\n");
        node = std::get<0>(parser::parse_cmd(tokens, 0));

        result = nodes_cmp(node, show_cmd);
        if (result != "") return result;

        //  `time_cmd_node`:
        //  ----------------
        const std::shared_ptr<ast_node::time_cmd_node> time_cmd = std::make_shared<ast_node::time_cmd_node>(show_cmd);

        tokens = lexer::lex_all("time show xyz\n");
        node = std::get<0>(parser::parse_cmd(tokens, 0));

        result = nodes_cmp(node, time_cmd);
        if (result != "") return result;

        //  `type_cmd_node`:
        //  ----------------
        const std::shared_ptr<ast_node::type_cmd_node> type_cmd = std::make_shared<ast_node::type_cmd_node>(
                variable_xyz, type_int);

        tokens = lexer::lex_all("type xyz = int\n");
        node = std::get<0>(parser::parse_cmd(tokens, 0));

        result = nodes_cmp(node, type_cmd);
        if (result != "") return result;

        //  `write_cmd_node`:
        //  -----------------
        const std::shared_ptr<ast_node::write_cmd_node> write_cmd = std::make_shared<ast_node::write_cmd_node>(
                expr_xyz, string_abc);

        tokens = lexer::lex_all("write image xyz to \"abc\"\n");
        node = std::get<0>(parser::parse_cmd(tokens, 0));

        return nodes_cmp(node, write_cmd);
    }

    /**
     * @brief Ensures that the parser can correctly parse expressions.
     * @details Does not include operators, except for the indexing operator
     *     on arrays and tuples.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string parse_expressions() {
        //  `array_loop_expr_node`:
        //  -----------------------
        const token::token variable_a {0, "a", token::token_type::VARIABLE};
        const token::token variable_b {0, "b", token::token_type::VARIABLE};
        const token::token variable_c {0, "c", token::token_type::VARIABLE};
        const std::shared_ptr<ast_node::expr_node> var_expr_a = std::make_shared<ast_node::variable_expr_node>(
                variable_a);
        const std::shared_ptr<ast_node::expr_node> var_expr_b = std::make_shared<ast_node::variable_expr_node>(
                variable_b);
        const std::shared_ptr<ast_node::expr_node> var_expr_c = std::make_shared<ast_node::variable_expr_node>(
                variable_c);
        const token::token int_1 {0, "1", token::token_type::INTVAL};
        const token::token int_2 {0, "2", token::token_type::INTVAL};
        const token::token int_3 {0, "3", token::token_type::INTVAL};
        const token::token int_12 {0, "12", token::token_type::INTVAL};
        const std::shared_ptr<ast_node::expr_node> expr_1 = std::make_shared<ast_node::integer_expr_node>(int_1);
        const std::shared_ptr<ast_node::expr_node> expr_2 = std::make_shared<ast_node::integer_expr_node>(int_2);
        const std::shared_ptr<ast_node::expr_node> expr_3 = std::make_shared<ast_node::integer_expr_node>(int_3);
        const std::shared_ptr<ast_node::expr_node> expr_12 = std::make_shared<ast_node::integer_expr_node>(int_12);
        const std::vector<ast_node::array_loop_expr_node::binding_pair_t> binding_pairs = {{variable_a, expr_1},
                                                                                           {variable_b, expr_2},
                                                                                           {variable_c, expr_3}};
        const std::shared_ptr<ast_node::array_loop_expr_node> array_loop_expr
                = std::make_shared<ast_node::array_loop_expr_node>(binding_pairs, expr_12);

        lexer::token_list_t tokens = lexer::lex_all("array[a: 1, b: 2, c: 3] 12");
        parser::node_ptr_t node = std::get<0>(parser::parse_expr(tokens, 0));

        std::string result = nodes_cmp(node, array_loop_expr);
        if (result != "") return result;

        //  `array_index_expr_node`:
        //  ------------------------
        const token::token variable_xyz {0, "xyz", token::token_type::VARIABLE};
        const token::token int_4 {0, "4", token::token_type::INTVAL};
        const token::token int_5 {0, "5", token::token_type::INTVAL};
        const std::shared_ptr<ast_node::expr_node> expr_xyz = std::make_shared<ast_node::variable_expr_node>(
                variable_xyz);
        const std::shared_ptr<ast_node::expr_node> expr_4 = std::make_shared<ast_node::integer_expr_node>(int_4);
        const std::shared_ptr<ast_node::expr_node> expr_5 = std::make_shared<ast_node::integer_expr_node>(int_5);
        const std::vector<std::shared_ptr<ast_node::expr_node>> expr_set_inner {expr_1, expr_2};
        const std::vector<std::shared_ptr<ast_node::expr_node>> expr_set_outer {expr_3, expr_4, expr_5};
        const std::shared_ptr<ast_node::array_index_expr_node> array_index_expr_inner
                = std::make_shared<ast_node::array_index_expr_node>(expr_xyz, expr_set_inner);
        const std::shared_ptr<ast_node::array_index_expr_node> array_index_expr_outer
                = std::make_shared<ast_node::array_index_expr_node>(array_index_expr_inner, expr_set_outer);

        tokens = lexer::lex_all("xyz[1, 2][3, 4, 5]");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        result = nodes_cmp(node, array_index_expr_outer);
        if (result != "") return result;

        //  `array_literal_expr_node`:
        //  --------------------------
        const std::shared_ptr<ast_node::array_literal_expr_node> array_literal_expr
                = std::make_shared<ast_node::array_literal_expr_node>(expr_set_inner);

        tokens = lexer::lex_all("[1, 2]");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        result = nodes_cmp(node, array_literal_expr);
        if (result != "") return result;

        //  `call_expr_node`:
        //  -----------------
        const token::token variable_blur = {0, "blur", token::token_type::VARIABLE};
        const std::shared_ptr<ast_node::expr_node> expr_true = std::make_shared<ast_node::true_expr_node>();
        const std::vector<std::shared_ptr<ast_node::expr_node>> args {expr_xyz, expr_true};
        const std::shared_ptr<ast_node::call_expr_node> call_expr = std::make_shared<ast_node::call_expr_node>(
                variable_blur, args);

        tokens = lexer::lex_all("blur(xyz, true)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        result = nodes_cmp(node, call_expr);
        if (result != "") return result;

        //  `false_expr_node`:
        //  ------------------
        const std::shared_ptr<ast_node::false_expr_node> false_expr = std::make_shared<ast_node::false_expr_node>();

        tokens = lexer::lex_all("false");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        result = nodes_cmp(node, false_expr);
        if (result != "") return result;

        //  `float_expr_node`:
        //  ------------------
        const std::shared_ptr<ast_node::float_expr_node> float_expr = std::make_shared<ast_node::float_expr_node>(
                token::token {0, "1.", token::token_type::FLOATVAL});

        tokens = lexer::lex_all("1.");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        result = nodes_cmp(node, float_expr);
        if (result != "") return result;

        //  `if_expr_node`:
        //  ---------------
        const std::shared_ptr<ast_node::if_expr_node> if_expr = std::make_shared<ast_node::if_expr_node>(
                var_expr_a, var_expr_b, var_expr_c);

        tokens = lexer::lex_all("if a then b else c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        result = nodes_cmp(node, if_expr);
        if (result != "") return result;

        //  `integer_expr_node`:
        //  --------------------
        const std::shared_ptr<ast_node::integer_expr_node> integer_expr = std::make_shared<ast_node::integer_expr_node>(
                token::token {0, "1", token::token_type::INTVAL});

        tokens = lexer::lex_all("1");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        result = nodes_cmp(node, integer_expr);
        if (result != "") return result;

        //  `sum_loop_expr_node`:
        //  -----------------------
        const std::shared_ptr<ast_node::sum_loop_expr_node> sum_loop_expr
                = std::make_shared<ast_node::sum_loop_expr_node>(binding_pairs, expr_12);

        tokens = lexer::lex_all("sum[a: 1, b: 2, c: 3] 12");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        result = nodes_cmp(node, sum_loop_expr);
        if (result != "") return result;

        //  `true_expr_node`:
        //  -----------------
        const std::shared_ptr<ast_node::true_expr_node> true_expr = std::make_shared<ast_node::true_expr_node>();

        tokens = lexer::lex_all("true");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        result = nodes_cmp(node, true_expr);
        if (result != "") return result;

        //  `tuple_index_expr_node`:
        //  ------------------------
        const token::token index {0, "0", token::token_type::INTVAL};
        const std::shared_ptr<ast_node::integer_expr_node> index_expr = std::make_shared<ast_node::integer_expr_node>(
                index);
        const std::shared_ptr<ast_node::tuple_index_expr_node> tuple_index_expr
                = std::make_shared<ast_node::tuple_index_expr_node>(expr_xyz, index_expr);

        tokens = lexer::lex_all("xyz{0}");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        result = nodes_cmp(node, tuple_index_expr);
        if (result != "") return result;

        //  `tuple_literal_expr_node`:
        //  --------------------------
        const std::shared_ptr<ast_node::tuple_literal_expr_node> tuple_literal_expr
                = std::make_shared<ast_node::tuple_literal_expr_node>(expr_set_inner);

        tokens = lexer::lex_all("{1, 2}");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        result = nodes_cmp(node, tuple_literal_expr);
        if (result != "") return result;

        //  `variable_expr_node`:
        //  ---------------------
        const std::shared_ptr<ast_node::variable_expr_node> variable_expr
                = std::make_shared<ast_node::variable_expr_node>(variable_xyz);

        tokens = lexer::lex_all("xyz");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        return nodes_cmp(node, variable_expr);
    }

    /**
     * @brief Ensures that the parser can correctly parse operators.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string parse_operators() {
        lexer::token_list_t tokens;
        parser::node_ptr_t node;
        parser::node_ptr_t expected_node;
        std::string result;

        const token::token variable_a {0, "a", token::token_type::VARIABLE};
        const token::token variable_b {0, "b", token::token_type::VARIABLE};
        const token::token variable_c {0, "c", token::token_type::VARIABLE};
        const token::token variable_d {0, "d", token::token_type::VARIABLE};
        const token::token variable_e {0, "e", token::token_type::VARIABLE};
        const token::token variable_abc {0, "abc", token::token_type::VARIABLE};
        const token::token variable_xyz {0, "xyz", token::token_type::VARIABLE};
        const std::shared_ptr<ast_node::variable_expr_node> var_expr_a = std::make_shared<ast_node::variable_expr_node>(
                variable_a);
        const std::shared_ptr<ast_node::variable_expr_node> var_expr_b = std::make_shared<ast_node::variable_expr_node>(
                variable_b);
        const std::shared_ptr<ast_node::variable_expr_node> var_expr_c = std::make_shared<ast_node::variable_expr_node>(
                variable_c);
        const std::shared_ptr<ast_node::variable_expr_node> var_expr_d = std::make_shared<ast_node::variable_expr_node>(
                variable_d);
        const std::shared_ptr<ast_node::variable_expr_node> var_expr_e = std::make_shared<ast_node::variable_expr_node>(
                variable_e);
        const std::shared_ptr<ast_node::variable_expr_node> var_expr_abc
                = std::make_shared<ast_node::variable_expr_node>(variable_abc);
        const std::shared_ptr<ast_node::variable_expr_node> var_expr_xyz
                = std::make_shared<ast_node::variable_expr_node>(variable_xyz);
        const token::token operator_inverse {0, "!", token::token_type::OP};
        const token::token operator_negate {0, "-", token::token_type::OP};
        const token::token operator_times {0, "*", token::token_type::OP};
        const token::token operator_divide {0, "/", token::token_type::OP};
        const token::token operator_mod {0, "%", token::token_type::OP};
        const token::token operator_plus {0, "+", token::token_type::OP};
        const token::token operator_minus {0, "-", token::token_type::OP};
        const token::token operator_lt {0, "<", token::token_type::OP};
        const token::token operator_gt {0, ">", token::token_type::OP};
        const token::token operator_leq {0, "<=", token::token_type::OP};
        const token::token operator_geq {0, ">=", token::token_type::OP};
        const token::token operator_eq {0, "==", token::token_type::OP};
        const token::token operator_neq {0, "!=", token::token_type::OP};
        const token::token operator_and {0, "&&", token::token_type::OP};
        const token::token operator_or {0, "||", token::token_type::OP};
        const std::shared_ptr<ast_node::integer_expr_node> int_expr_0 = std::make_shared<ast_node::integer_expr_node>(
                token::token {0, "0", token::token_type::INTVAL});

        std::vector<std::shared_ptr<ast_node::expr_node>> index_args;
        std::shared_ptr<ast_node::array_index_expr_node> inner_array_index_expr;
        std::shared_ptr<ast_node::tuple_index_expr_node> inner_tuple_index_expr;
        std::shared_ptr<ast_node::expr_node> inner_expr;
        std::vector<ast_node::array_loop_expr_node::binding_pair_t> array_bindings;
        std::vector<ast_node::sum_loop_expr_node::binding_pair_t> sum_bindings;
        std::shared_ptr<ast_node::expr_node> if_condition;
        std::shared_ptr<ast_node::expr_node> if_then;
        std::shared_ptr<ast_node::expr_node> if_else;
        std::shared_ptr<ast_node::expr_node> left_expr;
        std::shared_ptr<ast_node::expr_node> right_expr;

        //  1: Indexing:
        //  ------------
        tokens = lexer::lex_all("xyz[]");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        index_args = {};
        expected_node = std::make_shared<ast_node::array_index_expr_node>(var_expr_xyz, index_args);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("xyz[a, b, c]");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        index_args = {var_expr_a, var_expr_b, var_expr_c};
        expected_node = std::make_shared<ast_node::array_index_expr_node>(var_expr_xyz, index_args);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("xyz{0}");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        expected_node = std::make_shared<ast_node::tuple_index_expr_node>(var_expr_xyz, int_expr_0);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("xyz[a, b, c][d, e]");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        index_args = {var_expr_a, var_expr_b, var_expr_c};
        inner_array_index_expr = std::make_shared<ast_node::array_index_expr_node>(var_expr_xyz, index_args);
        index_args = {var_expr_d, var_expr_e};
        expected_node = std::make_shared<ast_node::array_index_expr_node>(inner_array_index_expr, index_args);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("xyz[a, b, c]{0}");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        index_args = {var_expr_a, var_expr_b, var_expr_c};
        inner_array_index_expr = std::make_shared<ast_node::array_index_expr_node>(var_expr_xyz, index_args);
        expected_node = std::make_shared<ast_node::tuple_index_expr_node>(inner_array_index_expr, int_expr_0);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("xyz{0}[a, b, c]");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_tuple_index_expr = std::make_shared<ast_node::tuple_index_expr_node>(var_expr_xyz, int_expr_0);
        index_args = {var_expr_a, var_expr_b, var_expr_c};
        expected_node = std::make_shared<ast_node::array_index_expr_node>(inner_tuple_index_expr, index_args);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("xyz{0}{0}");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_tuple_index_expr = std::make_shared<ast_node::tuple_index_expr_node>(var_expr_xyz, int_expr_0);
        expected_node = std::make_shared<ast_node::tuple_index_expr_node>(inner_tuple_index_expr, int_expr_0);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        //  2: Unary inverse `!` and negation `-`:
        //  --------------------------------------
        tokens = lexer::lex_all("!xyz");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        expected_node = std::make_shared<ast_node::unop_expr_node>(operator_inverse, var_expr_xyz);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("-xyz");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        expected_node = std::make_shared<ast_node::unop_expr_node>(operator_negate, var_expr_xyz);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("!-xyz");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_negate, var_expr_xyz);
        expected_node = std::make_shared<ast_node::unop_expr_node>(operator_inverse, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("-!xyz");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_inverse, var_expr_xyz);
        expected_node = std::make_shared<ast_node::unop_expr_node>(operator_negate, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("!xyz[a]");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        index_args = {var_expr_a};
        inner_array_index_expr = std::make_shared<ast_node::array_index_expr_node>(var_expr_xyz, index_args);
        expected_node = std::make_shared<ast_node::unop_expr_node>(operator_inverse, inner_array_index_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("(!xyz)[a]");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_negate, var_expr_xyz);
        index_args = {var_expr_a};
        expected_node = std::make_shared<ast_node::array_index_expr_node>(inner_expr, index_args);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("-xyz[a]");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        index_args = {var_expr_a};
        inner_array_index_expr = std::make_shared<ast_node::array_index_expr_node>(var_expr_xyz, index_args);
        expected_node = std::make_shared<ast_node::unop_expr_node>(operator_negate, inner_array_index_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("xyz[!a]");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_inverse, var_expr_xyz);
        index_args = {inner_expr};
        expected_node = std::make_shared<ast_node::array_index_expr_node>(var_expr_xyz, index_args);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("xyz[-a]");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_negate, var_expr_xyz);
        index_args = {inner_expr};
        expected_node = std::make_shared<ast_node::array_index_expr_node>(var_expr_xyz, index_args);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        //  3: Multiplicative `*`, `/`, and `%`:
        //  ------------------------------------
        tokens = lexer::lex_all("a * b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_times, var_expr_a, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a / b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_divide, var_expr_a, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a % b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_mod, var_expr_a, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a * b * c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_times, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_times, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a * b / c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_times, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_divide, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a * b % c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_times, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_mod, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a / b * c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_divide, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_times, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a / b / c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_divide, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_divide, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a / b % c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_divide, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_mod, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a % b * c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_mod, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_times, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a % b / c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_divide, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_mod, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a % b % c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_mod, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_mod, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("xyz[a] * b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        index_args = {var_expr_a};
        inner_array_index_expr = std::make_shared<ast_node::array_index_expr_node>(var_expr_xyz, index_args);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_times, inner_array_index_expr, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a * xyz[b]");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        index_args = {var_expr_b};
        inner_array_index_expr = std::make_shared<ast_node::array_index_expr_node>(var_expr_xyz, index_args);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_times, var_expr_a, inner_array_index_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("xyz[a * b]");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_times, var_expr_a, var_expr_b);
        index_args = {inner_expr};
        expected_node = std::make_shared<ast_node::array_index_expr_node>(var_expr_xyz, index_args);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("!a * b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_inverse, var_expr_a);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_times, inner_expr, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a * !b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_inverse, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_times, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("!(a * b)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_times, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::unop_expr_node>(operator_inverse, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("-a * b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_negate, var_expr_a);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_times, inner_expr, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a * -b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_negate, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_times, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("-(a * b)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_times, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::unop_expr_node>(operator_negate, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        //  4: Additive `+` and `-`:
        //  ------------------------
        tokens = lexer::lex_all("a + b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_a, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a - b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_minus, var_expr_a, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a + b + c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_plus, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a + (b + c)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a + b - c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_minus, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a + (b - c)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_minus, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a - b + c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_minus, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_plus, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a - (b + c)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_minus, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a - b - c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_minus, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_minus, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a - (b - c)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_minus, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_minus, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("xyz[a] + b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        index_args = {var_expr_a};
        inner_expr = std::make_shared<ast_node::array_index_expr_node>(var_expr_xyz, index_args);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_plus, inner_expr, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a + xyz[b]");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        index_args = {var_expr_b};
        inner_expr = std::make_shared<ast_node::array_index_expr_node>(var_expr_xyz, index_args);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("xyz[a + b]");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_a, var_expr_b);
        index_args = {inner_expr};
        expected_node = std::make_shared<ast_node::array_index_expr_node>(var_expr_xyz, index_args);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("!a + b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_inverse, var_expr_a);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_plus, inner_expr, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a + !b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_inverse, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("!(a + b)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::unop_expr_node>(operator_inverse, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("-a + b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_negate, var_expr_a);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_plus, inner_expr, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a + -b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_negate, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("-(a + b)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::unop_expr_node>(operator_negate, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a * b + c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_times, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_plus, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a * (b + c)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_times, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a + b * c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_times, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("(a + b) * c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_times, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;


        //  5: Comparative `<`, `>` `<=`, `>=`, `==`, and `!=`:
        //  ---------------------------------------------------
        tokens = lexer::lex_all("a < b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a > b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_gt, var_expr_a, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a <= b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_leq, var_expr_a, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a >= b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_geq, var_expr_a, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a == b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_eq, var_expr_a, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a != b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_neq, var_expr_a, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a < b < c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_lt, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a < (b < c)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a < b > c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_gt, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a < (b > c)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_gt, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a < b <= c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_leq, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a < (b <= c)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_leq, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a < b >= c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_geq, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a < (b >= c)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_geq, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a < b == c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_eq, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a < (b == c)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_eq, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a < b != c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_neq, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a < (b != c)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_neq, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("xyz[a] < b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        index_args = {var_expr_a};
        inner_array_index_expr = std::make_shared<ast_node::array_index_expr_node>(var_expr_xyz, index_args);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_lt, inner_expr, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a < xyz[b]");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        index_args = {var_expr_b};
        inner_array_index_expr = std::make_shared<ast_node::array_index_expr_node>(var_expr_xyz, index_args);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("xyz[a < b]");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, var_expr_b);
        index_args = {inner_expr};
        expected_node = std::make_shared<ast_node::array_index_expr_node>(var_expr_xyz, index_args);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("!a < b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_inverse, var_expr_a);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_lt, inner_expr, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a < !b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_inverse, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("!(a < b)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::unop_expr_node>(operator_inverse, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("-a < b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_negate, var_expr_a);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_lt, inner_expr, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a < -b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_negate, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("-(a < b)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::unop_expr_node>(operator_negate, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a < b + c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a + b < c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_lt, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("(a < b) + c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_plus, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a + (b < c)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        //  6: Boolean `&&` and `||`:
        //  -------------------------
        tokens = lexer::lex_all("a && b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_and, var_expr_a, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a || b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_or, var_expr_a, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a && b && c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_and, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_and, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a && b || c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_and, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_or, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a && (b || c)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_or, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_and, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a || b && c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_or, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_and, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a || (b || c)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_or, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_or, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("xyz[a] && b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        index_args = {var_expr_a};
        inner_array_index_expr = std::make_shared<ast_node::array_index_expr_node>(var_expr_xyz, index_args);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_and, inner_expr, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a && xyz[b]");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        index_args = {var_expr_b};
        inner_array_index_expr = std::make_shared<ast_node::array_index_expr_node>(var_expr_xyz, index_args);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_and, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("xyz[a && b]");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_and, var_expr_a, var_expr_b);
        index_args = {inner_expr};
        expected_node = std::make_shared<ast_node::array_index_expr_node>(var_expr_xyz, index_args);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("!a && b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_inverse, var_expr_a);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_and, inner_expr, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a && !b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_inverse, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_and, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("!(a && b)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_and, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::unop_expr_node>(operator_inverse, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("-a && b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_negate, var_expr_a);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_and, inner_expr, var_expr_b);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a && -b");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_negate, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_and, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("-(a && b)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_and, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::unop_expr_node>(operator_negate, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a && b * c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_times, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_and, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("(a && b) * c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_and, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_times, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a * b && c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_times, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_and, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a * (b && c)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_and, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_times, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a && b + c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_and, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("(a && b) + c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_and, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_plus, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a + b && c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_and, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a + (b && c)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_and, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a < b && c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_and, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a < (b && c)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_and, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("a && b < c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_and, var_expr_a, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("(a && b) < c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_and, var_expr_a, var_expr_b);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_lt, inner_expr, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        //  7: Prefix `array`, `sum`, and `if`:
        //  -----------------------------------
        tokens = lexer::lex_all("array[] abc");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        array_bindings = {};
        expected_node = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_abc);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("array[a: a] abc");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        array_bindings = {{variable_a, var_expr_a}};
        expected_node = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_abc);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("array[a: a, b: b] abc");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        array_bindings = {{variable_a, var_expr_a}, {variable_b, var_expr_b}};
        expected_node = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_abc);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("array[a: array[] xyz] abc");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        array_bindings = {};
        inner_expr = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_xyz);
        array_bindings = {{variable_a, inner_expr}};
        expected_node = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_abc);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("array[a: sum[] xyz] abc");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        sum_bindings = {};
        inner_expr = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_xyz);
        array_bindings = {{variable_a, inner_expr}};
        expected_node = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_abc);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("array[a: if b then c else d] abc");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::if_expr_node>(var_expr_b, var_expr_c, var_expr_d);
        array_bindings = {{variable_a, inner_expr}};
        expected_node = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_abc);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("sum[] abc");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        sum_bindings = {};
        expected_node = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_abc);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("sum[a: a] abc");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        sum_bindings = {{variable_a, var_expr_a}};
        expected_node = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_abc);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("sum[a: a, b: b] abc");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        sum_bindings = {{variable_a, var_expr_a}, {variable_b, var_expr_b}};
        expected_node = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_abc);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("sum[a: array[] xyz] abc");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        array_bindings = {};
        inner_expr = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_xyz);
        sum_bindings = {{variable_a, inner_expr}};
        expected_node = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_abc);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("sum[a: sum[] xyz] abc");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        sum_bindings = {};
        inner_expr = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_xyz);
        sum_bindings = {{variable_a, inner_expr}};
        expected_node = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_abc);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("sum[a: if b then c else d] abc");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::if_expr_node>(var_expr_b, var_expr_c, var_expr_d);
        sum_bindings = {{variable_a, inner_expr}};
        expected_node = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_abc);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("if a then b else c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        expected_node = std::make_shared<ast_node::if_expr_node>(var_expr_a, var_expr_b, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("if array[] xyz then b else c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        array_bindings = {};
        if_condition = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_xyz);
        expected_node = std::make_shared<ast_node::if_expr_node>(if_condition, var_expr_b, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("if a then array[] xyz else c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        array_bindings = {};
        if_then = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_xyz);
        expected_node = std::make_shared<ast_node::if_expr_node>(var_expr_a, if_condition, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("if a then b else array[] xyz");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        array_bindings = {};
        if_else = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_xyz);
        expected_node = std::make_shared<ast_node::if_expr_node>(var_expr_a, var_expr_b, if_else);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("if sum[] xyz then b else c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        sum_bindings = {};
        if_condition = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_xyz);
        expected_node = std::make_shared<ast_node::if_expr_node>(if_condition, var_expr_b, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("if a then sum[] xyz else c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        sum_bindings = {};
        if_then = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_xyz);
        expected_node = std::make_shared<ast_node::if_expr_node>(var_expr_a, if_condition, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("if a then b else sum[] xyz");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        sum_bindings = {};
        if_else = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_xyz);
        expected_node = std::make_shared<ast_node::if_expr_node>(var_expr_a, var_expr_b, if_else);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("-array[a: a] abc");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        array_bindings = {{variable_a, var_expr_a}};
        inner_expr = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_abc);
        expected_node = std::make_shared<ast_node::unop_expr_node>(operator_negate, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("array[a: -a] abc");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_negate, var_expr_a);
        array_bindings = {{variable_a, inner_expr}};
        expected_node = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_abc);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("array[a: a] -abc");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_negate, var_expr_abc);
        array_bindings = {{variable_a, var_expr_a}};
        expected_node = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("-sum[a: a] abc");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        sum_bindings = {{variable_a, var_expr_a}};
        inner_expr = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_abc);
        expected_node = std::make_shared<ast_node::unop_expr_node>(operator_negate, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("sum[a: -a] abc");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_negate, var_expr_a);
        sum_bindings = {{variable_a, inner_expr}};
        expected_node = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_abc);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("sum[a: a] -abc");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::unop_expr_node>(operator_negate, var_expr_abc);
        sum_bindings = {{variable_a, var_expr_a}};
        expected_node = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("-if a then b else c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        inner_expr = std::make_shared<ast_node::if_expr_node>(var_expr_a, var_expr_b, var_expr_c);
        expected_node = std::make_shared<ast_node::unop_expr_node>(operator_negate, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("if -a then b else c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        if_condition = std::make_shared<ast_node::unop_expr_node>(operator_negate, var_expr_a);
        expected_node = std::make_shared<ast_node::if_expr_node>(if_condition, var_expr_b, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("if a then -b else c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        if_then = std::make_shared<ast_node::unop_expr_node>(operator_negate, var_expr_b);
        expected_node = std::make_shared<ast_node::if_expr_node>(var_expr_a, if_then, var_expr_c);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("if a then b else -c");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        if_else = std::make_shared<ast_node::unop_expr_node>(operator_negate, var_expr_c);
        expected_node = std::make_shared<ast_node::if_expr_node>(var_expr_a, var_expr_b, if_else);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("array[] abc * array[] xyz");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        array_bindings = {};
        left_expr = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_abc);
        right_expr = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_xyz);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_times, left_expr, right_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("array[] (abc * array[] xyz)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        array_bindings = {};
        inner_expr = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_xyz);
        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_times, var_expr_abc, var_expr_xyz);
        expected_node = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("sum[] abc * sum[] xyz");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        sum_bindings = {};
        left_expr = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_abc);
        right_expr = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_xyz);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_times, left_expr, right_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("sum[] (abc * sum[] xyz)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        sum_bindings = {};
        inner_expr = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_xyz);
        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_times, var_expr_abc, var_expr_xyz);
        expected_node = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("array[] abc + array[] xyz");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        array_bindings = {};
        left_expr = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_abc);
        right_expr = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_xyz);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_plus, left_expr, right_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("array[] (abc + array[] xyz)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        array_bindings = {};
        inner_expr = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_xyz);
        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_abc, var_expr_xyz);
        expected_node = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("sum[] abc + sum[] xyz");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        sum_bindings = {};
        left_expr = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_abc);
        right_expr = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_xyz);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_plus, left_expr, right_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("sum[] (abc + sum[] xyz)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        sum_bindings = {};
        inner_expr = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_xyz);
        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_plus, var_expr_abc, var_expr_xyz);
        expected_node = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("array[] abc < array[] xyz");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        array_bindings = {};
        left_expr = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_abc);
        right_expr = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_xyz);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_lt, left_expr, right_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("array[] (abc < array[] xyz)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        array_bindings = {};
        inner_expr = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_xyz);
        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_abc, var_expr_xyz);
        expected_node = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("sum[] abc < sum[] xyz");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        sum_bindings = {};
        left_expr = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_abc);
        right_expr = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_xyz);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_lt, left_expr, right_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("sum[] (abc < sum[] xyz)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        sum_bindings = {};
        inner_expr = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_xyz);
        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_lt, var_expr_abc, var_expr_xyz);
        expected_node = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("array[] abc && array[] xyz");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        array_bindings = {};
        left_expr = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_abc);
        right_expr = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_xyz);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_and, left_expr, right_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("array[] (abc && array[] xyz)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        array_bindings = {};
        inner_expr = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, var_expr_xyz);
        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_and, var_expr_abc, var_expr_xyz);
        expected_node = std::make_shared<ast_node::array_loop_expr_node>(array_bindings, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("sum[] abc && sum[] xyz");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        sum_bindings = {};
        left_expr = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_abc);
        right_expr = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_xyz);
        expected_node = std::make_shared<ast_node::binop_expr_node>(operator_and, left_expr, right_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        tokens = lexer::lex_all("sum[] (abc && sum[] xyz)");
        node = std::get<0>(parser::parse_expr(tokens, 0));

        sum_bindings = {};
        inner_expr = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, var_expr_xyz);
        inner_expr = std::make_shared<ast_node::binop_expr_node>(operator_and, var_expr_abc, var_expr_xyz);
        expected_node = std::make_shared<ast_node::sum_loop_expr_node>(sum_bindings, inner_expr);

        result = nodes_cmp(node, expected_node);
        if (result != "") return result;

        return {};
    }

    /**
     * @brief Ensures that the parser can correctly parse lvalues.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string parse_lvalues() {
        //  `argument_lvalue_node`:
        //  -----------------------
        const token::token variable_xyz {0, "xyz", token::token_type::VARIABLE};
        const std::shared_ptr<ast_node::argument_node> argument_xyz
                = std::make_shared<ast_node::variable_argument_node>(variable_xyz);
        const std::shared_ptr<ast_node::argument_lvalue_node> argument_lvalue
                = std::make_shared<ast_node::argument_lvalue_node>(argument_xyz);

        lexer::token_list_t tokens = lexer::lex_all("xyz");
        parser::node_ptr_t node = std::get<0>(parser::parse_lvalue(tokens, 0));

        std::string result = nodes_cmp(node, argument_lvalue);
        if (result != "") return result;

        //  `tuple_lvalue_node`:
        //  --------------------
        const token::token variable_abc {0, "abc", token::token_type::VARIABLE};
        const std::shared_ptr<ast_node::argument_node> argument_abc
                = std::make_shared<ast_node::variable_argument_node>(variable_abc);
        const std::shared_ptr<ast_node::argument_lvalue_node> lvalue_abc
                = std::make_shared<ast_node::argument_lvalue_node>(argument_abc);
        const std::vector<std::shared_ptr<ast_node::lvalue_node>> lvalues {argument_lvalue, lvalue_abc};
        const std::shared_ptr<ast_node::tuple_lvalue_node> tuple_lvalue = std::make_shared<ast_node::tuple_lvalue_node>(
                lvalues);

        tokens = lexer::lex_all("{xyz, abc}");
        node = std::get<0>(parser::parse_lvalue(tokens, 0));

        return nodes_cmp(node, tuple_lvalue);
    }

    /**
     * @brief Ensures that the parser can correctly parse statements.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string parse_statements() {
        //  `assert_stmt_node`:
        //  -------------------
        const std::shared_ptr<ast_node::expr_node> expr_true = std::make_shared<ast_node::true_expr_node>();
        const token::string_token string_abc {{0, "\"abc\"", token::token_type::STRING}, "abc"};
        const std::shared_ptr<ast_node::assert_stmt_node> assert_stmt = std::make_shared<ast_node::assert_stmt_node>(
                expr_true, string_abc);

        lexer::token_list_t tokens = lexer::lex_all("assert true, \"abc\"\n");
        parser::node_ptr_t node = std::get<0>(parser::parse_statement(tokens, 0));

        std::string result = nodes_cmp(node, assert_stmt);
        if (result != "") return result;

        //  `let_stmt_node`:
        //  ----------------
        const token::token variable_xyz {0, "xyz", token::token_type::VARIABLE};
        const std::shared_ptr<ast_node::argument_node> argument_xyz
                = std::make_shared<ast_node::variable_argument_node>(variable_xyz);
        const std::shared_ptr<ast_node::lvalue_node> lvalue_xyz = std::make_shared<ast_node::argument_lvalue_node>(
                argument_xyz);
        const std::shared_ptr<ast_node::let_stmt_node> let_stmt = std::make_shared<ast_node::let_stmt_node>(lvalue_xyz,
                                                                                                            expr_true);
        tokens = lexer::lex_all("let xyz = true\n");
        node = std::get<0>(parser::parse_statement(tokens, 0));

        result = nodes_cmp(node, let_stmt);
        if (result != "") return result;

        //  `return_stmt_node`:
        //  -------------------
        const std::shared_ptr<ast_node::return_stmt_node> return_stmt = std::make_shared<ast_node::return_stmt_node>(
                expr_true);

        tokens = lexer::lex_all("return true\n");
        node = std::get<0>(parser::parse_statement(tokens, 0));

        return nodes_cmp(node, return_stmt);
    }

    /**
     * @brief Ensures that the parser can correctly parse types.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string parse_types() {
        //  `array_type_node`:
        //  ------------------
        const token::token variable_xyz {0, "xyz", token::token_type::VARIABLE};
        const std::shared_ptr<ast_node::type_node> type_xyz = std::make_shared<ast_node::variable_type_node>(
                variable_xyz);
        const std::shared_ptr<ast_node::array_type_node> array_type = std::make_shared<ast_node::array_type_node>(
                type_xyz, 2);

        lexer::token_list_t tokens = lexer::lex_all("xyz[,]");
        parser::node_ptr_t node = std::get<0>(parser::parse_type(tokens, 0));

        std::string result = nodes_cmp(node, array_type);
        if (result != "") return result;

        //  The same thing again, but with a nested array type:
        const std::shared_ptr<ast_node::array_type_node> outer_array_type = std::make_shared<ast_node::array_type_node>(
                array_type, 3);

        tokens = lexer::lex_all("xyz[,][,,]");
        node = std::get<0>(parser::parse_type(tokens, 0));

        result = nodes_cmp(node, outer_array_type);
        if (result != "") return result;

        //  `bool_type_node`:
        //  -----------------
        const std::shared_ptr<ast_node::bool_type_node> bool_type = std::make_shared<ast_node::bool_type_node>();

        tokens = lexer::lex_all("bool");
        node = std::get<0>(parser::parse_type(tokens, 0));

        result = nodes_cmp(node, bool_type);
        if (result != "") return result;

        //  `float_type_node`:
        //  ------------------
        const std::shared_ptr<ast_node::float_type_node> float_type = std::make_shared<ast_node::float_type_node>();

        tokens = lexer::lex_all("float");
        node = std::get<0>(parser::parse_type(tokens, 0));

        result = nodes_cmp(node, float_type);
        if (result != "") return result;

        //  `int_type_node`:
        //  ----------------
        const std::shared_ptr<ast_node::int_type_node> int_type = std::make_shared<ast_node::int_type_node>();

        tokens = lexer::lex_all("int");
        node = std::get<0>(parser::parse_type(tokens, 0));

        result = nodes_cmp(node, int_type);
        if (result != "") return result;

        //  `tuple_type_node`:
        //  ------------------
        const std::vector<std::shared_ptr<ast_node::type_node>> types {int_type, int_type};
        const std::shared_ptr<ast_node::tuple_type_node> tuple_type = std::make_shared<ast_node::tuple_type_node>(
                types);

        tokens = lexer::lex_all("{int, int}");
        node = std::get<0>(parser::parse_type(tokens, 0));

        result = nodes_cmp(node, tuple_type);
        if (result != "") return result;

        //  `variable_type_node`:
        //  ---------------------
        const std::shared_ptr<ast_node::variable_type_node> variable_type
                = std::make_shared<ast_node::variable_type_node>(variable_xyz);

        tokens = lexer::lex_all("xyz");
        node = std::get<0>(parser::parse_type(tokens, 0));

        return nodes_cmp(node, variable_type);
    }

    /**
     * @brief Assembles the set of all parser unit tests.
     *
     * @return The set of all parser unit tests.
     */
    std::vector<test_t> get_all_tests() {
        std::vector<test_t> tests;
        //  tests.emplace_back(parse_arguments, "Parse arguments");      //  OK
        //  tests.emplace_back(parse_bindings, "Parse bindings");        //  OK
        //  tests.emplace_back(parse_commands, "Parse commands");        //  OK
        tests.emplace_back(parse_operators, "Parse operators");      //  FAIL
        //  tests.emplace_back(parse_expressions, "Parse expressions");  //  FAIL
        //  tests.emplace_back(parse_lvalues, "Parse lvalues");          //  OK
        //  tests.emplace_back(parse_statements, "Parse statements");    //  OK
        //  tests.emplace_back(parse_types, "Parse types");              //  OK

        return tests;
    }
}  //  namespace tests::parser_tests

/**
 * @brief Parser test program entry point.
 *
 */
int main() {
    std::vector<tests::test_t> tests = tests::parser_tests::get_all_tests();
    tests::run_tests(tests);

    return 0;
}
