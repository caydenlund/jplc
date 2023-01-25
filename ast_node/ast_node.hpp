/**
 * @file ast_node.hpp
 * @package Assignment 3
 * @author Cayden Lund (u1182408)
 * @brief Defines the AST node classes.
 *
 */

#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include <memory>
#include <stdexcept>
#include <string>

#include "token/token.hpp"

namespace ast_node {
    /**
     * @brief An enumeration of all AST node types.
     *
     */
    enum node_type {
        //  ==================
        //  ||  Arguments:  ||
        //  ==================
        ARGUMENT,           //  AST node `argument_node`.
        VARIABLE_ARGUMENT,  //  AST node `variable_argument_node`.

        //  =================
        //  ||  Commands:  ||
        //  =================
        CMD,         //  AST node `cmd_node`.
        READ_CMD,    //  AST node `read_cmd_node`.
        WRITE_CMD,   //  AST node `write_cmd_node`.
        TYPE_CMD,    //  AST node `type_cmd_node`.
        LET_CMD,     //  AST node `let_cmd_node`.
        ASSERT_CMD,  //  AST node `assert_cmd_node`.
        PRINT_CMD,   //  AST node `print_cmd_node`.
        SHOW_CMD,    //  AST node `show_cmd_node`.

        //  ====================
        //  ||  Expressions:  ||
        //  ====================
        EXPR,           //  AST node `expr_node`.
        INTEGER_EXPR,   //  AST node `integer_expr_node`.
        FLOAT_EXPR,     //  AST node `float_expr_node`.
        TRUE_EXPR,      //  AST node `true_expr_node`.
        FALSE_EXPR,     //  AST node `false_expr_node`.
        VARIABLE_EXPR,  //  AST node `variable_expr_node`.

        //  ================
        //  ||  Lvalues:  ||
        //  ================
        LVALUE,           //  AST node `lvalue_node`.
        ARGUMENT_LVALUE,  //  AST node `argument_lvalue_node`.

        //  ==============
        //  ||  Types:  ||
        //  ==============
        TYPE,          //  AST node `type_node`.
        INT_TYPE,      //  AST node `int_type_node`.
        BOOL_TYPE,     //  AST node `bool_type_node`.
        FLOAT_TYPE,    //  AST node `float_type_node`.
        VARIABLE_TYPE  //  AST node `variable_type_node`.
    };

    /*
    =====================
    ||  Superclasses:  ||
    =====================
    */
    /**
     * @brief The base AST node class from which all other AST nodes inherit.
     *
     */
    struct ast_node {
        /**
         * @brief The type of this AST node.
         *
         */
        node_type type;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to the given type.
         *
         * @param type The type of this AST node.
         */
        ast_node(node_type type) : type(type) {}

        /**
         * @brief Class destructor.
         *
         */
        virtual ~ast_node() = default;

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        virtual std::string s_expression() const;
    };

    /**
     * @brief The base `argument` class from which all other `argument` AST nodes inherit.
     *
     */
    struct argument_node : public ast_node {
        /**
         * @brief Class constructor.
         * @details Initializes `type` to the given type.
         *
         * @param type The type of this AST node. Defaults to `node_type::ARGUMENT`.
         */
        argument_node(node_type type = node_type::ARGUMENT) : ast_node(type) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The base `cmd` class from which all other `cmd` AST nodes inherit.
     *
     */
    struct cmd_node : public ast_node {
        /**
         * @brief Class constructor.
         * @details Initializes `type` to the given type.
         *
         * @param type The type of this AST node. Defaults to `node_type::CMD`.
         */
        cmd_node(node_type type = node_type::CMD) : ast_node(type) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The base `expr` class from which all other `expr` AST nodes inherit.
     *
     */
    struct expr_node : public ast_node {
        /**
         * @brief Class constructor.
         * @details Initializes `type` to the given type.
         *
         * @param type The type of this AST node. Defaults to `node_type::EXPR`.
         */
        expr_node(node_type type = node_type::EXPR) : ast_node(type) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The base `lvalue` class from which all other `lvalue` AST nodes inherit.
     *
     */
    struct lvalue_node : public ast_node {
        /**
         * @brief Class constructor.
         * @details Initializes `type` to the given type.
         *
         * @param type The type of this AST node. Defaults to `node_type::LVALUE`.
         */
        lvalue_node(node_type type = node_type::LVALUE) : ast_node(type) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The base `type` class from which all other `type` AST nodes inherit.
     *
     */
    struct type_node : public ast_node {
        /**
         * @brief Class constructor.
         * @details Initializes `type` to the given type.
         *
         * @param type The type of this AST node. Defaults to `node_type::TYPE`.
         */
        type_node(node_type type = node_type::TYPE) : ast_node(type) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /*
    ========================
    ||  Derived classes:  ||
    ========================
    */

