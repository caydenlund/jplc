/**
 * @file ast_node.hpp
 * @package Assignments 3-5
 * @author Cayden Lund (u1182408)
 * @brief Defines the AST node classes.
 *
 */

#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

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
        ARRAY_ARGUMENT,     //  AST node `array_argument_node`.
        VARIABLE_ARGUMENT,  //  AST node `variable_argument_node`.

        //  =================
        //  ||  Bindings:  ||
        //  =================
        BINDING,        //  AST node `binding_node`.
        TUPLE_BINDING,  //  AST node `tuple_binding_node`.
        VAR_BINDING,    //  AST node `var_binding_node`.

        //  =================
        //  ||  Commands:  ||
        //  =================
        CMD,         //  AST node `cmd_node`.
        ASSERT_CMD,  //  AST node `assert_cmd_node`.
        FN_CMD,      //  AST node `fn_cmd_node`.
        LET_CMD,     //  AST node `let_cmd_node`.
        PRINT_CMD,   //  AST node `print_cmd_node`.
        READ_CMD,    //  AST node `read_cmd_node`.
        SHOW_CMD,    //  AST node `show_cmd_node`.
        TIME_CMD,    //  AST node `time_cmd_node`.
        TYPE_CMD,    //  AST node `type_cmd_node`.
        WRITE_CMD,   //  AST node `write_cmd_node`.

        //  ====================
        //  ||  Expressions:  ||
        //  ====================
        EXPR,                //  AST node `expr_node`.
        ARRAY_INDEX_EXPR,    //  AST node `array_index_expr_node`.
        ARRAY_LITERAL_EXPR,  //  AST node `array_literal_expr_node`.
        ARRAY_LOOP_EXPR,     //  AST node `array_loop_expr_node`.
        BINOP_EXPR,          //  AST node `binop_expr_node`.
        CALL_EXPR,           //  AST node `call_expr_node`.
        FALSE_EXPR,          //  AST node `false_expr_node`.
        FLOAT_EXPR,          //  AST node `float_expr_node`.
        IF_EXPR,             //  AST node `if_expr_node`.
        INTEGER_EXPR,        //  AST node `integer_expr_node`.
        SUM_LOOP_EXPR,       //  AST node `sum_loop_expr_node`.
        TRUE_EXPR,           //  AST node `true_expr_node`.
        TUPLE_INDEX_EXPR,    //  AST node `tuple_index_expr_node`.
        TUPLE_LITERAL_EXPR,  //  AST node `tuple_literal_expr_node`.
        UNOP_EXPR,           //  AST node `unop_expr_node`.
        VARIABLE_EXPR,       //  AST node `variable_expr_node`.

        //  ================
        //  ||  Lvalues:  ||
        //  ================
        LVALUE,           //  AST node `lvalue_node`.
        ARGUMENT_LVALUE,  //  AST node `argument_lvalue_node`.
        TUPLE_LVALUE,     //  AST node `tuple_lvalue_node`.

        //  ===================
        //  ||  Statements:  ||
        //  ===================
        STMT,         //  AST node `stmt_node`.
        ASSERT_STMT,  //  AST node `assert_stmt_node`.
        LET_STMT,     //  AST node `let_stmt_node`.
        RETURN_STMT,  //  AST node `return_stmt_node`.

        //  ==============
        //  ||  Types:  ||
        //  ==============
        TYPE,          //  AST node `type_node`.
        ARRAY_TYPE,    //  AST node `array_type_node`.
        BOOL_TYPE,     //  AST node `bool_type_node`.
        FLOAT_TYPE,    //  AST node `float_type_node`.
        INT_TYPE,      //  AST node `int_type_node`.
        TUPLE_TYPE,    //  AST node `tuple_type_node`.
        VARIABLE_TYPE  //  AST node `variable_type_node`.
    };

    /**
     * @brief An enumeration of the different binary operators.
     *
     */
    enum binop_type {
        PLUS,    //  "+"
        MINUS,   //  "-"
        TIMES,   //  "*"
        DIVIDE,  //  "/"
        MOD,     //  "%"
        LT,      //  "<"
        GT,      //  ">"
        EQ,      //  "=="
        NEQ,     //  "!="
        LEQ,     //  "<="
        GEQ,     //  ">="
        AND,     //  "&&"
        OR       //  "||"
    };

    /**
     * @brief An enumeration of the different unary operators.
     *
     */
    enum unop_type {
        INV,  // "!"
        NEG   // "-"
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
     * @brief The base `binding` class from which all other `binding` AST nodes inherit.
     *
     */
    struct binding_node : public ast_node {
        /**
         * @brief Class constructor.
         * @details Initializes `type` to the given type.
         *
         * @param type The type of this AST node. Defaults to `node_type::BINDING`.
         */
        binding_node(node_type type = node_type::BINDING) : ast_node(type) {}

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
     * @brief The base `stmt` class from which all other `stmt` AST nodes inherit.
     *
     */
    struct stmt_node : public ast_node {
        /**
         * @brief Class constructor.
         * @details Initializes `type` to the given type.
         *
         * @param type The type of this AST node. Defaults to `node_type::STMT`.
         */
        stmt_node(node_type type = node_type::STMT) : ast_node(type) {}

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
     * @brief The `<variable>[<variable>, ...]` argument (array argument).
     * @defines Example: in `fn blur_one(img[H, W] : pict)`, `img[H, W]` is an array argument.
     *
     */
    struct array_argument_node : public argument_node {
        /**
         * @brief The main array variable.
         *
         */
        const token::token main_var;

        /**
         * @brief The set of dimension variables.
         *
         */
        const std::vector<token::token> vars;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::ARRAY_ARGUMENT`.
         *
         * @param main_var The main array variable.
         * @param vars The set of variable dimensions for the main variable.
         */
        array_argument_node(const token::token main_var, const std::vector<token::token> vars)
            : argument_node(node_type::ARRAY_ARGUMENT), main_var(main_var), vars(vars) {
            if (main_var.type != token::token_type::VARIABLE)
                throw std::runtime_error(
                        "Attempted to construct an `array_argument_node` without a `<variable>` token");
            for (const token::token& tok : vars) {
                if (tok.type != token::token_type::VARIABLE)
                    throw std::runtime_error("Attempted to construct an `array_argument_node` without a valid set of "
                                             "`<variable>` tokens");
            }
        }

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

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

    //  Bindings:
    //  ---------
    /**
     * @brief The `{<binding>, ...}` binding (for tuples).
     *
     */
    struct tuple_binding_node : public binding_node {
        /**
         * @brief The sequence of bindings contained in this tuple.
         *
         */
        const std::vector<std::shared_ptr<binding_node>> args;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::TUPLE_BINDING`.
         *
         * @param args The binding arguments in the tuple sequence.
         */
        tuple_binding_node(const std::vector<std::shared_ptr<binding_node>> args)
            : binding_node(node_type::TUPLE_BINDING), args(args) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `<argument>: <type>` binding.
     *
     */
    struct var_binding_node : public binding_node {
        /**
         * @brief The `<argument>` argument.
         *
         */
        const std::shared_ptr<argument_node> arg_arg;

        /**
         * @brief The `<type>` argument.
         *
         */
        const std::shared_ptr<type_node> type_arg;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::VAR_BINDING`.
         *
         * @param arg_arg The `<argument>` of the binding.
         * @param type_arg The `<type>` of the binding.
         */
        var_binding_node(const std::shared_ptr<argument_node> arg_arg, const std::shared_ptr<type_node> type_arg)
            : binding_node(node_type::VAR_BINDING), arg_arg(arg_arg), type_arg(type_arg) {}

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
     * @brief The `fn <variable> (<binding>, ...) : <type> {; <stmt>; ...; }` command (for functions).
     * @details Above, the `;` represents the newline token.
     *
     */
    struct fn_cmd_node : public cmd_node {
        /**
         * @brief The `<variable>` argument.
         *
         */
        const token::token name;

        /**
         * @brief The set of `<binding>` arguments.
         *
         */
        const std::vector<std::shared_ptr<binding_node>> bindings;

        /**
         * @brief The `<type>` argument.
         *
         */
        const std::shared_ptr<type_node> return_type;

        /**
         * @brief The `<stmt>` argument.
         *
         */
        const std::vector<std::shared_ptr<stmt_node>> statements;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::FN_CMD`.
         *
         * @param name The function name (variable token).
         * @param bindings The function input (set of bindings).
         * @param return_type The function return type (type node).
         * @param statements The function body (list of statements).
         */
        fn_cmd_node(const token::token name, const std::vector<std::shared_ptr<binding_node>> bindings,
                    const std::shared_ptr<type_node> return_type,
                    const std::vector<std::shared_ptr<stmt_node>> statements)
            : cmd_node(node_type::FN_CMD), name(name), bindings(bindings), return_type(return_type),
              statements(statements) {
            if (name.type != token::token_type::VARIABLE)
                throw std::runtime_error("Attempted to construct a `fn_cmd_node` without a `<variable>` argument");
        }

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
     * @brief The `time <cmd>` command.
     *
     */
    struct time_cmd_node : public cmd_node {
        /**
         * @brief The command to time.
         *
         */
        const std::shared_ptr<cmd_node> command;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::TIME_CMD`.
         *
         * @param command The command to time.
         */
        time_cmd_node(const std::shared_ptr<cmd_node> command) : cmd_node(node_type::TIME_CMD), command(command) {}

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
     * @brief The `<expr> [<expr>, ...]` expression (for array indexing).
     *
     */
    struct array_index_expr_node : public expr_node {
        /**
         * @brief The array to access.
         *
         */
        const std::shared_ptr<expr_node> array;

        /**
         * @brief The set of subscript parameters.
         *
         */
        const std::vector<std::shared_ptr<expr_node>> params;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::ARRAY_INDEX_EXPR`.
         *
         * @param array The array to access.
         * @param params The subscript parameters as a set of expression nodes.
         */
        array_index_expr_node(const std::shared_ptr<expr_node> array,
                              const std::vector<std::shared_ptr<expr_node>> params)
            : expr_node(node_type::ARRAY_INDEX_EXPR), array(array), params(params) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `array[<variable> : <expr>, ...] <expr>` expression.
     *
     */
    struct array_loop_expr_node : public expr_node {
        /**
         * @brief Defines a single `<variable> : <expr>` ordered pair.
         *
         */
        using binding_pair_t = std::tuple<token::token, std::shared_ptr<expr_node>>;

        /**
         * @brief Contains all the `<variable> : <expr>` ordered pairs.
         *
         */
        const std::vector<binding_pair_t> binding_pairs;

        /**
         * @brief Defines the expression to evaluate for every combination of the bound variables.
         *
         */
        const std::shared_ptr<expr_node> item_expr;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::ARRAY_LOOP_EXPR`.
         *
         * @param binding_pairs All the `<variable> : <expr>` bindings.
         * @param item_expr The expression to evaluate and store for every combination of the bound variables.
         */
        array_loop_expr_node(std::vector<binding_pair_t> binding_pairs, const std::shared_ptr<expr_node> item_expr)
            : expr_node(node_type::ARRAY_LOOP_EXPR), binding_pairs(binding_pairs), item_expr(item_expr) {
            for (const binding_pair_t& binding : binding_pairs) {
                if (std::get<0>(binding).type != token::token_type::VARIABLE)
                    throw std::runtime_error(
                            "Attempted to construct an `array_loop_expr_node` without a `<variable>` argument");
            }
        }

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `[<expr, ...]` expression (for array literals).
     *
     */
    struct array_literal_expr_node : public expr_node {
        /**
         * @brief The set of expressions composing the array literal.
         *
         */
        const std::vector<std::shared_ptr<expr_node>> expressions;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::ARRAY_LITERAL_EXPR`.
         *
         * @param expressions The set of expressions composing the array literal.
         */
        array_literal_expr_node(const std::vector<std::shared_ptr<expr_node>> expressions)
            : expr_node(node_type::ARRAY_LITERAL_EXPR), expressions(expressions) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `<expr> <binop> <expr>` expression.
     * @details `<binop>` is one of the following:
     *     - `"+"`
     *     - `"-"`
     *     - `"*"`
     *     - `"/"`
     *     - `"%"`
     *     - `"<"`
     *     - `">"`
     *     - `"=="`
     *     - `"!="`
     *     - `"<="`
     *     - `">="`
     *     - `"&&"`
     *     - `"||"`
     *
     */
    struct binop_expr_node : public expr_node {
        /**
         * @brief This node's type.
         *
         */
        binop_type type;

        /**
         * @brief The left-hand side operand of the operator.
         *
         */
        const std::shared_ptr<expr_node> left_operand;

        /**
         * @brief The right-hand side operand of the operator.
         *
         */
        const std::shared_ptr<expr_node> right_operand;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::BINOP_EXPR`.
         *
         * @param binop The operator token that creates this expression.
         * @param left_operand The left-hand side operand of the operator.
         * @param right_operand The right-hand side operand of the operator.
         */
        binop_expr_node(const token::token binop, const std::shared_ptr<expr_node> left_operand,
                        const std::shared_ptr<expr_node> right_operand)
            : expr_node(node_type::BINOP_EXPR), left_operand(left_operand), right_operand(right_operand) {
            if (binop.type != token::token_type::OP)
                throw std::runtime_error("Attempted to construct a `binop_expr_node` without a `<binop>` argument");
            if (binop.text == "+") this->type = binop_type::PLUS;
            else if (binop.text == "-")
                this->type = binop_type::MINUS;
            else if (binop.text == "*")
                this->type = binop_type::TIMES;
            else if (binop.text == "/")
                this->type = binop_type::DIVIDE;
            else if (binop.text == "%")
                this->type = binop_type::MOD;
            else if (binop.text == "<")
                this->type = binop_type::LT;
            else if (binop.text == ">")
                this->type = binop_type::GT;
            else if (binop.text == "==")
                this->type = binop_type::EQ;
            else if (binop.text == "!=")
                this->type = binop_type::NEQ;
            else if (binop.text == "<=")
                this->type = binop_type::LEQ;
            else if (binop.text == ">=")
                this->type = binop_type::GEQ;
            else if (binop.text == "&&")
                this->type = binop_type::AND;
            else if (binop.text == "||")
                this->type = binop_type::OR;
            else
                throw std::runtime_error("Attempted to construct a `binop_expr_node` without a `<binop>` argument");
        }

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `<variable>(<expr>, ...)` expression (for function calls).
     *
     */
    struct call_expr_node : public expr_node {
        /**
         * @brief The function name.
         *
         */
        const token::token name;

        /**
         * @brief The set of function call arguments.
         *
         */
        const std::vector<std::shared_ptr<expr_node>> call_args;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::CALL_EXPR`.
         *
         * @param name The name of the function.
         * @param call_args The arguments in the function call.
         */
        call_expr_node(const token::token name, const std::vector<std::shared_ptr<expr_node>> call_args)
            : expr_node(node_type::CALL_EXPR), name(name), call_args(call_args) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

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
     * @brief The `if <expr> then <expr> else <expr>` expression.
     *
     */
    struct if_expr_node : public expr_node {
        /**
         * @brief The conditional expression.
         * @details "if THIS then . . .".
         *     Determines whether to evaluate the affirmative or the negative expression.
         *
         */
        const std::shared_ptr<expr_node> conditional_expr;

        /**
         * @brief The affirmative expression.
         * @details "if . . . then THIS else . . .".
         *     Evaluated when the condition is true.
         *
         */
        const std::shared_ptr<expr_node> affirmative_expr;

        /**
         * @brief The negative expression.
         * @details "if . . . then . . . else THIS".
         *     Evaluated when the conditional is false.
         *
         */
        const std::shared_ptr<expr_node> negative_expr;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::IF_EXPR`.
         *
         * @param conditional_expr The expression that determines whether to evaluate the affirmative or the negative
         *     expression.
         * @param affirmative_expr The expression that is evaluated when the condition is true.
         * @param negative_expr The expression that is evaluated when the condition is false.
         */
        if_expr_node(const std::shared_ptr<expr_node> conditional_expr,
                     const std::shared_ptr<expr_node> affirmative_expr, const std::shared_ptr<expr_node> negative_expr)
            : expr_node(node_type::IF_EXPR), conditional_expr(conditional_expr), affirmative_expr(affirmative_expr),
              negative_expr(negative_expr) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `sum[<variable> : <expr>, ...] <expr>` expression.
     *
     */
    struct sum_loop_expr_node : public expr_node {
        /**
         * @brief Defines a single `<variable> : <expr>` ordered pair.
         *
         */
        using binding_pair_t = std::tuple<token::token, std::shared_ptr<expr_node>>;

        /**
         * @brief Contains all the `<variable> : <expr>` ordered pairs.
         *
         */
        const std::vector<binding_pair_t> binding_pairs;

        /**
         * @brief Defines the expression to evaluate for every combination of the bound variables.
         *
         */
        const std::shared_ptr<expr_node> sum_expr;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::SUM_LOOP_EXPR`.
         *
         * @param binding_pairs All the `<variable> : <expr>` bindings.
         * @param sum_expr The expression to evaluate and sum for every combination of the bound variables.
         */
        sum_loop_expr_node(std::vector<binding_pair_t> binding_pairs, const std::shared_ptr<expr_node> sum_expr)
            : expr_node(node_type::SUM_LOOP_EXPR), binding_pairs(binding_pairs), sum_expr(sum_expr) {
            for (const binding_pair_t& binding : binding_pairs) {
                if (std::get<0>(binding).type != token::token_type::VARIABLE)
                    throw std::runtime_error(
                            "Attempted to construct a `sum_loop_expr_node` without a `<variable>` argument");
            }
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
     * @brief The `<expr> {<integer>}` expression (for tuple indexing).
     *
     */
    struct tuple_index_expr_node : public expr_node {
        /**
         * @brief The `<expr>` tuple argument.
         *
         */
        const std::shared_ptr<expr_node> expr;

        /**
         * @brief The `<integer>` index.
         *
         */
        std::shared_ptr<integer_expr_node> index;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::TUPLE_INDEX_EXPR`.
         *
         * @param expr The `<expr>` node tuple argument.
         * @param expr The `<integer>` node index.
         */
        tuple_index_expr_node(const std::shared_ptr<expr_node> expr, std::shared_ptr<integer_expr_node> index)
            : expr_node(node_type::TUPLE_INDEX_EXPR), expr(expr), index(index) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `{<expr>, ...}` expression (for tuple literals).
     *
     */
    struct tuple_literal_expr_node : public expr_node {
        /**
         * @brief The set of expressions that compose the tuple.
         *
         */
        const std::vector<std::shared_ptr<expr_node>> exprs;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::TUPLE_LITERAL_EXPR`.
         *
         * @param exprs The set of `expr_node`s that compose the tuple.
         */
        tuple_literal_expr_node(const std::vector<std::shared_ptr<expr_node>> exprs)
            : expr_node(node_type::TUPLE_LITERAL_EXPR), exprs(exprs) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `<unop> <expr>` expression.
     * @details `<unop>` is one of the following:
     *     - `"!"`
     *     - `"-"`
     *
     */
    struct unop_expr_node : public expr_node {
        /**
         * @brief This node's type.
         *
         */
        unop_type type;

        /**
         * @brief The operand of the operator.
         *
         */
        const std::shared_ptr<expr_node> operand;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::UNOP_EXPR`.
         *
         * @param unop The operator token that creates this expression.
         * @param operand The operand of this operator.
         */
        unop_expr_node(const token::token unop, const std::shared_ptr<expr_node> operand)
            : expr_node(node_type::UNOP_EXPR), operand(operand) {
            if (unop.type != token::token_type::OP)
                throw std::runtime_error("Attempted to construct a `unop_expr_node` without a `<unop>` argument");
            if (unop.text == "!") this->type = unop_type::INV;
            else if (unop.text == "-")
                this->type = unop_type::NEG;
            else
                throw std::runtime_error("Attempted to construct a `unop_expr_node` without a `<unop>` argument");
        }

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

    /**
     * @brief The `{<lvalue>, ...}` lvalue (for lvalue tuples).
     *
     */
    struct tuple_lvalue_node : public lvalue_node {
        /**
         * @brief The set of lvalues in the tuple.
         *
         */
        const std::vector<std::shared_ptr<lvalue_node>> lvalues;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::TUPLE_LVALUE`.
         *
         * @param lvalues The set of lvalues in the lvalue tuple.
         */
        tuple_lvalue_node(const std::vector<std::shared_ptr<lvalue_node>> lvalues)
            : lvalue_node(node_type::TUPLE_LVALUE), lvalues(lvalues) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    //  Statements:
    //  -----------
    /**
     * @brief The `assert <expr>, <string>` statement.
     *
     */
    struct assert_stmt_node : public stmt_node {
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
         * @details Initializes `type` to `node_type::ASSERT_STMT`.
         *
         * @param arg_1 The `<expr>` node argument.
         * @param arg_2 The `<string>` node argument.
         */
        assert_stmt_node(const std::shared_ptr<expr_node> arg_1, const token::string_token arg_2)
            : stmt_node(node_type::ASSERT_STMT), arg_1(arg_1), arg_2(arg_2) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `let <lvalue> = <expr>` statement.
     *
     */
    struct let_stmt_node : public stmt_node {
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
         * @details Initializes `type` to `node_type::LET_STMT`.
         *
         * @param arg_1 The first (`<lvalue>`) node.
         * @param arg_2 The second (`<expr>`) node.
         */
        let_stmt_node(const std::shared_ptr<lvalue_node> arg_1, const std::shared_ptr<expr_node> arg_2

                      )
            : stmt_node(node_type::LET_STMT), arg_1(arg_1), arg_2(arg_2) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

    /**
     * @brief The `return <expr>` statement.
     *
     */
    struct return_stmt_node : public stmt_node {
        /**
         * @brief The `<expr>` node return value.
         *
         */
        const std::shared_ptr<expr_node> return_val;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::RETURN_STMT`.
         *
         * @param return_val The return value.
         */
        return_stmt_node(const std::shared_ptr<expr_node> return_val)
            : stmt_node(node_type::RETURN_STMT), return_val(return_val) {}

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
     * @brief The `<type> [, ...]` type (for defining an array type).
     * @details Example: `type pict = color[,]` defines a 2D array of `color`s.
     *
     */
    struct array_type_node : public type_node {
        /**
         * @brief The main type of the array.
         *
         */
        const std::shared_ptr<type_node> main_type;

        /**
         * @brief The number of dimensions of the array.
         * @details The number of commas, plus one.
         *
         */
        const unsigned long dimensions;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::ARRAY_TYPE`.
         *
         * @param main_type The main type of the array.
         * @param dimensions The number of dimensions in the array.
         */
        array_type_node(const std::shared_ptr<type_node> main_type, const unsigned long dimensions)
            : type_node(node_type::ARRAY_TYPE), main_type(main_type), dimensions(dimensions) {}

        /**
         * @brief Returns the s-expression string for this AST node.
         *
         * @return The s-expression string for this AST node.
         */
        std::string s_expression() const override;
    };

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
     * @brief The `{<type, ...}` type (for type tuples).
     *
     */
    struct tuple_type_node : public type_node {
        /**
         * @brief The set of types in the tuple.
         *
         */
        const std::vector<std::shared_ptr<type_node>> types;

        /**
         * @brief Class constructor.
         * @details Initializes `type` to `node_type::TUPLE_TYPE`.
         *
         * @param types The set of types in the tuple.
         */
        tuple_type_node(const std::vector<std::shared_ptr<type_node>> types)
            : type_node(node_type::TUPLE_TYPE), types(types) {}

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
}  //  namespace ast_node

#endif
