/**
 * @file type_checker.cpp
 * @package Assignments 6-8
 * @author Cayden Lund (u1182408)
 * @brief Implements the `type_checker` methods.
 * @details See the header file for documentation.
 *
 */

#include "type_checker.hpp"

namespace type_checker {
    void check(const parser::node_list_t& nodes, bool initialize_hw6) {
        symbol_table::symbol_table global_symbol_table;

        //  Add the global symbol `args` and `argnum` to the symbol table.
        const std::shared_ptr<resolved_type::resolved_type> int_type = std::make_shared<resolved_type::resolved_type>(
                resolved_type::INT_TYPE);
        const std::shared_ptr<resolved_type::array_resolved_type> int_array_type
                = std::make_shared<resolved_type::array_resolved_type>(int_type, 1);
        global_symbol_table.add_symbol("argnum", std::make_shared<name_info::variable_info>(int_type));
        global_symbol_table.add_symbol("args", std::make_shared<name_info::variable_info>(int_array_type));

        //  This initialization is done for homework 6.
        //  It initializes the `pict.` variable to a rank-2 array of tuples of four floating-point numbers.
        //  I'm not removing this yet, because I still want to be able to go back and run HW6's tests to make sure that
        //  I haven't broken any of my previous work.
        //  TODO (HW7): Remove this initialization.
        if (initialize_hw6) {
            const std::shared_ptr<resolved_type::resolved_type> float_type
                    = std::make_shared<resolved_type::resolved_type>(resolved_type::FLOAT_TYPE);
            const std::vector<std::shared_ptr<resolved_type::resolved_type>> floats = {float_type, float_type,
                                                                                       float_type, float_type};
            const std::shared_ptr<resolved_type::resolved_type> element_type
                    = std::make_shared<resolved_type::tuple_resolved_type>(floats);
            const std::shared_ptr<resolved_type::array_resolved_type> array_r_type
                    = std::make_shared<resolved_type::array_resolved_type>(element_type, 2);
            array_r_type->type = resolved_type::ARRAY_TYPE;
            global_symbol_table.add_symbol("pict.", std::make_shared<name_info::variable_info>(array_r_type));
        }

        for (const parser::node_ptr_t& node : nodes) {
            check_cmd(std::reinterpret_pointer_cast<ast_node::cmd_node>(node), global_symbol_table);
        }
    }

    //  Superclasses:
    //  -------------

    void check_cmd(const std::shared_ptr<ast_node::cmd_node>& command, symbol_table::symbol_table& sym_tab) {
        switch (command->type) {
            case ast_node::node_type::ASSERT_CMD:
                check_cmd_assert(std::reinterpret_pointer_cast<ast_node::assert_cmd_node>(command), sym_tab);
                break;
            case ast_node::node_type::FN_CMD:
                check_cmd_fn(std::reinterpret_pointer_cast<ast_node::fn_cmd_node>(command), sym_tab);
                break;
            case ast_node::node_type::LET_CMD:
                check_cmd_let(std::reinterpret_pointer_cast<ast_node::let_cmd_node>(command), sym_tab);
                break;
            case ast_node::node_type::PRINT_CMD:
                check_cmd_print(std::reinterpret_pointer_cast<ast_node::print_cmd_node>(command), sym_tab);
                break;
            case ast_node::node_type::READ_CMD:
                check_cmd_read(std::reinterpret_pointer_cast<ast_node::read_cmd_node>(command), sym_tab);
                break;
            case ast_node::node_type::SHOW_CMD:
                check_cmd_show(std::reinterpret_pointer_cast<ast_node::show_cmd_node>(command), sym_tab);
                break;
            case ast_node::node_type::TIME_CMD:
                check_cmd_time(std::reinterpret_pointer_cast<ast_node::time_cmd_node>(command), sym_tab);
                break;
            case ast_node::node_type::TYPE_CMD:
                check_cmd_type(std::reinterpret_pointer_cast<ast_node::type_cmd_node>(command), sym_tab);
                break;
            case ast_node::node_type::WRITE_CMD:
                check_cmd_write(std::reinterpret_pointer_cast<ast_node::write_cmd_node>(command), sym_tab);
                break;
            default:
                throw std::runtime_error("Tried to call `type_checker::check()` on non-command node");
        }
    }

