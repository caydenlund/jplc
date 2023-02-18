/**
 * @file type_checker.cpp
 * @package Assignment 6
 * @author Cayden Lund (u1182408)
 * @brief Implements the `type_checker` methods.
 * @details See the header file for documentation.
 *
 */

#include "type_checker.hpp"

namespace type_checker {
    void check(const parser::node_list_t& nodes) {
        scope_t global_scope;

        //  TODO (HW7): Remove this hardcoded initial state.
        //  TODO (HW8): Add `args` and `argnum` to the symbol table.
        const std::shared_ptr<resolved_type::resolved_type> float_type = std::make_shared<resolved_type::resolved_type>(
                resolved_type::FLOAT_TYPE);
        const std::vector<std::shared_ptr<resolved_type::resolved_type>> floats = {float_type, float_type, float_type,
                                                                                   float_type};
        const std::shared_ptr<resolved_type::resolved_type> element_type
                = std::make_shared<resolved_type::tuple_resolved_type>(floats);
        const std::shared_ptr<resolved_type::array_resolved_type> array_r_type
                = std::make_shared<resolved_type::array_resolved_type>(element_type, 2);
        array_r_type->type = resolved_type::ARRAY_TYPE;
        global_scope["pict."] = array_r_type;

        for (const parser::node_ptr_t& node : nodes) {
            check_cmd(std::reinterpret_pointer_cast<ast_node::cmd_node>(node), global_scope);
        }
    }

    //  Superclasses:
    //  -------------

    void check_cmd(const std::shared_ptr<ast_node::cmd_node>& command, scope_t& global_scope) {
        switch (command->type) {
            case ast_node::node_type::ASSERT_CMD:
                check_cmd_assert(std::reinterpret_pointer_cast<ast_node::assert_cmd_node>(command), global_scope);
                break;
            case ast_node::node_type::FN_CMD:
                check_cmd_fn(std::reinterpret_pointer_cast<ast_node::fn_cmd_node>(command), global_scope);
                break;
            case ast_node::node_type::LET_CMD:
                check_cmd_let(std::reinterpret_pointer_cast<ast_node::let_cmd_node>(command), global_scope);
                break;
            case ast_node::node_type::PRINT_CMD:
                check_cmd_print(std::reinterpret_pointer_cast<ast_node::print_cmd_node>(command), global_scope);
                break;
            case ast_node::node_type::READ_CMD:
                check_cmd_read(std::reinterpret_pointer_cast<ast_node::read_cmd_node>(command), global_scope);
                break;
            case ast_node::node_type::SHOW_CMD:
                check_cmd_show(std::reinterpret_pointer_cast<ast_node::show_cmd_node>(command), global_scope);
                break;
            case ast_node::node_type::TIME_CMD:
                check_cmd_time(std::reinterpret_pointer_cast<ast_node::time_cmd_node>(command), global_scope);
                break;
            case ast_node::node_type::TYPE_CMD:
                check_cmd_type(std::reinterpret_pointer_cast<ast_node::type_cmd_node>(command), global_scope);
                break;
            case ast_node::node_type::WRITE_CMD:
                check_cmd_write(std::reinterpret_pointer_cast<ast_node::write_cmd_node>(command), global_scope);
                break;
            default:
                throw std::runtime_error("Tried to call `type_checker::check()` on non-command node");
        }
    }

