/**
 * @file parser.tests.cpp
 * @package Assignment 3-4.
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
    inline std::string nodes_cmp(const std::vector<parser::node_ptr_t>& actual,
                                 const std::vector<parser::node_ptr_t>& expected) {
        if (actual.size() != expected.size())
            return "Parser returned the incorrect number of nodes (" + std::to_string(actual.size()) + " instead of "
                 + std::to_string(expected.size()) + ")";

        for (unsigned int index = 0; index < (unsigned int)expected.size(); index++) {
            const std::string actual_s_expr = ast_node::get_s_expression(actual[index]);
            const std::string expected_s_expr = ast_node::get_s_expression(expected[index]);
            if (actual_s_expr != expected_s_expr)
                return "Node " + std::to_string(index) + " was incorrect (" + actual_s_expr + " instead of "
                     + expected_s_expr + ")";
        }

        return "";
    }

    /**
     * @brief Ensures that the parser can correctly parse all kinds of homework 3's commands.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string parse_commands_hw3() {
        std::stringstream input;
        input << R"(assert a, "Error.")"
              << "\n"
              << R"(let x = 7)"
              << "\n"
              << R"(print "Hello, world.")"
              << "\n"
              << R"(read image "photo.png" to xyz)"
              << "\n"
              << R"(show x)"
              << "\n"
              << R"(type abc = float)"
              << "\n"
              << R"(write image xyz to "photo.png")"
              << "\n";
        parser::token_vec_t tokens = lexer::lex_all(input.str());
        std::vector<parser::node_ptr_t> nodes = parser::parse(tokens);
        std::vector<parser::node_ptr_t> expected_nodes {
                //  `assert a, "Error."`
                std::make_shared<ast_node::assert_cmd_node>(
                        std::make_shared<ast_node::variable_expr_node>(
                                token::token {0, "a", token::token_type::VARIABLE}),
                        token::string_token {{0, "\"Error.\"", token::token_type::STRING}, "Error."}),
                //  `let x = 7`
                std::make_shared<ast_node::let_cmd_node>(
                        std::make_shared<ast_node::argument_lvalue_node>(
                                std::make_shared<ast_node::variable_argument_node>(
                                        token::token {0, "x", token::token_type::VARIABLE})),
                        std::make_shared<ast_node::integer_expr_node>(
                                token::token {0, "7", token::token_type::INTVAL})),
                //  `print "Hello, world."`
                std::make_shared<ast_node::print_cmd_node>("Hello, world."),
                //  `read image "photo.png" to xyz`
                std::make_shared<ast_node::read_cmd_node>(
                        token::string_token {{0, "\"photo.png\"", token::token_type::STRING}, "photo.png"},
                        std::make_shared<ast_node::variable_argument_node>(
                                token::token {0, "xyz", token::token_type::VARIABLE})),
                //  `show x`
                std::make_shared<ast_node::show_cmd_node>(std::make_shared<ast_node::variable_expr_node>(
                        token::token {0, "x", token::token_type::VARIABLE})),
                //  `type abc = float`
                std::make_shared<ast_node::type_cmd_node>(token::token {0, "abc", token::token_type::VARIABLE},
                                                          std::make_shared<ast_node::float_type_node>()),
                //  `write image xyz to "photo.png"`
                std::make_shared<ast_node::write_cmd_node>(
                        std::make_shared<ast_node::variable_expr_node>(
                                token::token {0, "xyz", token::token_type::VARIABLE}),
                        token::string_token {{0, "\"photo.png\"", token::token_type::STRING}, "photo.png"})};

        return nodes_cmp(nodes, expected_nodes);
    }

    /**
     * @brief Ensures that the parser can correctly parse an entire function.
     *
     * @return The empty string if successful; an error message otherwise.
     */
    std::string parse_function() {
        //  Hey, check out this mess:
        std::stringstream input;
        input << "fn test_function(x : int, y : int) : bool {\n"
              << "    let left = x\n"
              << "    let right = y\n"
              << "    return true\n"
              << "}\n";

        parser::token_vec_t tokens = lexer::lex_all(input.str());
        std::vector<parser::node_ptr_t> nodes = parser::parse(tokens);

        const token::token function_name {0, "test_function", token::token_type::VARIABLE};

        const std::shared_ptr<ast_node::var_binding_node> function_binding_x
                = std::make_shared<ast_node::var_binding_node>(
                        std::make_shared<ast_node::variable_argument_node>(
                                token::token {0, "x", token::token_type::VARIABLE}),
                        std::make_shared<ast_node::int_type_node>());
        const std::shared_ptr<ast_node::var_binding_node> function_binding_y
                = std::make_shared<ast_node::var_binding_node>(
                        std::make_shared<ast_node::variable_argument_node>(
                                token::token {0, "y", token::token_type::VARIABLE}),
                        std::make_shared<ast_node::int_type_node>());
        const std::vector<std::shared_ptr<ast_node::binding_node>> function_bindings {function_binding_x,
                                                                                      function_binding_y};
        const std::shared_ptr<ast_node::type_node> function_return_type = std::make_shared<ast_node::bool_type_node>();

        const std::shared_ptr<ast_node::let_stmt_node> function_statement_1 = std::make_shared<ast_node::let_stmt_node>(
                std::make_shared<ast_node::argument_lvalue_node>(std::make_shared<ast_node::variable_argument_node>(
                        token::token {0, "left", token::token_type::VARIABLE})),
                std::make_shared<ast_node::variable_expr_node>(token::token {0, "x", token::token_type::VARIABLE}));
        const std::shared_ptr<ast_node::let_stmt_node> function_statement_2 = std::make_shared<ast_node::let_stmt_node>(
                std::make_shared<ast_node::argument_lvalue_node>(std::make_shared<ast_node::variable_argument_node>(
                        token::token {0, "right", token::token_type::VARIABLE})),
                std::make_shared<ast_node::variable_expr_node>(token::token {0, "y", token::token_type::VARIABLE}));
        const std::shared_ptr<ast_node::return_stmt_node> function_statement_3
                = std::make_shared<ast_node::return_stmt_node>(std::make_shared<ast_node::true_expr_node>());
        const std::vector<std::shared_ptr<ast_node::stmt_node>> function_statements {
                function_statement_1, function_statement_2, function_statement_3};

        const std::shared_ptr<ast_node::fn_cmd_node> function = std::make_shared<ast_node::fn_cmd_node>(
                function_name, function_bindings, function_return_type, function_statements);

        const std::vector<parser::node_ptr_t> expected_nodes {function};
        return nodes_cmp(nodes, expected_nodes);
    }

    /**
     * @brief Assembles the set of all parser unit tests.
     *
     * @return The set of all parser unit tests.
     */
    std::vector<test_t> get_all_tests() {
        std::vector<test_t> tests;
        tests.emplace_back(parse_commands_hw3, "Parse commands (hw3)");
        tests.emplace_back(parse_function, "Parse function");

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
