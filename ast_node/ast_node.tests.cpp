/**
 * @file ast_node.tests.cpp
 * @package Assignments 3-5
 * @author Cayden Lund (u1182408)
 * @brief Contains tests for the `ast_node` class and derivatives.
 *
 */

#include "ast_node.hpp"
#include "tests/tests.hpp"
#include <sstream>

namespace tests::ast_node_tests {
    /**
     * @brief Compares two strings.
     * @details This function is explicitly inline.
     *
     * @param actual The actual string.
     * @param expected The expected string.
     * @return The empty string if correct, or an error message.
     */
    inline std::string str_cmp(const std::string& actual, const std::string& expected) {
        if (actual == expected) return "";
        return "Result was '" + actual + "' instead of '" + expected + "'";
    }

    /**
     * @brief Ensures that the `variable_argument_node` constructor throws an exception when an invalid token is given.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string variable_argument_node_constructor_exception() {
        try {
            const token::token read {0, "read", token::token_type::READ};
            const ast_node::variable_argument_node variable_argument(read);
        } catch (const std::exception& except) { return ""; }

        return "The `variable_argument_node` constructor did not throw an exception";
    }

    /**
     * @brief Ensures that argument AST nodes return the correct s-expression.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string argument_s_expressions() {
        //  `argument_node`:
        //  ----------------
        const ast_node::argument_node argument;
        std::string result = str_cmp(argument.s_expression(), "");
        if (result != "") return result;

        //  `array_argument_node`:
        //  ----------------------
        const token::token variable {0, "xyz", token::token_type::VARIABLE};
        const std::vector<token::token> vars = {{0, "a", token::token_type::VARIABLE},
                                                {0, "b", token::token_type::VARIABLE},
                                                {0, "c", token::token_type::VARIABLE}};

        const ast_node::array_argument_node array_argument(variable, vars);
        result = str_cmp(array_argument.s_expression(), "(ArrayArgument xyz a b c)");
        if (result != "") return result;

        //  `variable_argument_node`:
        //  -------------------------
        const ast_node::variable_argument_node variable_argument(variable);
        result = str_cmp(variable_argument.s_expression(), "(VarArgument xyz)");
        return result;
    }

    /**
     * @brief Ensures that binding AST nodes return the correct s-expression.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string binding_s_expressions() {
        //  `binding_node`:
        //  ---------------
        const ast_node::binding_node binding;
        std::string result = str_cmp(binding.s_expression(), "");
        if (result != "") return result;

        //  `var_binding_node`:
        //  -------------------
        //  Note: we do this one before the next one because we need to check that its s-expression works.
        const token::token variable {0, "xyz", token::token_type::VARIABLE};
        const std::shared_ptr<ast_node::argument_node> argument = std::make_shared<ast_node::variable_argument_node>(
                variable);
        const std::shared_ptr<ast_node::type_node> type = std::make_shared<ast_node::bool_type_node>();

        const ast_node::var_binding_node var_binding(argument, type);
        result = str_cmp(var_binding.s_expression(), "(VarBinding (VarArgument xyz) (BoolType))");
        if (result != "") return result;

        //  `tuple_binding_node`:
        //  ---------------------
        const std::vector<std::shared_ptr<ast_node::binding_node>> bindings {
                std::make_shared<ast_node::var_binding_node>(argument, type)};

        const ast_node::tuple_binding_node tuple_binding(bindings);
        result = str_cmp(tuple_binding.s_expression(), "(TupleBinding (VarBinding (VarArgument xyz) (BoolType)))");
        if (result != "") return result;

        return "";
    }

    /**
     * @brief Ensures that command AST nodes return the correct s-expression.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string command_s_expressions() {
        //  `cmd_node`:
        //  -----------
        const ast_node::cmd_node cmd;
        std::string result = str_cmp(cmd.s_expression(), "");
        if (result != "") return result;

        //  `assert_cmd_node`:
        //  ------------------
        const std::shared_ptr<ast_node::true_expr_node> true_expr = std::make_shared<ast_node::true_expr_node>();
        const token::string_token file {{0, R"("file.png")", token::token_type::VARIABLE}, "file.png"};

        const ast_node::assert_cmd_node assert_cmd(true_expr, file);
        result = str_cmp(assert_cmd.s_expression(), R"((AssertCmd (TrueExpr) "file.png"))");
        if (result != "") return result;

        //  `fn_cmd_node`:
        //  --------------
        const token::token variable {0, "xyz", token::token_type::VARIABLE};
        const token::token variable_2 {0, "abc", token::token_type::VARIABLE};
        const token::token variable_3 {0, "qrs", token::token_type::VARIABLE};
        const std::vector<std::shared_ptr<ast_node::binding_node>> bindings {
                std::make_shared<ast_node::var_binding_node>(
                        std::make_shared<ast_node::variable_argument_node>(variable_2),
                        std::make_shared<ast_node::bool_type_node>()),
                std::make_shared<ast_node::var_binding_node>(
                        std::make_shared<ast_node::variable_argument_node>(variable_3),
                        std::make_shared<ast_node::int_type_node>())};
        const std::shared_ptr<ast_node::type_node> return_type = std::make_shared<ast_node::bool_type_node>();
        const std::vector<std::shared_ptr<ast_node::stmt_node>> statements {
                std::make_shared<ast_node::let_stmt_node>(
                        std::make_shared<ast_node::argument_lvalue_node>(
                                std::make_shared<ast_node::variable_argument_node>(variable_2)),
                        std::make_shared<ast_node::true_expr_node>()),
                std::make_shared<ast_node::return_stmt_node>(std::make_shared<ast_node::true_expr_node>())};

        const ast_node::fn_cmd_node fn_cmd(variable, bindings, return_type, statements);
        result = str_cmp(fn_cmd.s_expression(),
                         "(FnCmd xyz "
                         "((VarBinding (VarArgument abc) (BoolType)) (VarBinding (VarArgument qrs) (IntType))) "
                         "(BoolType) "
                         "(LetStmt (ArgLValue (VarArgument abc)) (TrueExpr)) "
                         "(ReturnStmt (TrueExpr)))");
        if (result != "") return result;

        //  `let_cmd_node`:
        //  ---------------
        const std::shared_ptr<ast_node::variable_argument_node> variable_argument
                = std::make_shared<ast_node::variable_argument_node>(variable);
        const std::shared_ptr<ast_node::argument_lvalue_node> argument_lvalue
                = std::make_shared<ast_node::argument_lvalue_node>(variable_argument);

        const ast_node::let_cmd_node let_cmd(argument_lvalue, true_expr);
        result = str_cmp(let_cmd.s_expression(), R"((LetCmd (ArgLValue (VarArgument xyz)) (TrueExpr)))");
        if (result != "") return result;

        //  `print_cmd_node`:
        //  -----------------
        const ast_node::print_cmd_node print_cmd(file.value);
        result = str_cmp(print_cmd.s_expression(), R"((PrintCmd "file.png"))");
        if (result != "") return result;

        //  `read_cmd_node`:
        //  ----------------
        const ast_node::read_cmd_node read_cmd(file, variable_argument);
        result = str_cmp(read_cmd.s_expression(), R"((ReadCmd "file.png" (VarArgument xyz)))");
        if (result != "") return result;

        //  `show_cmd_node`:
        //  ----------------
        const ast_node::show_cmd_node show_cmd(true_expr);
        result = str_cmp(show_cmd.s_expression(), R"((ShowCmd (TrueExpr)))");
        if (result != "") return result;

        //  `time_cmd_node`:
        //  ----------------
        std::shared_ptr<ast_node::cmd_node> command = std::make_shared<ast_node::show_cmd_node>(true_expr);

        const ast_node::time_cmd_node time_cmd(command);
        result = str_cmp(time_cmd.s_expression(), R"((TimeCmd (ShowCmd (TrueExpr))))");
        if (result != "") return result;

        //  `type_cmd_node`:
        //  ----------------
        const std::shared_ptr<ast_node::bool_type_node> bool_type = std::make_shared<ast_node::bool_type_node>();

        const ast_node::type_cmd_node type_cmd(variable, bool_type);
        result = str_cmp(type_cmd.s_expression(), R"((TypeCmd xyz (BoolType)))");
        if (result != "") return result;

        //  `write_cmd_node`:
        //  -----------------
        const std::shared_ptr<ast_node::variable_expr_node> variable_expr
                = std::make_shared<ast_node::variable_expr_node>(variable);

        const ast_node::write_cmd_node write_cmd(variable_expr, file);
        result = str_cmp(write_cmd.s_expression(), R"((WriteCmd (VarExpr xyz) "file.png"))");
        return result;
    }

    /**
     * @brief Ensures that expression AST nodes return the correct s-expression.
     * @details Does not include tests for operators.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string expression_s_expressions() {
        //  `expr_node`:
        //  ------------
        const ast_node::expr_node expr;
        std::string result = str_cmp(expr.s_expression(), "");
        if (result != "") return result;

        //  `array_index_expr_node`:
        //  ------------------------
        const token::token variable {0, "xyz", token::token_type::VARIABLE};
        const std::shared_ptr<ast_node::expr_node> array = std::make_shared<ast_node::variable_expr_node>(variable);
        const std::vector<std::shared_ptr<ast_node::expr_node>> index_params {
                std::make_shared<ast_node::true_expr_node>(), std::make_shared<ast_node::false_expr_node>()};

        const ast_node::array_index_expr_node array_index_expr(array, index_params);
        result = str_cmp(array_index_expr.s_expression(), R"((ArrayIndexExpr (VarExpr xyz) (TrueExpr) (FalseExpr)))");
        if (result != "") return result;

        //  `array_literal_expr_node`:
        //  --------------------------
        const std::vector<std::shared_ptr<ast_node::expr_node>> literal_params {
                std::make_shared<ast_node::true_expr_node>(), std::make_shared<ast_node::false_expr_node>()};

        const ast_node::array_literal_expr_node array_literal_expr(literal_params);
        result = str_cmp(array_literal_expr.s_expression(), R"((ArrayLiteralExpr (TrueExpr) (FalseExpr)))");
        if (result != "") return result;

        //  `call_expr_node`:
        //  -----------------
        const ast_node::call_expr_node call_expr(variable, literal_params);
        result = str_cmp(call_expr.s_expression(), R"((CallExpr xyz (TrueExpr) (FalseExpr)))");
        if (result != "") return result;

        //  `false_expr_node`:
        //  ------------------
        const ast_node::false_expr_node false_expr;
        result = str_cmp(false_expr.s_expression(), R"((FalseExpr))");
        if (result != "") return result;

        //  `float_expr_node`:
        //  ------------------
        const token::token float_val {0, "3.", token::token_type::FLOATVAL};

        const ast_node::float_expr_node float_expr(float_val);
        result = str_cmp(float_expr.s_expression(), R"((FloatExpr 3))");
        if (result != "") return result;

        //  `integer_expr_node`:
        //  --------------------
        const token::token int_val {0, "3", token::token_type::INTVAL};

        const ast_node::integer_expr_node integer_expr(int_val);
        result = str_cmp(integer_expr.s_expression(), R"((IntExpr 3))");
        if (result != "") return result;

        //  `true_expr_node`:
        //  -----------------
        const ast_node::true_expr_node true_expr;
        result = str_cmp(true_expr.s_expression(), R"((TrueExpr))");
        if (result != "") return result;

        //  `tuple_index_expr_node`:
        //  ------------------------
        const std::shared_ptr<ast_node::expr_node> true_expr_ptr = std::make_shared<ast_node::true_expr_node>();
        const std::shared_ptr<ast_node::integer_expr_node> index_expr = std::make_shared<ast_node::integer_expr_node>(
                token::token {0, "1", token::token_type::INTVAL});

        const ast_node::tuple_index_expr_node tuple_index_expr(true_expr_ptr, index_expr);
        result = str_cmp(tuple_index_expr.s_expression(), R"((TupleIndexExpr (TrueExpr) 1))");
        if (result != "") return result;

        //  `tuple_literal_expr_node`:
        //  --------------------------
        const ast_node::tuple_literal_expr_node tuple_literal_expr(literal_params);
        result = str_cmp(tuple_literal_expr.s_expression(), R"((TupleLiteralExpr (TrueExpr) (FalseExpr)))");
        if (result != "") return result;

        //  `variable_expr_node`:
        //  ---------------------
        const ast_node::variable_expr_node variable_expr(variable);
        result = str_cmp(variable_expr.s_expression(), R"((VarExpr xyz))");
        return result;
    }

    /**
     * @brief Ensures that operator AST nodes return the correct s-expression.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string operator_s_expressions() {
        const token::token variable_a {0, "a", token::token_type::VARIABLE};
        const token::token variable_b {0, "b", token::token_type::VARIABLE};
        const token::token variable_c {0, "c", token::token_type::VARIABLE};
        const std::shared_ptr<ast_node::expr_node> var_expr_a = std::make_shared<ast_node::variable_expr_node>(
                variable_a);
        const std::shared_ptr<ast_node::expr_node> var_expr_b = std::make_shared<ast_node::variable_expr_node>(
                variable_b);
        const std::shared_ptr<ast_node::expr_node> var_expr_c = std::make_shared<ast_node::variable_expr_node>(
                variable_c);
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

        //  `array_loop_expr_node`:
        //  -----------------------
        const std::vector<ast_node::array_loop_expr_node::binding_pair_t> array_bindings {{variable_a, var_expr_b}};
        const ast_node::array_loop_expr_node array_loop_expr(array_bindings, var_expr_c);
        std::string result = str_cmp(array_loop_expr.s_expression(), "(ArrayLoopExpr a (VarExpr b) (VarExpr c))");
        if (result != "") return result;

        //  `binop_expr_node`:
        //  ------------------
        //  `binop_type::PLUS`:
        const ast_node::binop_expr_node plus_binop_expr(operator_plus, var_expr_a, var_expr_b);
        result = str_cmp(plus_binop_expr.s_expression(), "(BinopExpr + (VarExpr a) (VarExpr b))");
        if (result != "") return result;
        //  `binop_type::MINUS`:
        const ast_node::binop_expr_node minus_binop_expr(operator_minus, var_expr_a, var_expr_b);
        result = str_cmp(minus_binop_expr.s_expression(), "(BinopExpr - (VarExpr a) (VarExpr b))");
        if (result != "") return result;
        //  `binop_type::TIMES`:
        const ast_node::binop_expr_node times_binop_expr(operator_times, var_expr_a, var_expr_b);
        result = str_cmp(times_binop_expr.s_expression(), "(BinopExpr * (VarExpr a) (VarExpr b))");
        if (result != "") return result;
        //  `binop_type::DIVIDE`:
        const ast_node::binop_expr_node divide_binop_expr(operator_divide, var_expr_a, var_expr_b);
        result = str_cmp(divide_binop_expr.s_expression(), "(BinopExpr / (VarExpr a) (VarExpr b))");
        if (result != "") return result;
        //  `binop_type::MOD`:
        const ast_node::binop_expr_node mod_binop_expr(operator_mod, var_expr_a, var_expr_b);
        result = str_cmp(mod_binop_expr.s_expression(), "(BinopExpr % (VarExpr a) (VarExpr b))");
        if (result != "") return result;
        //  `binop_type::LT`:
        const ast_node::binop_expr_node lt_binop_expr(operator_lt, var_expr_a, var_expr_b);
        result = str_cmp(lt_binop_expr.s_expression(), "(BinopExpr < (VarExpr a) (VarExpr b))");
        if (result != "") return result;
        //  `binop_type::GT`:
        const ast_node::binop_expr_node gt_binop_expr(operator_gt, var_expr_a, var_expr_b);
        result = str_cmp(gt_binop_expr.s_expression(), "(BinopExpr > (VarExpr a) (VarExpr b))");
        if (result != "") return result;
        //  `binop_type::EQ`:
        const ast_node::binop_expr_node eq_binop_expr(operator_eq, var_expr_a, var_expr_b);
        result = str_cmp(eq_binop_expr.s_expression(), "(BinopExpr == (VarExpr a) (VarExpr b))");
        if (result != "") return result;
        //  `binop_type::NEQ`:
        const ast_node::binop_expr_node neq_binop_expr(operator_neq, var_expr_a, var_expr_b);
        result = str_cmp(neq_binop_expr.s_expression(), "(BinopExpr != (VarExpr a) (VarExpr b))");
        if (result != "") return result;
        //  `binop_type::LEQ`:
        const ast_node::binop_expr_node leq_binop_expr(operator_leq, var_expr_a, var_expr_b);
        result = str_cmp(leq_binop_expr.s_expression(), "(BinopExpr <= (VarExpr a) (VarExpr b))");
        if (result != "") return result;
        //  `binop_type::GEQ`:
        const ast_node::binop_expr_node geq_binop_expr(operator_geq, var_expr_a, var_expr_b);
        result = str_cmp(geq_binop_expr.s_expression(), "(BinopExpr >= (VarExpr a) (VarExpr b))");
        if (result != "") return result;
        //  `binop_type::AND`:
        const ast_node::binop_expr_node and_binop_expr(operator_and, var_expr_a, var_expr_b);
        result = str_cmp(and_binop_expr.s_expression(), "(BinopExpr && (VarExpr a) (VarExpr b))");
        if (result != "") return result;
        //  `binop_type::OR`:
        const ast_node::binop_expr_node or_binop_expr(operator_or, var_expr_a, var_expr_b);
        result = str_cmp(or_binop_expr.s_expression(), "(BinopExpr || (VarExpr a) (VarExpr b))");
        if (result != "") return result;

        //  `if_expr_node`:
        //  ---------------
        const ast_node::if_expr_node if_expr(var_expr_a, var_expr_b, var_expr_c);
        result = str_cmp(if_expr.s_expression(), "(IfExpr (VarExpr a) (VarExpr b) (VarExpr c))");
        if (result != "") return result;

        //  `sum_loop_expr_node`:
        //  ---------------------
        const std::vector<ast_node::sum_loop_expr_node::binding_pair_t> sum_bindings {{variable_a, var_expr_b}};
        const ast_node::sum_loop_expr_node sum_loop_expr(sum_bindings, var_expr_c);
        result = str_cmp(sum_loop_expr.s_expression(), "(SumLoopExpr a (VarExpr b) (VarExpr c))");
        if (result != "") return result;

        //  `unop_expr_node`:
        //  -----------------
        //  `unop_type::INV`:
        const ast_node::unop_expr_node inv_unop_expr(operator_inverse, var_expr_a);
        result = str_cmp(inv_unop_expr.s_expression(), "(UnopExpr ! (VarExpr a))");
        if (result != "") return result;
        //  `unop_type::NEG`:
        const ast_node::unop_expr_node neg_unop_expr(operator_negate, var_expr_a);
        result = str_cmp(neg_unop_expr.s_expression(), "(UnopExpr - (VarExpr a))");
        return result;
    }

    /**
     * @brief Ensures that Lvalue AST nodes return the correct s-expression.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string lvalue_s_expressions() {
        //  `lvalue_node`:
        //  --------------
        const ast_node::lvalue_node lvalue;
        std::string result = str_cmp(lvalue.s_expression(), "");
        if (result != "") return result;

        //  `argument_lvalue_node`:
        //  -----------------------
        const token::token variable {0, "xyz", token::token_type::VARIABLE};
        const std::shared_ptr<ast_node::variable_argument_node> variable_argument
                = std::make_shared<ast_node::variable_argument_node>(variable);

        const ast_node::argument_lvalue_node argument_lvalue(variable_argument);
        result = str_cmp(argument_lvalue.s_expression(), "(ArgLValue (VarArgument xyz))");
        if (result != "") return result;

        //  `tuple_lvalue_node`:
        //  --------------------
        const std::vector<std::shared_ptr<ast_node::lvalue_node>> lvalues {
                std::make_shared<ast_node::argument_lvalue_node>(std::make_shared<ast_node::variable_argument_node>(
                        token::token {0, "abc", token::token_type::VARIABLE})),
                std::make_shared<ast_node::argument_lvalue_node>(std::make_shared<ast_node::variable_argument_node>(
                        token::token {0, "xyz", token::token_type::VARIABLE}))};

        const ast_node::tuple_lvalue_node tuple_lvalue(lvalues);
        result = str_cmp(tuple_lvalue.s_expression(),
                         "(TupleLValue (ArgLValue (VarArgument abc)) (ArgLValue (VarArgument xyz)))");
        return result;
    }

    /**
     * @brief Ensures that statement AST nodes return the correct s-expression.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string statement_s_expressions() {
        //  `stmt_node`:
        //  ------------
        const ast_node::stmt_node stmt;
        std::string result = str_cmp(stmt.s_expression(), "");
        if (result != "") return result;

        //  `assert_stmt_node`:
        //  -------------------
        const std::shared_ptr<ast_node::true_expr_node> true_expr = std::make_shared<ast_node::true_expr_node>();
        const token::string_token file {{0, R"("file.png")", token::token_type::VARIABLE}, "file.png"};

        const ast_node::assert_stmt_node assert_stmt(true_expr, file);
        result = str_cmp(assert_stmt.s_expression(), R"((AssertStmt (TrueExpr) "file.png"))");
        if (result != "") return result;

        //  `let_stmt_node`:
        //  ----------------
        const token::token variable {0, "xyz", token::token_type::VARIABLE};
        const std::shared_ptr<ast_node::variable_argument_node> variable_argument
                = std::make_shared<ast_node::variable_argument_node>(variable);
        const std::shared_ptr<ast_node::argument_lvalue_node> argument_lvalue
                = std::make_shared<ast_node::argument_lvalue_node>(variable_argument);

        const ast_node::let_stmt_node let_stmt(argument_lvalue, true_expr);
        result = str_cmp(let_stmt.s_expression(), R"((LetStmt (ArgLValue (VarArgument xyz)) (TrueExpr)))");
        if (result != "") return result;

        //  `return_stmt_node`:
        //  -------------------
        const std::shared_ptr<ast_node::expr_node> return_val = std::make_shared<ast_node::variable_expr_node>(
                variable);

        const ast_node::return_stmt_node return_stmt(return_val);
        result = str_cmp(return_stmt.s_expression(), R"((ReturnStmt (VarExpr xyz)))");
        return result;
    }

    /**
     * @brief Ensures that type AST nodes return the correct s-expression.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string type_s_expressions() {
        //  `type_node`:
        //  ------------
        const ast_node::type_node type;
        std::string result = str_cmp(type.s_expression(), "");
        if (result != "") return result;

        //  `array_type_node`:
        //  ------------------
        const std::shared_ptr<ast_node::type_node> array_main_type = std::make_shared<ast_node::bool_type_node>();

        const ast_node::array_type_node array_type(array_main_type, 2);
        result = str_cmp(array_type.s_expression(), "(ArrayType (BoolType) 2)");
        if (result != "") return result;

        //  `bool_type_node`:
        //  -----------------
        const ast_node::bool_type_node bool_type;
        result = str_cmp(bool_type.s_expression(), "(BoolType)");
        if (result != "") return result;

        //  `float_type_node`:
        //  ------------------
        const ast_node::float_type_node float_type;
        result = str_cmp(float_type.s_expression(), "(FloatType)");
        if (result != "") return result;

        //  `int_type_node`:
        //  ----------------
        const ast_node::int_type_node int_type;
        result = str_cmp(int_type.s_expression(), "(IntType)");
        if (result != "") return result;

        //  `tuple_type_node`:
        //  ------------------
        const std::vector<std::shared_ptr<ast_node::type_node>> type_nodes {
                std::make_shared<ast_node::bool_type_node>(), std::make_shared<ast_node::float_type_node>(),
                std::make_shared<ast_node::int_type_node>()};

        const ast_node::tuple_type_node tuple_type(type_nodes);
        result = str_cmp(tuple_type.s_expression(), "(TupleType (BoolType) (FloatType) (IntType))");
        if (result != "") return result;

        //  `variable_type_node`:
        //  ---------------------
        const token::token variable {0, "xyz", token::token_type::VARIABLE};

        const ast_node::variable_type_node variable_type(variable);
        result = str_cmp(variable_type.s_expression(), "(VarType xyz)");
        return result;
    }

    /**
     * @brief Assembles the set of all AST node unit tests.
     *
     * @return The set of all AST node unit tests.
     */
    std::vector<test_t> get_all_tests() {
        std::vector<test_t> tests;
        tests.emplace_back(variable_argument_node_constructor_exception,
                           "`variable_argument_node` constructor: exception");
        tests.emplace_back(argument_s_expressions, "Argument s-expressions");
        tests.emplace_back(binding_s_expressions, "Binding s-expressions");
        tests.emplace_back(command_s_expressions, "Command s-expressions");
        tests.emplace_back(expression_s_expressions, "Expression s-expressions");
        tests.emplace_back(operator_s_expressions, "Operator s-expressions");
        tests.emplace_back(lvalue_s_expressions, "Lvalue s-expressions");
        tests.emplace_back(statement_s_expressions, "Statement s-expressions");
        tests.emplace_back(type_s_expressions, "Type s-expressions");
        return tests;
    }
}  //  namespace tests::ast_node_tests

/**
 * @brief `lexer` test program entry point.
 *
 */
int main() {
    std::vector<tests::test_t> tests = tests::ast_node_tests::get_all_tests();
    tests::run_tests(tests);

    return 0;
}