    std::shared_ptr<resolved_type::resolved_type> check_expr(const std::shared_ptr<ast_node::expr_node>& expression,
                                                             const scopes_t& scopes) {
        switch (expression->type) {
            case ast_node::node_type::ARRAY_INDEX_EXPR:
                return check_expr_array_index(
                        std::reinterpret_pointer_cast<ast_node::array_index_expr_node>(expression), scopes);
            case ast_node::node_type::ARRAY_LITERAL_EXPR:
                return check_expr_array_literal(
                        std::reinterpret_pointer_cast<ast_node::array_literal_expr_node>(expression), scopes);
            case ast_node::node_type::ARRAY_LOOP_EXPR:
                return check_expr_array_loop(std::reinterpret_pointer_cast<ast_node::array_loop_expr_node>(expression),
                                             scopes);
            case ast_node::node_type::BINOP_EXPR:
                return check_expr_binop(std::reinterpret_pointer_cast<ast_node::binop_expr_node>(expression), scopes);
            case ast_node::node_type::CALL_EXPR:
                return check_expr_call(std::reinterpret_pointer_cast<ast_node::call_expr_node>(expression), scopes);
            case ast_node::node_type::FALSE_EXPR:
                return check_expr_false(std::reinterpret_pointer_cast<ast_node::false_expr_node>(expression), scopes);
            case ast_node::node_type::FLOAT_EXPR:
                return check_expr_float(std::reinterpret_pointer_cast<ast_node::float_expr_node>(expression), scopes);
            case ast_node::node_type::IF_EXPR:
                return check_expr_if(std::reinterpret_pointer_cast<ast_node::if_expr_node>(expression), scopes);
            case ast_node::node_type::INTEGER_EXPR:
                return check_expr_integer(std::reinterpret_pointer_cast<ast_node::integer_expr_node>(expression),
                                          scopes);
            case ast_node::node_type::SUM_LOOP_EXPR:
                return check_expr_sum_loop(std::reinterpret_pointer_cast<ast_node::sum_loop_expr_node>(expression),
                                           scopes);
            case ast_node::node_type::TRUE_EXPR:
                return check_expr_true(std::reinterpret_pointer_cast<ast_node::true_expr_node>(expression), scopes);
            case ast_node::node_type::TUPLE_INDEX_EXPR:
                return check_expr_tuple_index(
                        std::reinterpret_pointer_cast<ast_node::tuple_index_expr_node>(expression), scopes);
            case ast_node::node_type::TUPLE_LITERAL_EXPR:
                return check_expr_tuple_literal(
                        std::reinterpret_pointer_cast<ast_node::tuple_literal_expr_node>(expression), scopes);
            case ast_node::node_type::UNOP_EXPR:
                return check_expr_unop(std::reinterpret_pointer_cast<ast_node::unop_expr_node>(expression), scopes);
            case ast_node::node_type::VARIABLE_EXPR:
                return check_expr_variable(std::reinterpret_pointer_cast<ast_node::variable_expr_node>(expression),
                                           scopes);
            default:
                throw std::runtime_error("Tried to call `type_checker::check_expression()` on non-expression node");
        }
    }

    void check_stmt(const std::shared_ptr<ast_node::stmt_node>&, const scopes_t&) {
        //  TODO (HW8): Implement.
    }

    /*
    ===================
    ||  Subclasses:  ||
    ===================
    */

    //  Commands:
    //  ---------

    void check_cmd_assert(const std::shared_ptr<ast_node::assert_cmd_node>& command, scope_t& global_scope) {
        if (check_expr(command->arg_1, {global_scope})->type != resolved_type::BOOL_TYPE)
            throw type_check_exception("`check_cmd_assert`: non-boolean expression: " + command->arg_1->s_expression());
    }

    void check_cmd_fn(const std::shared_ptr<ast_node::fn_cmd_node>&, scope_t&) {
        //  TODO (HW8): Implement.
    }

    void check_cmd_let(const std::shared_ptr<ast_node::let_cmd_node>&, scope_t&) {
        //  TODO (HW7): Implement.
    }

    void check_cmd_print(const std::shared_ptr<ast_node::print_cmd_node>&, scope_t&) {
        //  There are no expressions in a `print` command. No type-checking to be done here.
    }

    void check_cmd_read(const std::shared_ptr<ast_node::read_cmd_node>&, scope_t&) {
        //  TODO (HW7): Implement.
    }

    void check_cmd_show(const std::shared_ptr<ast_node::show_cmd_node>& command, scope_t& global_scope) {
        check_expr(command->arg_1, {global_scope});
    }