    std::shared_ptr<resolved_type::resolved_type> check_expr(const std::shared_ptr<ast_node::expr_node>& expression,
                                                             const symbol_table::symbol_table& sym_tab) {
        switch (expression->type) {
            case ast_node::node_type::ARRAY_INDEX_EXPR:
                return check_expr_array_index(
                        std::reinterpret_pointer_cast<ast_node::array_index_expr_node>(expression), sym_tab);
            case ast_node::node_type::ARRAY_LITERAL_EXPR:
                return check_expr_array_literal(
                        std::reinterpret_pointer_cast<ast_node::array_literal_expr_node>(expression), sym_tab);
            case ast_node::node_type::ARRAY_LOOP_EXPR:
                return check_expr_array_loop(std::reinterpret_pointer_cast<ast_node::array_loop_expr_node>(expression),
                                             sym_tab);
            case ast_node::node_type::BINOP_EXPR:
                return check_expr_binop(std::reinterpret_pointer_cast<ast_node::binop_expr_node>(expression), sym_tab);
            case ast_node::node_type::CALL_EXPR:
                return check_expr_call(std::reinterpret_pointer_cast<ast_node::call_expr_node>(expression), sym_tab);
            case ast_node::node_type::FALSE_EXPR:
                return check_expr_false(std::reinterpret_pointer_cast<ast_node::false_expr_node>(expression), sym_tab);
            case ast_node::node_type::FLOAT_EXPR:
                return check_expr_float(std::reinterpret_pointer_cast<ast_node::float_expr_node>(expression), sym_tab);
            case ast_node::node_type::IF_EXPR:
                return check_expr_if(std::reinterpret_pointer_cast<ast_node::if_expr_node>(expression), sym_tab);
            case ast_node::node_type::INTEGER_EXPR:
                return check_expr_integer(std::reinterpret_pointer_cast<ast_node::integer_expr_node>(expression),
                                          sym_tab);
            case ast_node::node_type::SUM_LOOP_EXPR:
                return check_expr_sum_loop(std::reinterpret_pointer_cast<ast_node::sum_loop_expr_node>(expression),
                                           sym_tab);
            case ast_node::node_type::TRUE_EXPR:
                return check_expr_true(std::reinterpret_pointer_cast<ast_node::true_expr_node>(expression), sym_tab);
            case ast_node::node_type::TUPLE_INDEX_EXPR:
                return check_expr_tuple_index(
                        std::reinterpret_pointer_cast<ast_node::tuple_index_expr_node>(expression), sym_tab);
            case ast_node::node_type::TUPLE_LITERAL_EXPR:
                return check_expr_tuple_literal(
                        std::reinterpret_pointer_cast<ast_node::tuple_literal_expr_node>(expression), sym_tab);
            case ast_node::node_type::UNOP_EXPR:
                return check_expr_unop(std::reinterpret_pointer_cast<ast_node::unop_expr_node>(expression), sym_tab);
            case ast_node::node_type::VARIABLE_EXPR:
                return check_expr_variable(std::reinterpret_pointer_cast<ast_node::variable_expr_node>(expression),
                                           sym_tab);
            default:
                throw std::runtime_error("Tried to call `type_checker::check_expression()` on non-expression node");
        }
    }

    void check_stmt(const std::shared_ptr<ast_node::stmt_node>&, const symbol_table::symbol_table&) {
        //  TODO (HW8): Implement.
    }

    /*
    ===================
    ||  Subclasses:  ||
    ===================
    */

    //  Commands:
    //  ---------

    void check_cmd_assert(const std::shared_ptr<ast_node::assert_cmd_node>& command,
                          symbol_table::symbol_table& sym_tab) {
        if (check_expr(command->arg_1, sym_tab)->type != resolved_type::BOOL_TYPE)
            throw type_check_exception("`check_cmd_assert`: non-boolean expression: " + command->arg_1->s_expression());
    }