    //  Arguments:
    //  ----------
    /**
     * @brief The `<variable>` argument.
     *
     */
    struct variable_argument_node : public argument_node {
        /**
         * @brief The `<variable>` node argument.
         *
         */
        const token::token arg_1;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::VARIABLE_ARGUMENT`.
         *
         * @param arg_1 The first (`<variable>`) argument.
         */
        variable_argument_node(const token::token& arg_1) : argument_node(node_type::VARIABLE_ARGUMENT), arg_1(arg_1) {
            if (arg_1.type != token::token_type::VARIABLE)
                throw std::runtime_error(
                        "Attempted to construct a `variable_argument_node` without a `<variable>` argument");
        }

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    //  Commands:
    //  ---------
    /**
     * @brief The `assert` command.
     * @details Construction: `assert <expr>, <string>`
     *
     */
    struct assert_cmd_node : public cmd_node {
        /**
         * @brief The `<expr>` node argument.
         *
         */
        const std::shared_ptr<expr_node> arg_1;

        /**
         * @brief The `<string>` node argument.
         *
         */
        const token::string_token arg_2;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::ASSERT_CMD`.
         *
         * @param arg_1 The first (`<expr>`) argument.
         * @param arg_2 The second (`<string>`) argument.
         */
        assert_cmd_node(const std::shared_ptr<expr_node>& arg_1, const token::string_token& arg_2)
            : cmd_node(node_type::ASSERT_CMD), arg_1(arg_1), arg_2(arg_2) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `let` command.
     * @details Construction: `let <lvalue> = <expr>`
     *
     */
    struct let_cmd_node : public cmd_node {
        /**
         * @brief The `<lvalue>` node argument.
         *
         */
        const std::shared_ptr<lvalue_node> arg_1;

        /**
         * @brief The `<expr>` node argument.
         *
         */
        const std::shared_ptr<expr_node> arg_2;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::LET_CMD`.
         *
         * @param arg_1 The first (`<lvalue>`) node.
         * @param arg_2 The second (`<expr>`) node.
         */
        let_cmd_node(const std::shared_ptr<lvalue_node>& arg_1, const std::shared_ptr<expr_node>& arg_2)
            : cmd_node(node_type::LET_CMD), arg_1(arg_1), arg_2(arg_2) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `print` command.
     * @details Construction: `print <string>`
     *
     */
    struct print_cmd_node : public cmd_node {
        /**
         * @brief The `<string>` node argument.
         *
         */
        const std::string arg_1;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::PRINT_CMD`.
         *
         * @param arg_1 The first (`<string>`) argument.
         */
        print_cmd_node(const std::string& arg_1) : cmd_node(node_type::PRINT_CMD), arg_1(arg_1) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `read` command.
     * @details Construction: `read image <string> to <argument>`
     *
     */
    struct read_cmd_node : public cmd_node {
        /**
         * @brief The `<string>` node argument.
         *
         */
        const token::string_token arg_1;

        /**
         * @brief The `<argument>` node argument.
         *
         */
        const std::shared_ptr<argument_node> arg_2;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::READ_CMD`.
         *
         * @param arg_1 The first (`<string>`) argument.
         * @param arg_2 The second (`<argument>`) argument.
         */
        read_cmd_node(const token::string_token& arg_1, std::shared_ptr<argument_node> arg_2)
            : cmd_node(node_type::READ_CMD), arg_1(arg_1), arg_2(arg_2) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `show` command.
     * @details Construction: `show <expr>`
     *
     */
    struct show_cmd_node : public cmd_node {
        /**
         * @brief The `<expr>` node argument.
         *
         */
        const std::shared_ptr<expr_node> arg_1;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::SHOW_CMD`.
         *
         * @param arg_1 The first (`<expr>`) argument.
         */
        show_cmd_node(const std::shared_ptr<expr_node>& arg_1) : cmd_node(node_type::SHOW_CMD), arg_1(arg_1) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `type` command.
     * @details Construction: `type <variable> = <type>`
     *
     */
    struct type_cmd_node : public cmd_node {
        /**
         * @brief The `<variable>` node argument.
         *
         */
        const token::token arg_1;