    void check_cmd_time(const std::shared_ptr<ast_node::time_cmd_node>& command, scope_t& global_scope) {
        check_cmd(command->command, {global_scope});
    }

    void check_cmd_type(const std::shared_ptr<ast_node::type_cmd_node>&, scope_t&) {
        //  TODO (HW8): Implement.
    }

    void check_cmd_write(const std::shared_ptr<ast_node::write_cmd_node>& command, scope_t& global_scope) {
        const std::shared_ptr<resolved_type::resolved_type> expr_type = check_expr(command->arg_1, {global_scope});
        if (expr_type->type != resolved_type::ARRAY_TYPE)
            throw type_check_exception("`check_cmd_write`: not an array: " + command->arg_1->s_expression());

        const std::shared_ptr<resolved_type::array_resolved_type> array_type
                = std::reinterpret_pointer_cast<resolved_type::array_resolved_type>(expr_type);
        if (array_type->rank != 2)
            throw type_check_exception("`check_cmd_write`: not a rank-2 array: " + command->arg_1->s_expression());

        const std::shared_ptr<resolved_type::resolved_type> float_type = std::make_shared<resolved_type::resolved_type>(
                resolved_type::FLOAT_TYPE);
        const std::vector<std::shared_ptr<resolved_type::resolved_type>> four_floats = {float_type, float_type,
                                                                                        float_type, float_type};
        const std::shared_ptr<resolved_type::resolved_type> pixel_type
                = std::make_shared<resolved_type::tuple_resolved_type>(four_floats);
        if (*(array_type->element_type) != *pixel_type)
            throw type_check_exception("`check_cmd_write`: not a rank-2 array of {float, float, float, float}: "
                                       + command->arg_1->s_expression());
    }

