/**
 * @file ast_node.hpp
 * @package Assignment 3
 * @author Cayden Lund (u1182408)
 * @brief Defines the AST node classes.
 *
 */

#ifndef AST_NODE_HPP
#define AST_NODE_HPP

namespace ast_node {
    /**
     * @brief The base AST node class from which all other AST nodes inherit.
     *
     */
    struct ast_node {};

    /**
     * @brief The base `cmd` class from which all other `cmd` AST nodes inherit.
     *
     */
    struct cmd : public ast_node {};

    /**
     * @brief The `read` command.
     * @details Construction: `read image <string> to <argument>`
     *
     */
    struct read_cmd : public cmd {};

    /**
     * @brief The `write` command.
     * @details Construction: `write image <expr> to <string>`
     *
     */
    struct write_cmd : public cmd {};

    /**
     * @brief The `type` command.
     * @details Construction: `type <variable> = <type>`
     *
     */
    struct type_cmd : public cmd {};

    /**
     * @brief The `let` command.
     * @details Construction: `let <lvalue> = <expr>`
     *
     */
    struct let_cmd : public cmd {};

    /**
     * @brief The `assert` command.
     * @details Construction: `assert <expr>, <string>`
     *
     */
    struct assert_cmd : public cmd {};

    /**
     * @brief The `print` command.
     * @details Construction: `print <string>`
     *
     */
    struct print_cmd : public cmd {};

    /**
     * @brief The `show` command.
     * @details Construction: `show <expr>`
     *
     */
    struct show_cmd : public cmd {};

    /**
     * @brief The base `type` class from which all other `type` AST nodes inherit.
     *
     */
    struct type : public ast_node {};

    /**
     * @brief The `int` type.
     *
     */
    struct int_type : public type {};

    /**
     * @brief The `bool` type.
     *
     */
    struct bool_type : public type {};

    /**
     * @brief The `float` type.
     *
     */
    struct float_type : public type {};

    /**
     * @brief The `<variable>` type.
     *
     */
    struct variable_type : public type {};

    /**
     * @brief The base `expr` class from which all other `expr` AST nodes inherit.
     *
     */
    struct expr : public ast_node {};

    /**
     * @brief The `<integer>` expression.
     *
     */
    struct integer_expr : public expr {};

    /**
     * @brief The `<float>` expression.
     *
     */
    struct float_expr : public expr {};

    /**
     * @brief The `true` expression.
     *
     */
    struct true_expr : public expr {};

    /**
     * @brief The `false` expression.
     *
     */
    struct false_expr : public expr {};

    /**
     * @brief The `<variable>` expression.
     *
     */
    struct variable_expr : public expr {};

    /**
     * @brief The base `argument` class from which all other `argument` AST nodes inherit.
     *
     */
    struct argument : public ast_node {};

    /**
     * @brief The `<variable>` argument.
     *
     */
    struct variable_argument : public argument {};

    /**
     * @brief The base `lvalue` class from which all other `lvalue` AST nodes inherit.
     *
     */
    struct lvalue : public ast_node {};

    /**
     * @brief The `<argument>` lvalue.
     *
     */
    struct argument_lvalue : public lvalue {};
}  //  namespace ast_node

#endif
