/**
 * @file type_checker.hpp
 * @package Assignments 6-8
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
        /**
         * @brief Class constructor.
         *
         * @param message The error message to report.
         */
        type_check_exception(const std::string& message) : std::runtime_error(message.c_str()) {}
    };

    /*
    ================
    ||  Methods:  ||
    ================
    */

    /**
     * @brief Initializes a new symbol table with the built-in methods and variables.
     *
     * @param sym_tab The symbol table to initialize.
     * @param initialize_hw6 Whether to add the extra initialization for homework 6.
     */
    void initialize_symbol_table(symbol_table::symbol_table& sym_tab, bool initialize_hw6 = false);

    /**
     * @brief Binds the given resolved type to the given lvalue in the given symbol table.
     *
     * @param lvalue The lvalue to bind.
     * @param r_type The resolved type to bind to the lvalue.
     * @param sym_tab The symbol table to update.
     */
    void bind_lvalue(const std::shared_ptr<ast_node::lvalue_node>& lvalue,
                     const std::shared_ptr<resolved_type::resolved_type>& r_type, symbol_table::symbol_table& sym_tab);

    /**
     * @brief Adds the given binding to the given symbol table.
     *
     * @param binding The binding to add.
     * @param symbol_table The symbol table to update.
     * @return The resolved type of the binding.
     */
    std::shared_ptr<resolved_type::resolved_type> add_binding(const std::shared_ptr<ast_node::binding_node>& binding,
                                                              symbol_table::symbol_table& sym_tab);

    /**
     * @brief Given a type node, returns the correct corresponding resolved type.
     *
     * @param type The type node.
     * @param sym_tab The symbol table.
     * @return A shared pointer to the new resolved type object.
     */
    std::shared_ptr<resolved_type::resolved_type> resolve_type(const std::shared_ptr<ast_node::type_node>& type,
                                                               const symbol_table::symbol_table& sym_tab);

    /**
     * @brief Given two resolved types, checks them for equality.
     *
     * @param type_1 The first resolved type.
     * @param type_2 The second resolved type.
     * @return Whether the two types are equal.
     */
    bool resolved_types_are_equal(const std::shared_ptr<resolved_type::resolved_type>& type_1,
                                  const std::shared_ptr<resolved_type::resolved_type>& type_2);

    /**
     * @brief Type-checks the given list of AST nodes.
     *
     * @param nodes A set of AST command nodes.
     * @param initialize_hw6 True if the compiler should initialize the `pict.` variable for homework 6.
     *     Defaults to false.
     */
    void check(const parser::node_list_t& nodes, bool initialize_hw6 = false);

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
     * @param r_type The return type of the containing function.
     * @return True if this statement is a return statement.
     */
    bool check_stmt(const std::shared_ptr<ast_node::stmt_node>& statement, symbol_table::symbol_table& sym_tab,
                    const std::shared_ptr<resolved_type::resolved_type>& r_type);

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

    /**
     * @brief Type-checks the given `ast_node::assert_stmt_node`.
     * @details Throws an exception on failure.
     *
     * @param statement An AST statement node.
     * @param sym_tab The symbol table accessible by this statement.
     */
    void check_stmt_assert(const std::shared_ptr<ast_node::assert_stmt_node>& statement,
                           symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::let_stmt_node`.
     * @details Throws an exception on failure.
     *
     * @param statement An AST statement node.
     * @param sym_tab The symbol table accessible by this statement.
     */
    void check_stmt_let(const std::shared_ptr<ast_node::let_stmt_node>& statement, symbol_table::symbol_table& sym_tab);

    /**
     * @brief Type-checks the given `ast_node::return_stmt_node`.
     * @details Throws an exception on failure.
     *
     * @param statement An AST statement node.
     * @param sym_tab The symbol table accessible by this statement.
     * @param r_type The return type of the containing function.
     */
    void check_stmt_return(const std::shared_ptr<ast_node::return_stmt_node>& statement,
                           symbol_table::symbol_table& sym_tab,
                           const std::shared_ptr<resolved_type::resolved_type>& r_type);
}  //  namespace type_checker

#endif