    //  Expressions:
    //  ------------

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_array_index(const std::shared_ptr<ast_node::array_index_expr_node>& expression, const scopes_t& scopes) {
        const std::shared_ptr<resolved_type::resolved_type> array_type = check_expr(expression->array, scopes);
        if (array_type->type != resolved_type::ARRAY_TYPE)
            throw type_check_exception("`check_expr_array_index`: indexing a non-array: " + expression->s_expression());

        const std::shared_ptr<resolved_type::array_resolved_type> full_array_type
                = std::reinterpret_pointer_cast<resolved_type::array_resolved_type>(array_type);
        if (full_array_type->rank != expression->params.size())
            throw type_check_exception("`check_expr_array_index`: incorrect number of index args: "
                                       + expression->s_expression());

        for (const std::shared_ptr<ast_node::expr_node>& param : expression->params) {
            if (check_expr(param, scopes)->type != resolved_type::INT_TYPE)
                throw type_check_exception("`check_expr_array_index`: index arg not an integer: "
                                           + expression->s_expression());
        }

        const std::shared_ptr<resolved_type::resolved_type> r_type = full_array_type->element_type;

        expression->r_type = r_type;

        return r_type;
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_array_literal(const std::shared_ptr<ast_node::array_literal_expr_node>& expression,
                             const scopes_t& scopes) {
        if (expression->expressions.empty())
            throw type_check_exception("`check_expr_array_literal`: empty arrays not allowed");
        const std::shared_ptr<resolved_type::resolved_type> element_type = check_expr(expression->expressions[0],
                                                                                      scopes);
        for (unsigned int index = 1; index < (unsigned int)expression->expressions.size(); index++) {
            const std::shared_ptr<resolved_type::resolved_type> type = check_expr(expression->expressions[index],
                                                                                  scopes);
            if (*element_type != *type)
                throw type_check_exception("`check_expr_array_literal`: type mismatch: " + expression->s_expression());
        }

        std::shared_ptr<resolved_type::resolved_type> r_type = std::make_shared<resolved_type::array_resolved_type>(
                element_type, 1);
        expression->r_type = r_type;

        return r_type;
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_array_loop(const std::shared_ptr<ast_node::array_loop_expr_node>&, const scopes_t&) {
        //  TODO (HW7): Implement.

        return {};
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_binop(const std::shared_ptr<ast_node::binop_expr_node>& expression, const scopes_t& scopes) {
        const std::shared_ptr<resolved_type::resolved_type> left_type = check_expr(expression->left_operand, scopes);
        const std::shared_ptr<resolved_type::resolved_type> right_type = check_expr(expression->right_operand, scopes);

        if (*left_type != *right_type)
            throw type_check_exception("`check_expr_binop`: mismatched types: " + expression->s_expression());

        std::shared_ptr<resolved_type::resolved_type> r_type;
        switch (expression->type) {
            case ast_node::BINOP_PLUS:
            case ast_node::BINOP_MINUS:
            case ast_node::BINOP_TIMES:
            case ast_node::BINOP_DIVIDE:
            case ast_node::BINOP_MOD:
                if (left_type->type != resolved_type::INT_TYPE && left_type->type != resolved_type::FLOAT_TYPE)
                    throw type_check_exception("`check_expr_binop`: not valid operands: " + expression->s_expression());
                r_type = left_type;
                break;
            case ast_node::BINOP_LT:
            case ast_node::BINOP_GT:
            case ast_node::BINOP_EQ:
            case ast_node::BINOP_NEQ:
            case ast_node::BINOP_LEQ:
            case ast_node::BINOP_GEQ:
                if (left_type->type != resolved_type::BOOL_TYPE && left_type->type != resolved_type::INT_TYPE
                    && left_type->type != resolved_type::FLOAT_TYPE)
                    throw type_check_exception("`check_expr_binop`: not valid operands: " + expression->s_expression());
                r_type = std::make_shared<resolved_type::resolved_type>(resolved_type::BOOL_TYPE);
                break;
            case ast_node::BINOP_AND:
            case ast_node::BINOP_OR:
                if (left_type->type != resolved_type::BOOL_TYPE)
                    throw type_check_exception("`check_expr_binop`: not valid operands: " + expression->s_expression());
                r_type = left_type;
                break;
            default:
                throw type_check_exception("`check_expr_binop`: not a valid operator: " + expression->s_expression());
        }

        expression->r_type = r_type;

        return r_type;
    }

    std::shared_ptr<resolved_type::resolved_type> check_expr_call(const std::shared_ptr<ast_node::call_expr_node>&,
                                                                  const scopes_t&) {
        //  TODO (HW7): Implement.

        return {};
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_false(const std::shared_ptr<ast_node::false_expr_node>& expression, const scopes_t&) {
        const std::shared_ptr<resolved_type::resolved_type> bool_type = std::make_shared<resolved_type::resolved_type>(
                resolved_type::BOOL_TYPE);

        expression->r_type = bool_type;

        return bool_type;
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_float(const std::shared_ptr<ast_node::float_expr_node>& expression, const scopes_t&) {
        const std::shared_ptr<resolved_type::resolved_type> float_type = std::make_shared<resolved_type::resolved_type>(
                resolved_type::FLOAT_TYPE);

        expression->r_type = float_type;

        return float_type;
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_if(const std::shared_ptr<ast_node::if_expr_node>& expression, const scopes_t& scopes) {
        const std::shared_ptr<resolved_type::resolved_type> conditional_type = check_expr(expression->conditional_expr,
                                                                                          scopes);
        if (conditional_type->type != resolved_type::BOOL_TYPE)
            throw type_check_exception("`check_expr_if`: condition not a boolean: "
                                       + expression->conditional_expr->s_expression());

        const std::shared_ptr<resolved_type::resolved_type> affirmative_type = check_expr(expression->affirmative_expr,
                                                                                          scopes);
        const std::shared_ptr<resolved_type::resolved_type> negative_type = check_expr(expression->negative_expr,
                                                                                       scopes);

        if (*affirmative_type != *negative_type)
            throw type_check_exception("`check_expr_if`: type mismatch: " + expression->s_expression());

        expression->r_type = affirmative_type;

        return affirmative_type;
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_integer(const std::shared_ptr<ast_node::integer_expr_node>& expression, const scopes_t&) {
        const std::shared_ptr<resolved_type::resolved_type> int_type = std::make_shared<resolved_type::resolved_type>(
                resolved_type::INT_TYPE);

        expression->r_type = int_type;

        return int_type;
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_sum_loop(const std::shared_ptr<ast_node::sum_loop_expr_node>&, const scopes_t&) {
        //  TODO (HW7): Implement.

        return {};
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_true(const std::shared_ptr<ast_node::true_expr_node>& expression, const scopes_t&) {
        const std::shared_ptr<resolved_type::resolved_type> bool_type = std::make_shared<resolved_type::resolved_type>(
                resolved_type::BOOL_TYPE);

        expression->r_type = bool_type;

        return bool_type;
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_tuple_index(const std::shared_ptr<ast_node::tuple_index_expr_node>& expression, const scopes_t& scopes) {
        const std::shared_ptr<resolved_type::resolved_type> tuple_type = check_expr(expression->expr, scopes);
        if (tuple_type->type != resolved_type::TUPLE_TYPE)
            throw type_check_exception("`check_expr_tuple_index`: indexing a non-tuple: " + expression->s_expression());

        const std::shared_ptr<resolved_type::tuple_resolved_type> full_tuple_type
                = std::reinterpret_pointer_cast<resolved_type::tuple_resolved_type>(tuple_type);

        const unsigned int index = expression->index->value;
        if (index >= full_tuple_type->element_types.size())
            throw type_check_exception("`check_expr_tuple_index`: out-of-bounds: " + expression->s_expression());

        const std::shared_ptr<resolved_type::resolved_type> r_type = full_tuple_type->element_types[index];

        expression->r_type = r_type;

        return r_type;
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_tuple_literal(const std::shared_ptr<ast_node::tuple_literal_expr_node>& expression,
                             const scopes_t& scopes) {
        std::vector<std::shared_ptr<resolved_type::resolved_type>> r_types;

        for (const std::shared_ptr<ast_node::expr_node>& expr : expression->exprs) {
            r_types.push_back(check_expr(expr, scopes));
        }

        const std::shared_ptr<resolved_type::resolved_type> r_type
                = std::make_shared<resolved_type::tuple_resolved_type>(r_types);

        expression->r_type = r_type;

        return r_type;
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_unop(const std::shared_ptr<ast_node::unop_expr_node>& expression, const scopes_t& scopes) {
        const std::shared_ptr<resolved_type::resolved_type> r_type = check_expr(expression->operand, scopes);

        if (expression->type == ast_node::op_type::UNOP_NEG && r_type->type != resolved_type::INT_TYPE
            && r_type->type != resolved_type::FLOAT_TYPE)
            throw type_check_exception("`check_expr_unop`: not a number: " + expression->s_expression());

        if (expression->type == ast_node::op_type::UNOP_INV && r_type->type != resolved_type::BOOL_TYPE)
            throw type_check_exception("`check_expr_unop`: not a boolean: " + expression->s_expression());

        expression->r_type = r_type;

        return r_type;
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_variable(const std::shared_ptr<ast_node::variable_expr_node>& expression, const scopes_t& scopes) {
        std::shared_ptr<resolved_type::resolved_type> r_type;
        const std::string variable = expression->arg_1.text;

        for (const scope_t& scope : scopes) {
            if (scope.count(variable) > 0) {
                r_type = scope.at(variable);
                break;
            }
        }

        if (r_type == nullptr)
            throw type_check_exception("`check_expr_variable`: no variable in scope: " + expression->s_expression());

        expression->r_type = r_type;

        return r_type;
    }
}  // namespace type_checker
