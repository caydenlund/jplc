/**
 * @file type_checker.hpp
 * @package Assignment 6
 * @author Cayden Lund (u1182408)
 * @brief Defines the `type_checker` methods.
 *
 */

#ifndef TYPE_CHECKER_HPP
#define TYPE_CHECKER_HPP

#include <unordered_map>
#include <vector>

#include "ast_node/ast_node.hpp"
#include "parser/parser.hpp"
#include "resolved_type/resolved_type.hpp"
#include "symbol_table/symbol_table.hpp"

namespace type_checker {
    /*
    ==============
    ||  Types:  ||
    ==============
    */

    /**
     * @brief The exception that's thrown when the type checker fails.
     *
     */
    struct type_check_exception : public std::runtime_error {
        type_check_exception(const std::string& message) : std::runtime_error(message.c_str()) {}
    };

    /*
    ================
    ||  Methods:  ||
    ================
    */

    /**
     * @brief Type-checks the given list of AST nodes.
     *
     * @param nodes A set of AST command nodes.
     */
    void check(const parser::node_list_t& nodes);

    /*
    =====================
    ||  Superclasses:  ||
    =====================
    */

    /**
     * @brief Type-checks the given AST command node.
     * @details Throws an exception on failure.
     *
     * @param command An AST command node.
     * @param sym_tab The symbol table accessible by this command.
     */
    void check_cmd(const std::shared_ptr<ast_node::cmd_node>& command, symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given AST expression node.
     * @details Throws an exception on failure.
     *
     * @param expression An AST expression node.
     * @param sym_tab The symbol table accessible by this expression.
     * @return The type of the expression.
     */
    std::shared_ptr<resolved_type::resolved_type> check_expr(const std::shared_ptr<ast_node::expr_node>& expression,
                                                             const symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given AST statement node.
     * @details Throws an exception on failure.
     *
     * @param statement An AST statement node.
     * @param sym_tab The symbol table accessible by this expression.
     */
    void check_stmt(const std::shared_ptr<ast_node::stmt_node>& statement, symbol_table::symbol_table& sym_tab);

    /*
    ===================
    ||  Subclasses:  ||
    ===================
    */

    //  Commands:
    //  ---------

    /**
     * @brief Type-checks the given `ast_node::assert_cmd_node`.
     * @details Throws an exception on failure.
     *
     * @param command An AST command node.
     * @param sym_tab The symbol table accessible by this command.
     */
    void check_cmd_assert(const std::shared_ptr<ast_node::assert_cmd_node>& command,
                          symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::fn_cmd_node`.
     * @details Throws an exception on failure.
     *
     * @param command An AST command node.
     * @param sym_tab The symbol table accessible by this command.
     */
    void check_cmd_fn(const std::shared_ptr<ast_node::fn_cmd_node>& command, symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::let_cmd_node`.
     * @details Throws an exception on failure.
     *
     * @param command An AST command node.
     * @param sym_tab The symbol table accessible by this command.
     */
    void check_cmd_let(const std::shared_ptr<ast_node::let_cmd_node>& command, symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::print_cmd_node`.
     * @details Throws an exception on failure.
     *
     * @param command An AST command node.
     * @param sym_tab The symbol table accessible by this command.
     */
    void check_cmd_print(const std::shared_ptr<ast_node::print_cmd_node>& command, symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::read_cmd_node`.
     * @details Throws an exception on failure.
     *
     * @param command An AST command node.
     * @param sym_tab The symbol table accessible by this command.
     */
    void check_cmd_read(const std::shared_ptr<ast_node::read_cmd_node>& command, symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::show_cmd_node`.
     * @details Throws an exception on failure.
     *
     * @param command An AST command node.
     * @param sym_tab The symbol table accessible by this command.
     */
    void check_cmd_show(const std::shared_ptr<ast_node::show_cmd_node>& command, symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::time_cmd_node`.
     * @details Throws an exception on failure.
     *
     * @param command An AST command node.
     * @param sym_tab The symbol table accessible by this command.
     */
    void check_cmd_time(const std::shared_ptr<ast_node::time_cmd_node>& command, symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::type_cmd_node`.
     * @details Throws an exception on failure.
     *
     * @param command An AST command node.
     * @param sym_tab The symbol table accessible by this command.
     */
    void check_cmd_type(const std::shared_ptr<ast_node::type_cmd_node>& command, symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::write_cmd_node`.
     * @details Throws an exception on failure.
     *
     * @param command An AST command node.
     * @param sym_tab The symbol table accessible by this command.
     */
    void check_cmd_write(const std::shared_ptr<ast_node::write_cmd_node>& command, symbol_table::symbol_table& sym_tab);

    //  Expressions:
    //  ------------

    /**
     * @brief Type-checks the given `ast_node::array_index_expr_node`.
     * @details Throws an exception on failure.
     *
     * @param expression An AST expression node.
     * @param sym_tab The symbol table accessible by this expression.
     * @return The type of the expression.
     */
    std::shared_ptr<resolved_type::resolved_type>
    check_expr_array_index(const std::shared_ptr<ast_node::array_index_expr_node>& expression,
                           const symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::array_literal_expr_node`.
     * @details Throws an exception on failure.
     *
     * @param expression An AST expression node.
     * @param sym_tab The symbol table accessible by this expression.
     * @return The type of the expression.
     */
    std::shared_ptr<resolved_type::resolved_type>
    check_expr_array_literal(const std::shared_ptr<ast_node::array_literal_expr_node>& expression,
                             const symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::array_loop_expr_node`.
     * @details Throws an exception on failure.
     *
     * @param expression An AST expression node.
     * @param sym_tab The symbol table accessible by this expression.
     * @return The type of the expression.
     */
    std::shared_ptr<resolved_type::resolved_type>
    check_expr_array_loop(const std::shared_ptr<ast_node::array_loop_expr_node>& expression,
                          const symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::binop_expr_node`.
     * @details Throws an exception on failure.
     *
     * @param expression An AST expression node.
     * @param sym_tab The symbol table accessible by this expression.
     * @return The type of the expression.
     */
    std::shared_ptr<resolved_type::resolved_type>
    check_expr_binop(const std::shared_ptr<ast_node::binop_expr_node>& expression,
                     const symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::call_expr_node`.
     * @details Throws an exception on failure.
     *
     * @param expression An AST expression node.
     * @param sym_tab The symbol table accessible by this expression.
     * @return The type of the expression.
     */
    std::shared_ptr<resolved_type::resolved_type>
    check_expr_call(const std::shared_ptr<ast_node::call_expr_node>& expression,
                    const symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::false_expr_node`.
     * @details Throws an exception on failure.
     *
     * @param expression An AST expression node.
     * @param sym_tab The symbol table accessible by this expression.
     * @return The type of the expression.
     */
    std::shared_ptr<resolved_type::resolved_type>
    check_expr_false(const std::shared_ptr<ast_node::false_expr_node>& expression,
                     const symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::float_expr_node`.
     * @details Throws an exception on failure.
     *
     * @param expression An AST expression node.
     * @param sym_tab The symbol table accessible by this expression.
     * @return The type of the expression.
     */
    std::shared_ptr<resolved_type::resolved_type>
    check_expr_float(const std::shared_ptr<ast_node::float_expr_node>& expression,
                     const symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::if_expr_node`.
     * @details Throws an exception on failure.
     *
     * @param expression An AST expression node.
     * @param sym_tab The symbol table accessible by this expression.
     * @return The type of the expression.
     */
    std::shared_ptr<resolved_type::resolved_type>
    check_expr_if(const std::shared_ptr<ast_node::if_expr_node>& expression, const symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::integer_expr_node`.
     * @details Throws an exception on failure.
     *
     * @param expression An AST expression node.
     * @param sym_tab The symbol table accessible by this expression.
     * @return The type of the expression.
     */
    std::shared_ptr<resolved_type::resolved_type>
    check_expr_integer(const std::shared_ptr<ast_node::integer_expr_node>& expression,
                       const symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::sum_loop_expr_node`.
     * @details Throws an exception on failure.
     *
     * @param expression An AST expression node.
     * @param sym_tab The symbol table accessible by this expression.
     * @return The type of the expression.
     */
    std::shared_ptr<resolved_type::resolved_type>
    check_expr_sum_loop(const std::shared_ptr<ast_node::sum_loop_expr_node>& expression,
                        const symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::true_expr_node`.
     * @details Throws an exception on failure.
     *
     * @param expression An AST expression node.
     * @param sym_tab The symbol table accessible by this expression.
     * @return The type of the expression.
     */
    std::shared_ptr<resolved_type::resolved_type>
    check_expr_true(const std::shared_ptr<ast_node::true_expr_node>& expression,
                    const symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::tuple_index_expr_node`.
     * @details Throws an exception on failure.
     *
     * @param expression An AST expression node.
     * @param sym_tab The symbol table accessible by this expression.
     * @return The type of the expression.
     */
    std::shared_ptr<resolved_type::resolved_type>
    check_expr_tuple_index(const std::shared_ptr<ast_node::tuple_index_expr_node>& expression,
                           const symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::tuple_literal_expr_node`.
     * @details Throws an exception on failure.
     *
     * @param expression An AST expression node.
     * @param sym_tab The symbol table accessible by this expression.
     * @return The type of the expression.
     */
    std::shared_ptr<resolved_type::resolved_type>
    check_expr_tuple_literal(const std::shared_ptr<ast_node::tuple_literal_expr_node>& expression,
                             const symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::unop_expr_node`.
     * @details Throws an exception on failure.
     *
     * @param expression An AST expression node.
     * @param sym_tab The symbol table accessible by this expression.
     * @return The type of the expression.
     */
    std::shared_ptr<resolved_type::resolved_type>
    check_expr_unop(const std::shared_ptr<ast_node::unop_expr_node>& expression,
                    const symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::variable_expr_node`.
     * @details Throws an exception on failure.
     *
     * @param expression An AST expression node.
     * @param sym_tab The symbol table accessible by this expression.
     * @return The type of the expression.
     */
    std::shared_ptr<resolved_type::resolved_type>
    check_expr_variable(const std::shared_ptr<ast_node::variable_expr_node>& expression,
                        const symbol_table::symbol_table& sym_tab);
}  //  namespace type_checker

#endif