    void check_cmd_fn(const std::shared_ptr<ast_node::fn_cmd_node>&, symbol_table::symbol_table&) {
        //  TODO (HW8): Implement.
    }

    void check_cmd_let(const std::shared_ptr<ast_node::let_cmd_node>& command, symbol_table::symbol_table& sym_tab) {
        /**
         * @brief Binds the given resolved_type to the given lvalue.
         * @details This is a local lambda to support recursion.
         *     This will likely be refactored out into its own method in HW8.
         *
         * @param lvalue The lvalue to bind.
         * @param r_type The resolved_type to bind to the lvalue.
         */
        const std::function<void(const std::shared_ptr<ast_node::lvalue_node>&,
                                 const std::shared_ptr<resolved_type::resolved_type>&)>
                bind_lvalue = [&bind_lvalue, &sym_tab](const std::shared_ptr<ast_node::lvalue_node>& lvalue,
                                                       const std::shared_ptr<resolved_type::resolved_type>& r_type) {
                    if (lvalue->type == ast_node::node_type::ARGUMENT_LVALUE) {
                        const std::shared_ptr<ast_node::argument_lvalue_node> argument_lvalue
                                = std::reinterpret_pointer_cast<ast_node::argument_lvalue_node>(lvalue);
                        if (argument_lvalue->arg_1->type == ast_node::node_type::ARRAY_ARGUMENT) {
                            //  Check array arguments (e.g., `let img[H, W] = ...`):
                            //  ----------------------------------------------------
                            const std::shared_ptr<ast_node::array_argument_node> array_argument
                                    = std::reinterpret_pointer_cast<ast_node::array_argument_node>(
                                            argument_lvalue->arg_1);

                            //  Check that the variable isn't already in the symbol table.
                            const std::string variable = array_argument->main_var.text;
                            if (sym_tab.has_symbol(variable))
                                throw type_check_exception("`check_cmd_let`: symbol table already has entry for \""
                                                           + variable + "\"");

                            //  Check each of the binding dimension variables.
                            const std::shared_ptr<resolved_type::resolved_type> int_type
                                    = std::make_shared<resolved_type::resolved_type>(resolved_type::INT_TYPE);
                            for (const token::token& var : array_argument->vars) {
                                //  If the symbol is not already in the symbol table, add it.
                                if (sym_tab.has_symbol(var.text))
                                    throw type_check_exception("`check_cmd_let`: symbol table already has entry for \""
                                                               + var.text + "\"");

                                sym_tab.add_symbol(var.text, std::make_shared<name_info::variable_info>(int_type));
                            }

                            //  Make sure that the expression is an array.
                            if (r_type->type != resolved_type::ARRAY_TYPE)
                                throw type_check_exception(
                                        "`check_cmd_let`: attempting to bind a non-array to an array: "
                                        + r_type->s_expression());

                            //  Make sure that the array sizes match.
                            const std::shared_ptr<resolved_type::array_resolved_type> array_r_type
                                    = std::reinterpret_pointer_cast<resolved_type::array_resolved_type>(r_type);
                            if ((unsigned int)array_argument->vars.size() != array_r_type->rank)
                                throw type_check_exception("`check_cmd_let`: attempting to bind a rank-"
                                                           + std::to_string(array_r_type->rank) + " array to a rank-"
                                                           + std::to_string(array_argument->vars.size()) + " lvalue: ");

                            //  Finally, save the type to the symbol table.
                            sym_tab.add_symbol(variable, std::make_shared<name_info::variable_info>(r_type));
                        } else if (argument_lvalue->arg_1->type == ast_node::node_type::VARIABLE_ARGUMENT) {
                            //  Check simple variable lvalues:
                            //  ------------------------------
                            const std::shared_ptr<ast_node::variable_argument_node> variable_argument
                                    = std::reinterpret_pointer_cast<ast_node::variable_argument_node>(
                                            argument_lvalue->arg_1);
                            const std::string variable = variable_argument->arg_1.text;

                            //  Check that the variable isn't already in the symbol table.
                            if (sym_tab.has_symbol(variable))
                                throw type_check_exception("`check_cmd_let`: symbol table already has entry for \""
                                                           + variable + "\"");

                            //  Finally, add the symbol to the symbol table.
                            sym_tab.add_symbol(variable, std::make_shared<name_info::variable_info>(r_type));
                        } else {
                            throw type_check_exception("`check_cmd_let`: not an argument lvalue: "
                                                       + argument_lvalue->arg_1->s_expression());
                        }
                    } else if (lvalue->type == ast_node::node_type::TUPLE_LVALUE) {
                        //  Check tuple lvalues:
                        //  --------------------
                        const std::shared_ptr<ast_node::tuple_lvalue_node> tuple_lvalue
                                = std::reinterpret_pointer_cast<ast_node::tuple_lvalue_node>(lvalue);

                        //  Check the expression tuple.
                        if (r_type->type != resolved_type::TUPLE_TYPE)
                            throw type_check_exception("`check_cmd_let`: not a tuple expression: "
                                                       + r_type->s_expression());
                        const std::shared_ptr<resolved_type::tuple_resolved_type> tuple_r_type
                                = std::reinterpret_pointer_cast<resolved_type::tuple_resolved_type>(r_type);

                        //  Make sure that the dimensions match.
                        if (tuple_lvalue->lvalues.size() != tuple_r_type->element_types.size())
                            throw type_check_exception("`check_cmd_let`: tuple dimension mismatch: "
                                                       + tuple_lvalue->s_expression() + ", "
                                                       + tuple_r_type->s_expression());

                        for (unsigned int index = 0; index < (unsigned int)tuple_lvalue->lvalues.size(); index++) {
                            //  Bind each variable.
                            bind_lvalue(tuple_lvalue->lvalues[index], tuple_r_type->element_types[index]);
                        }
                    } else {
                        throw type_check_exception("`check_cmd_let`: not an lvalue: " + lvalue->s_expression());
                    }
                };

        const std::shared_ptr<resolved_type::resolved_type> r_type = check_expr(command->arg_2, sym_tab);
        bind_lvalue(command->arg_1, r_type);
    }