        /**
         * @brief The `<type>` node argument.
         *
         */
        const std::shared_ptr<type_node> arg_2;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::TYPE_CMD`.
         *
         * @param arg_1 The first (`<variable>`) argument.
         * @param arg_2 The second (`<type>`) argument.
         */
        type_cmd_node(const token::token& arg_1, const std::shared_ptr<type_node>& arg_2)
            : cmd_node(node_type::TYPE_CMD), arg_1(arg_1), arg_2(arg_2) {
            if (arg_1.type != token::token_type::VARIABLE)
                throw std::runtime_error("Attempted to construct a `type_cmd_node` without a `<variable>` argument");
        }

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `write` command.
     * @details Construction: `write image <expr> to <string>`
     *
     */
    struct write_cmd_node : public cmd_node {
        /**
         * @brief The `<expr>` node argument.
         *
         */
        const std::shared_ptr<expr_node> arg_1;

        /**
         * @brief The `<string>` node argument.
         *
         */
        const token::string_token arg_2;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::WRITE_CMD`.
         *
         * @param arg_1 The first (`<expr>`) argument.
         * @param arg_2 The second (`<string>`) argument.
         */
        write_cmd_node(const std::shared_ptr<expr_node>& arg_1, const token::string_token& arg_2)
            : cmd_node(node_type::WRITE_CMD), arg_1(arg_1), arg_2(arg_2) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    //  Expressions:
    //  ------------
    /**
     * @brief The `false` expression.
     *
     */
    struct false_expr_node : public expr_node {
        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::FALSE_EXPR`.
         *
         */
        false_expr_node() : expr_node(node_type::FALSE_EXPR) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `<float>` expression.
     *
     */
    struct float_expr_node : public expr_node {
        /**
         * @brief The `<float>` value.
         *
         */
        double value;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::FLOAT_EXPR`.
         *
         * @param arg_1 The first (`<float>`) argument.
         */
        float_expr_node(const token::token& arg_1) : expr_node(node_type::FLOAT_EXPR) {
            if (arg_1.type != token::token_type::FLOATVAL)
                throw std::runtime_error("Attempted to construct a `float_expr_node` without a `<float>` argument");
            try {
                value = std::stod(arg_1.text);
            } catch (const std::exception& err) { throw std::runtime_error("Floating-point literal overflow"); }
        }

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `<integer>` expression.
     *
     */
    struct integer_expr_node : public expr_node {
        /**
         * @brief The `<integer>` value.
         *
         */
        long value;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::INTEGER_EXPR`.
         *
         * @param arg_1 The first (`<integer>`) argument.
         */
        integer_expr_node(const token::token& arg_1) : expr_node(node_type::INTEGER_EXPR) {
            if (arg_1.type != token::token_type::INTVAL)
                throw std::runtime_error("Attempted to construct an `int_expr_node` without an `<integer>` argument");
            try {
                value = std::stol(arg_1.text);
            } catch (const std::exception& err) { throw std::runtime_error("Integer literal overflow"); }
        }

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `true` expression.
     *
     */
    struct true_expr_node : public expr_node {
        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::TRUE_EXPR`.
         *
         */
        true_expr_node() : expr_node(node_type::TRUE_EXPR) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `<variable>` expression.
     *
     */
    struct variable_expr_node : public expr_node {
        /**
         * @brief The `<variable>` argument.
         *
         */
        const token::token arg_1;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::VARIABLE_EXPR`.
         *
         * @param arg_1 The first (`<variable>`) argument.
         */
        variable_expr_node(const token::token& arg_1) : expr_node(node_type::VARIABLE_EXPR), arg_1(arg_1) {
            if (arg_1.type != token::token_type::VARIABLE)
                throw std::runtime_error(
                        "Attempted to construct a `variable_expr_node` without a `<variable>` argument");
        }

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    //  Lvalues:
    //  --------
    /**
     * @brief The `<argument>` lvalue.
     *
     */
    struct argument_lvalue_node : public lvalue_node {
        /**
         * @brief The `<argument>` argument.
         *
         */
        const std::shared_ptr<argument_node> arg_1;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::ARGUMENT_LVALUE`.
         *
         * @param arg_1 The first (`<argument>`) argument.
         */
        argument_lvalue_node(const std::shared_ptr<argument_node>& arg_1)
            : lvalue_node(node_type::ARGUMENT_LVALUE), arg_1(arg_1) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    //  Types:
    //  ------
    /**
     * @brief The `bool` type.
     *
     */
    struct bool_type_node : public type_node {
        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::BOOL_TYPE`.
         *
         */
        bool_type_node() : type_node(node_type::BOOL_TYPE) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `float` type.
     *
     */
    struct float_type_node : public type_node {
        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::FLOAT_TYPE`.
         *
         */
        float_type_node() : type_node(node_type::FLOAT_TYPE) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `int` type.
     *
     */
    struct int_type_node : public type_node {
        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::INT_TYPE`.
         *
         */
        int_type_node() : type_node(node_type::INT_TYPE) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `<variable>` type.
     *
     */
    struct variable_type_node : public type_node {
        /**
         * @brief The `<variable>` argument.
         *
         */
        const token::token arg_1;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::VARIABLE_TYPE`.
         *
         * @param arg_1 The first (`<variable>`) argument.
         */
        variable_type_node(const token::token& arg_1) : type_node(node_type::VARIABLE_TYPE), arg_1(arg_1) {
            if (arg_1.type != token::token_type::VARIABLE)
                throw std::runtime_error(
                        "Attempted to construct a `variable_type_node` without a `<variable>` argument");
        }

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /*
    ================
    ||  Methods:  ||
    ================
    */
    /**
     * @brief Returns the s-expression for the given AST node.
     * @details Performs the necessary type casting.
     *
     * @param node An AST node.
     * @return The s-expression for the given AST node.
     */
    std::string get_s_expression(const std::shared_ptr<ast_node>& node);
}  //  namespace ast_node

#endif
