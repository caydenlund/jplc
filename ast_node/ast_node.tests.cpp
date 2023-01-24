/**
 * @file ast_node.tests.cpp
 * @package Assignment 3.
 * @author Cayden Lund (u1182408)
 * @brief Contains tests for the `ast_node` class and derivatives.
 *
 */

#include "ast_node.hpp"
#include "tests/tests.hpp"

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
     * @brief Ensures that the `ast_node` constructors do not throw an exception.
     *
     * @return The empty string if successful; an exception is thrown otherwise.
     */
    std::string constructor_no_fail() {
        [[maybe_unused]] const ast_node::ast_node ast(ast_node::node_type::CMD);
        [[maybe_unused]] const ast_node::cmd_node cmd;

        const token::token variable {0, "xyz", token::token_type::VARIABLE};
        const ast_node::variable_argument_node variable_argument(variable);

        return "";
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

        //  `variable_argument_node`:
        //  -------------------------
        const token::token variable {0, "xyz", token::token_type::VARIABLE};

        const ast_node::variable_argument_node variable_argument(variable);
        result = str_cmp(variable_argument.s_expression(), "(VarArgument xyz)");
        return result;
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

        //  `let_cmd_node`:
        //  ---------------
        const token::token variable {0, "xyz", token::token_type::VARIABLE};
        const std::shared_ptr<ast_node::variable_argument_node> variable_argument
                = std::make_shared<ast_node::variable_argument_node>(variable);
        const std::shared_ptr<ast_node::argument_lvalue_node> argument_lvalue
                = std::make_shared<ast_node::argument_lvalue_node>(variable_argument);

        const ast_node::let_cmd_node let_cmd(argument_lvalue, true_expr);
        result = str_cmp(let_cmd.s_expression(), R"((LetCmd (ArgLValue (VarArgument xyz)) (TrueExpr)))");
        if (result != "") return result;

        //  `print_cmd_node`:
        //  -----------------
        const ast_node::print_cmd_node print_cmd(file);
        result = str_cmp(print_cmd.s_expression(), R"((PrintCmd "file.png"))");
        if (result != "") return result;

        //  `read_cmd_node`:
        //  ----------------
        const ast_node::read_cmd_node read_cmd(file, variable_argument);
        result = str_cmp(read_cmd.s_expression(), R"((ReadCmd "photo.png" (VarArgument xyz)))");
        if (result != "") return result;

        //  `show_cmd_node`:
        //  ----------------
        const ast_node::show_cmd_node show_cmd(true_expr);
        result = str_cmp(show_cmd.s_expression(), R"((ShowCmd (TrueExpr)))");
        if (result != "") return result;

        //  `type_cmd_node`:
        //  ----------------
        const std::shared_ptr<ast_node::bool_type_node> bool_type = std::make_shared<ast_node::bool_type_node>();

        const ast_node::type_cmd_node type_cmd(variable, bool_type);
        result = str_cmp(read_cmd.s_expression(), R"((TypeCmd (VarArgument xyz) (BoolType)))");
        if (result != "") return result;

        //  `write_cmd_node`:
        //  -----------------
        const ast_node::write_cmd_node write_cmd(variable_argument, file);
        result = str_cmp(read_cmd.s_expression(), R"((WriteCmd (VarArgument xyz) "photo.png"))");
        return result;
    }

    /**
     * @brief Ensures that expression AST nodes return the correct s-expression.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string expression_s_expressions() {
        //  `expr_node`:
        //  ------------
        const ast_node::expr_node expr;
        std::string result = str_cmp(expr.s_expression(), "");
        if (result != "") return result;

        //  `false_expr_node`:
        //  ------------------
        const ast_node::false_expr_node false_expr;
        result = str_cmp(false_expr.s_expression(), R"((FalseExpr))");
        if (result != "") return result;

        //  `float_expr_node`:
        //  ------------------
        const token::float_token float_val {{0, "3.", token::token_type::INTVAL}, 3.0};
        const ast_node::float_expr_node float_expr(float_val);
        result = str_cmp(float_expr.s_expression(), R"((FloatExpr 3))");
        if (result != "") return result;

        //  `integer_expr_node`:
        //  --------------------
        const token::int_token int_val {{0, "3", token::token_type::INTVAL}, 3};
        const ast_node::integer_expr_node integer_expr(int_val);
        result = str_cmp(integer_expr.s_expression(), R"((IntExpr 3))");
        if (result != "") return result;

        //  `true_expr_node`:
        //  -----------------
        const ast_node::true_expr_node true_expr;
        result = str_cmp(true_expr.s_expression(), R"((TrueExpr))");
        if (result != "") return result;

        //  `variable_expr_node`:
        //  ---------------------
        const token::token variable {0, "xyz", token::token_type::VARIABLE};
        const ast_node::variable_expr_node variable_expr(variable);
        result = str_cmp(variable_expr.s_expression(), R"((VarExpr xyz))");
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
        tests.emplace_back(constructor_no_fail, "`ast_node` constructor: no-fail");
        tests.emplace_back(variable_argument_node_constructor_exception,
                           "`variable_argument_node` constructor: exception");
        tests.emplace_back(argument_s_expressions, "Argument s-expressions");
        tests.emplace_back(command_s_expressions, "Command s-expressions");
        tests.emplace_back(expression_s_expressions, "Expression s-expressions");
        tests.emplace_back(lvalue_s_expressions, "Lvalue s-expressions");
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