    void check_cmd_print(const std::shared_ptr<ast_node::print_cmd_node>&, symbol_table::symbol_table&) {
        //  There are no expressions in a `print` command. No type-checking to be done here.
    }

    void check_cmd_read(const std::shared_ptr<ast_node::read_cmd_node>& command, symbol_table::symbol_table& sym_tab) {
        //  Define a single pixel's type as a tuple of four floats.
        const std::shared_ptr<resolved_type::resolved_type> float_type = std::make_shared<resolved_type::resolved_type>(
                resolved_type::FLOAT_TYPE);
        const std::vector<std::shared_ptr<resolved_type::resolved_type>> pixel_tuple {float_type, float_type,
                                                                                      float_type, float_type};
        const std::shared_ptr<resolved_type::resolved_type> pixel_type
                = std::make_shared<resolved_type::tuple_resolved_type>(pixel_tuple);

        //  Define an image's type as a rank-2 array of pixels.
        const std::shared_ptr<resolved_type::resolved_type> img_type
                = std::make_shared<resolved_type::array_resolved_type>(pixel_type, 2);

        if (command->arg_2->type == ast_node::node_type::ARRAY_ARGUMENT) {
            //  Check array arguments:
            //  ----------------------
            const std::shared_ptr<ast_node::array_argument_node> array_argument
                    = std::reinterpret_pointer_cast<ast_node::array_argument_node>(command->arg_2);

            //  Make sure that it's a rank-2 array.
            if (array_argument->vars.size() != 2)
                throw type_check_exception("`check_cmd_read`: can't bind an image to a rank-"
                                           + std::to_string(array_argument->vars.size()) + " array");

            //  Add each dimension variable to the symbol table.
            const std::shared_ptr<resolved_type::resolved_type> int_type
                    = std::make_shared<resolved_type::resolved_type>(resolved_type::INT_TYPE);
            for (const token::token& var : array_argument->vars) {
                if (sym_tab.has_symbol(var.text))
                    throw type_check_exception("`check_cmd_read`: symbol table already has entry for \"" + var.text
                                               + "\"");
                sym_tab.add_symbol(var.text, std::make_shared<name_info::variable_info>(int_type));
            }

            //  Add the main variable to the symbol table.
            const std::string variable = array_argument->main_var.text;
            if (sym_tab.has_symbol(variable))
                throw type_check_exception("`check_cmd_read`: symbol table already has entry for \"" + variable + "\"");
            sym_tab.add_symbol(variable, std::make_shared<name_info::variable_info>(img_type));
        } else if (command->arg_2->type == ast_node::node_type::VARIABLE_ARGUMENT) {
            //  Check variable arguments:
            //  -------------------------
            const std::shared_ptr<ast_node::variable_argument_node> variable_argument
                    = std::reinterpret_pointer_cast<ast_node::variable_argument_node>(command->arg_2);

            //  Add the variable to the symbol table.
            const std::string variable = variable_argument->arg_1.text;
            if (sym_tab.has_symbol(variable))
                throw type_check_exception("`check_cmd_read`: symbol table already has entry for \"" + variable + "\"");
            sym_tab.add_symbol(variable, std::make_shared<name_info::variable_info>(img_type));
        } else {
            throw type_check_exception("`check_cmd_read`: not an argument lvalue command: \""
                                       + command->arg_2->s_expression() + "\"");
        }
    }

    void check_cmd_show(const std::shared_ptr<ast_node::show_cmd_node>& command, symbol_table::symbol_table& sym_tab) {
        check_expr(command->arg_1, sym_tab);
    }

    void check_cmd_time(const std::shared_ptr<ast_node::time_cmd_node>& command, symbol_table::symbol_table& sym_tab) {
        check_cmd(command->command, sym_tab);
    }

    void check_cmd_type(const std::shared_ptr<ast_node::type_cmd_node>&, symbol_table::symbol_table&) {
        //  TODO (HW8): Implement.
    }

    void check_cmd_write(const std::shared_ptr<ast_node::write_cmd_node>& command,
                         symbol_table::symbol_table& sym_tab) {
        const std::shared_ptr<resolved_type::resolved_type> expr_type = check_expr(command->arg_1, sym_tab);
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
    check_expr_array_index(const std::shared_ptr<ast_node::array_index_expr_node>& expression,
                           const symbol_table::symbol_table& sym_tab) {
        const std::shared_ptr<resolved_type::resolved_type> array_type = check_expr(expression->array, sym_tab);
        if (array_type->type != resolved_type::ARRAY_TYPE)
            throw type_check_exception("`check_expr_array_index`: indexing a non-array: " + expression->s_expression());

        const std::shared_ptr<resolved_type::array_resolved_type> full_array_type
                = std::reinterpret_pointer_cast<resolved_type::array_resolved_type>(array_type);
        if (full_array_type->rank != expression->params.size())
            throw type_check_exception("`check_expr_array_index`: incorrect number of index args: "
                                       + expression->s_expression());

        for (const std::shared_ptr<ast_node::expr_node>& param : expression->params) {
            if (check_expr(param, sym_tab)->type != resolved_type::INT_TYPE)
                throw type_check_exception("`check_expr_array_index`: index arg not an integer: "
                                           + expression->s_expression());
        }

        const std::shared_ptr<resolved_type::resolved_type> r_type = full_array_type->element_type;

        expression->r_type = r_type;

        return r_type;
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_array_literal(const std::shared_ptr<ast_node::array_literal_expr_node>& expression,
                             const symbol_table::symbol_table& sym_tab) {
        if (expression->expressions.empty())
            throw type_check_exception("`check_expr_array_literal`: empty arrays not allowed");
        const std::shared_ptr<resolved_type::resolved_type> element_type = check_expr(expression->expressions[0],
                                                                                      sym_tab);
        for (unsigned int index = 1; index < (unsigned int)expression->expressions.size(); index++) {
            const std::shared_ptr<resolved_type::resolved_type> type = check_expr(expression->expressions[index],
                                                                                  sym_tab);
            if (*element_type != *type)
                throw type_check_exception("`check_expr_array_literal`: type mismatch: " + expression->s_expression());
        }

        std::shared_ptr<resolved_type::resolved_type> r_type = std::make_shared<resolved_type::array_resolved_type>(
                element_type, 1);
        expression->r_type = r_type;

        return r_type;
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_array_loop(const std::shared_ptr<ast_node::array_loop_expr_node>& expression,
                          const symbol_table::symbol_table& sym_tab) {
        symbol_table::symbol_table array_loop_expr_symbols(sym_tab);

        for (const ast_node::array_loop_expr_node::binding_pair_t& binding_pair : expression->binding_pairs) {
            const std::string variable = std::get<0>(binding_pair).text;

            if (array_loop_expr_symbols.has_symbol(variable))
                throw type_check_exception("`check_expr_array_loop`: symbol table already has entry for \"" + variable
                                           + "\"");

            const std::shared_ptr<resolved_type::resolved_type> r_type = check_expr(std::get<1>(binding_pair), sym_tab);

            if (r_type->type != resolved_type::INT_TYPE)
                throw type_check_exception("`check_expr_array_loop`: not an integer: " + r_type->s_expression());

            array_loop_expr_symbols.add_symbol(variable, std::make_shared<name_info::variable_info>(r_type));
        }

        const std::shared_ptr<resolved_type::resolved_type> item_type = check_expr(expression->item_expr,
                                                                                   array_loop_expr_symbols);
        const std::shared_ptr<resolved_type::resolved_type> r_type
                = std::make_shared<resolved_type::array_resolved_type>(item_type, expression->binding_pairs.size());

        expression->r_type = r_type;

        return r_type;
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_binop(const std::shared_ptr<ast_node::binop_expr_node>& expression,
                     const symbol_table::symbol_table& sym_tab) {
        const std::shared_ptr<resolved_type::resolved_type> left_type = check_expr(expression->left_operand, sym_tab);
        const std::shared_ptr<resolved_type::resolved_type> right_type = check_expr(expression->right_operand, sym_tab);

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
            case ast_node::BINOP_LEQ:
            case ast_node::BINOP_GEQ:
                if (left_type->type != resolved_type::INT_TYPE && left_type->type != resolved_type::FLOAT_TYPE)
                    throw type_check_exception("`check_expr_binop`: not valid operands: " + expression->s_expression());
                r_type = std::make_shared<resolved_type::resolved_type>(resolved_type::BOOL_TYPE);
                break;
            case ast_node::BINOP_EQ:
            case ast_node::BINOP_NEQ:
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
                                                                  const symbol_table::symbol_table&) {
        //  TODO (HW8): Implement.

        return {};
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_false(const std::shared_ptr<ast_node::false_expr_node>& expression, const symbol_table::symbol_table&) {
        const std::shared_ptr<resolved_type::resolved_type> bool_type = std::make_shared<resolved_type::resolved_type>(
                resolved_type::BOOL_TYPE);

        expression->r_type = bool_type;

        return bool_type;
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_float(const std::shared_ptr<ast_node::float_expr_node>& expression, const symbol_table::symbol_table&) {
        const std::shared_ptr<resolved_type::resolved_type> float_type = std::make_shared<resolved_type::resolved_type>(
                resolved_type::FLOAT_TYPE);

        expression->r_type = float_type;

        return float_type;
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_if(const std::shared_ptr<ast_node::if_expr_node>& expression, const symbol_table::symbol_table& scopes) {
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
    check_expr_integer(const std::shared_ptr<ast_node::integer_expr_node>& expression,
                       const symbol_table::symbol_table&) {
        const std::shared_ptr<resolved_type::resolved_type> int_type = std::make_shared<resolved_type::resolved_type>(
                resolved_type::INT_TYPE);

        expression->r_type = int_type;

        return int_type;
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_sum_loop(const std::shared_ptr<ast_node::sum_loop_expr_node>& expression,
                        const symbol_table::symbol_table& sym_tab) {
        symbol_table::symbol_table sum_loop_expr_symbols(sym_tab);

        for (const ast_node::sum_loop_expr_node::binding_pair_t& binding_pair : expression->binding_pairs) {
            const std::string variable = std::get<0>(binding_pair).text;

            if (sum_loop_expr_symbols.has_symbol(variable))
                throw type_check_exception("`check_expr_sum_loop`: symbol table already has entry for \"" + variable
                                           + "\"");

            const std::shared_ptr<resolved_type::resolved_type> r_type = check_expr(std::get<1>(binding_pair), sym_tab);

            if (r_type->type != resolved_type::INT_TYPE)
                throw type_check_exception("`check_expr_sum_loop`: not an integer: " + r_type->s_expression());

            sum_loop_expr_symbols.add_symbol(variable, std::make_shared<name_info::variable_info>(r_type));
        }

        const std::shared_ptr<resolved_type::resolved_type> r_type = check_expr(expression->sum_expr,
                                                                                sum_loop_expr_symbols);
        if (r_type->type != resolved_type::INT_TYPE && r_type->type != resolved_type::FLOAT_TYPE)
            throw type_check_exception("`check_expr_sum_loop`: not a number: " + r_type->s_expression());

        expression->r_type = r_type;

        return r_type;
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_true(const std::shared_ptr<ast_node::true_expr_node>& expression, const symbol_table::symbol_table&) {
        const std::shared_ptr<resolved_type::resolved_type> bool_type = std::make_shared<resolved_type::resolved_type>(
                resolved_type::BOOL_TYPE);

        expression->r_type = bool_type;

        return bool_type;
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_tuple_index(const std::shared_ptr<ast_node::tuple_index_expr_node>& expression,
                           const symbol_table::symbol_table& sym_tab) {
        const std::shared_ptr<resolved_type::resolved_type> tuple_type = check_expr(expression->expr, sym_tab);
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
                             const symbol_table::symbol_table& sym_tab) {
        std::vector<std::shared_ptr<resolved_type::resolved_type>> r_types;

        for (const std::shared_ptr<ast_node::expr_node>& expr : expression->exprs) {
            r_types.push_back(check_expr(expr, sym_tab));
        }

        const std::shared_ptr<resolved_type::resolved_type> r_type
                = std::make_shared<resolved_type::tuple_resolved_type>(r_types);

        expression->r_type = r_type;

        return r_type;
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_unop(const std::shared_ptr<ast_node::unop_expr_node>& expression,
                    const symbol_table::symbol_table& sym_tab) {
        const std::shared_ptr<resolved_type::resolved_type> r_type = check_expr(expression->operand, sym_tab);

        if (expression->type == ast_node::op_type::UNOP_NEG && r_type->type != resolved_type::INT_TYPE
            && r_type->type != resolved_type::FLOAT_TYPE)
            throw type_check_exception("`check_expr_unop`: not a number: " + expression->s_expression());

        if (expression->type == ast_node::op_type::UNOP_INV && r_type->type != resolved_type::BOOL_TYPE)
            throw type_check_exception("`check_expr_unop`: not a boolean: " + expression->s_expression());

        expression->r_type = r_type;

        return r_type;
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_variable(const std::shared_ptr<ast_node::variable_expr_node>& expression,
                        const symbol_table::symbol_table& sym_tab) {
        const std::string variable = expression->arg_1.text;

        if (!sym_tab.has_symbol(variable))
            throw type_check_exception("`check_expr_variable`: no entry in symbol table: " + variable);

        const std::shared_ptr<name_info::name_info> info = sym_tab[variable];

        if (info->name_info_type != name_info::name_info_class::VARIABLE_INFO) {
            throw type_check_exception("`check_expr_variable`: not a variable: " + variable);
        }

        const std::shared_ptr<resolved_type::resolved_type> r_type
                = std::reinterpret_pointer_cast<name_info::variable_info>(info)->r_type;

        expression->r_type = r_type;

        return r_type;
    }
}  // namespace type_checker
