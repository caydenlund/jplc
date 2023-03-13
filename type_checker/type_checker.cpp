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
    void bind_lvalue(const std::shared_ptr<ast_node::lvalue_node>& lvalue,
                     const std::shared_ptr<resolved_type::resolved_type>& r_type, symbol_table::symbol_table& sym_tab) {
        if (lvalue->type == ast_node::node_type::ARGUMENT_LVALUE) {
            const std::shared_ptr<ast_node::argument_lvalue_node> argument_lvalue
                    = std::reinterpret_pointer_cast<ast_node::argument_lvalue_node>(lvalue);
            if (argument_lvalue->argument->type == ast_node::node_type::ARRAY_ARGUMENT) {
                //  Check array arguments (e.g., `let img[H, W] = ...`):
                //  ----------------------------------------------------
                const std::shared_ptr<ast_node::array_argument_node> array_argument
                        = std::reinterpret_pointer_cast<ast_node::array_argument_node>(argument_lvalue->argument);

                //  Check that the variable isn't already in the symbol table.
                const std::string variable = array_argument->name;
                if (sym_tab.has_symbol(variable))
                    throw type_check_exception("`check_cmd_let`: symbol table already has entry for \"" + variable
                                               + "\"");

                //  Check each of the binding dimension variables.
                const std::shared_ptr<resolved_type::resolved_type> int_type
                        = std::make_shared<resolved_type::resolved_type>(resolved_type::INT_TYPE);
                for (const token::token& var : array_argument->dimension_vars) {
                    //  If the symbol is not already in the symbol table, add it.
                    if (sym_tab.has_symbol(var.text))
                        throw type_check_exception("`check_cmd_let`: symbol table already has entry for \"" + var.text
                                                   + "\"");

                    sym_tab.add_symbol(var.text, std::make_shared<name_info::variable_info>(int_type));
                }

                //  Make sure that the expression is an array.
                if (r_type->type != resolved_type::ARRAY_TYPE)
                    throw type_check_exception("`check_cmd_let`: attempting to bind a non-array to an array: "
                                               + r_type->s_expression());

                //  Make sure that the array sizes match.
                const std::shared_ptr<resolved_type::array_resolved_type> array_r_type
                        = std::reinterpret_pointer_cast<resolved_type::array_resolved_type>(r_type);
                if ((unsigned int)array_argument->dimension_vars.size() != array_r_type->rank)
                    throw type_check_exception("`check_cmd_let`: attempting to bind a rank-"
                                               + std::to_string(array_r_type->rank) + " array to a rank-"
                                               + std::to_string(array_argument->dimension_vars.size()) + " lvalue: ");

                //  Finally, save the type to the symbol table.
                sym_tab.add_symbol(variable, std::make_shared<name_info::variable_info>(r_type));
            } else if (argument_lvalue->argument->type == ast_node::node_type::VARIABLE_ARGUMENT) {
                //  Check simple variable lvalues:
                //  ------------------------------
                const std::shared_ptr<ast_node::variable_argument_node> variable_argument
                        = std::reinterpret_pointer_cast<ast_node::variable_argument_node>(argument_lvalue->argument);
                const std::string variable = variable_argument->name;

                //  Check that the variable isn't already in the symbol table.
                if (sym_tab.has_symbol(variable))
                    throw type_check_exception("`check_cmd_let`: symbol table already has entry for \"" + variable
                                               + "\"");

                //  Finally, add the symbol to the symbol table.
                sym_tab.add_symbol(variable, std::make_shared<name_info::variable_info>(r_type));
            } else {
                throw type_check_exception("`check_cmd_let`: not an argument lvalue: "
                                           + argument_lvalue->argument->s_expression());
            }
        } else if (lvalue->type == ast_node::node_type::TUPLE_LVALUE) {
            //  Check tuple lvalues:
            //  --------------------
            const std::shared_ptr<ast_node::tuple_lvalue_node> tuple_lvalue
                    = std::reinterpret_pointer_cast<ast_node::tuple_lvalue_node>(lvalue);

            //  Check the expression tuple.
            if (r_type->type != resolved_type::TUPLE_TYPE)
                throw type_check_exception("`check_cmd_let`: not a tuple expression: " + r_type->s_expression());
            const std::shared_ptr<resolved_type::tuple_resolved_type> tuple_r_type
                    = std::reinterpret_pointer_cast<resolved_type::tuple_resolved_type>(r_type);

            //  Make sure that the dimensions match.
            if (tuple_lvalue->lvalues.size() != tuple_r_type->element_types.size())
                throw type_check_exception("`check_cmd_let`: tuple dimension mismatch: " + tuple_lvalue->s_expression()
                                           + ", " + tuple_r_type->s_expression());

            for (unsigned int index = 0; index < (unsigned int)tuple_lvalue->lvalues.size(); index++) {
                //  Bind each variable.
                bind_lvalue(tuple_lvalue->lvalues[index], tuple_r_type->element_types[index], sym_tab);
            }
        } else {
            throw type_check_exception("`check_cmd_let`: not an lvalue: " + lvalue->s_expression());
        }
    }

    std::shared_ptr<resolved_type::resolved_type> add_binding(const std::shared_ptr<ast_node::binding_node>& binding,
                                                              symbol_table::symbol_table& sym_tab) {
        if (binding->type == ast_node::node_type::VAR_BINDING) {
            const std::shared_ptr<ast_node::var_binding_node> var_binding
                    = std::reinterpret_pointer_cast<ast_node::var_binding_node>(binding);
            const std::shared_ptr<ast_node::argument_node> binding_arg
                    = std::reinterpret_pointer_cast<ast_node::argument_node>(var_binding->binding_arg);
            const std::shared_ptr<ast_node::type_node>& binding_type = var_binding->binding_type;
            const std::shared_ptr<resolved_type::resolved_type> r_type = resolve_type(binding_type, sym_tab);

            if (binding_arg->type == ast_node::node_type::ARRAY_ARGUMENT) {
                if (binding_type->type != ast_node::node_type::ARRAY_TYPE)
                    throw type_check_exception("`add_binding`: not an array: \"" + binding_type->s_expression() + "\"");

                const std::shared_ptr<ast_node::array_argument_node> array_argument
                        = std::reinterpret_pointer_cast<ast_node::array_argument_node>(binding_arg);
                const std::shared_ptr<ast_node::array_type_node> array_type
                        = std::reinterpret_pointer_cast<ast_node::array_type_node>(binding_type);

                if (array_argument->dimension_vars.size() != array_type->dimensions)
                    throw type_check_exception("`add_binding`: array dimension mismatch: \"" + binding->s_expression()
                                               + "\"");

                if (sym_tab.has_symbol(array_argument->name))
                    throw type_check_exception("`add_binding`: symbol table already has entry for \""
                                               + array_argument->name + "\"");

                sym_tab.add_symbol(array_argument->name, std::make_shared<name_info::variable_info>(r_type));

                const std::shared_ptr<resolved_type::resolved_type> int_r_type
                        = std::make_shared<resolved_type::resolved_type>(resolved_type::INT_TYPE);
                for (const token::token& var : array_argument->dimension_vars) {
                    if (sym_tab.has_symbol(var.text))
                        throw type_check_exception("`add_binding`: symbol table already has entry for \"" + var.text
                                                   + "\"");

                    sym_tab.add_symbol(var.text, std::make_shared<name_info::variable_info>(int_r_type));
                }
            } else if (binding_arg->type == ast_node::node_type::VARIABLE_ARGUMENT) {
                const std::shared_ptr<ast_node::variable_argument_node> var_arg
                        = std::reinterpret_pointer_cast<ast_node::variable_argument_node>(binding_arg);

                if (sym_tab.has_symbol(var_arg->name))
                    throw type_check_exception("`add_binding`: symbol table already has entry for \"" + var_arg->name
                                               + "\"");

                sym_tab.add_symbol(var_arg->name, std::make_shared<name_info::variable_info>(r_type));
            } else {
                throw type_check_exception("`add_binding`: unimplemented argument type: \""
                                           + binding_arg->s_expression() + "\"");
            }

            return r_type;
        }

        if (binding->type == ast_node::node_type::TUPLE_BINDING) {
            const std::shared_ptr<ast_node::tuple_binding_node> tuple_binding
                    = std::reinterpret_pointer_cast<ast_node::tuple_binding_node>(binding);

            std::vector<std::shared_ptr<resolved_type::resolved_type>> r_types;

            for (const std::shared_ptr<ast_node::binding_node>& item_binding : tuple_binding->bindings) {
                const std::shared_ptr<resolved_type::resolved_type> r_type = add_binding(item_binding, sym_tab);
                r_types.push_back(r_type);
            }

            const std::shared_ptr<resolved_type::resolved_type> r_type
                    = std::make_shared<resolved_type::tuple_resolved_type>(r_types);

            return r_type;
        }

        throw type_check_exception("`add_binding`: invalid binding: \"" + binding->s_expression() + "\"");
    }

    void initialize_symbol_table(symbol_table::symbol_table& sym_tab) {
        //  Command-line arguments:
        //  -----------------------
        const std::shared_ptr<resolved_type::resolved_type> int_type = std::make_shared<resolved_type::resolved_type>(
                resolved_type::INT_TYPE);
        const std::shared_ptr<resolved_type::array_resolved_type> int_array_type
                = std::make_shared<resolved_type::array_resolved_type>(int_type, 1);
        sym_tab.add_symbol("argnum", std::make_shared<name_info::variable_info>(int_type));
        sym_tab.add_symbol("args", std::make_shared<name_info::variable_info>(int_array_type));

        //  Built-in functions:
        //  -------------------
        const std::shared_ptr<resolved_type::resolved_type> float_type = std::make_shared<resolved_type::resolved_type>(
                resolved_type::FLOAT_TYPE);
        const std::shared_ptr<name_info::function_info> one_float_ret_float
                = std::make_shared<name_info::function_info>(
                        std::vector<std::shared_ptr<resolved_type::resolved_type>> {float_type}, float_type);
        sym_tab.add_symbol("acos", one_float_ret_float);
        sym_tab.add_symbol("asin", one_float_ret_float);
        sym_tab.add_symbol("atan", one_float_ret_float);
        sym_tab.add_symbol("cos", one_float_ret_float);
        sym_tab.add_symbol("exp", one_float_ret_float);
        sym_tab.add_symbol("log", one_float_ret_float);
        sym_tab.add_symbol("sin", one_float_ret_float);
        sym_tab.add_symbol("sqrt", one_float_ret_float);
        sym_tab.add_symbol("tan", one_float_ret_float);

        const std::shared_ptr<name_info::function_info> two_float_ret_float
                = std::make_shared<name_info::function_info>(
                        std::vector<std::shared_ptr<resolved_type::resolved_type>> {float_type, float_type},
                        float_type);
        sym_tab.add_symbol("atan2", two_float_ret_float);
        sym_tab.add_symbol("pow", two_float_ret_float);

        const std::shared_ptr<name_info::function_info> one_float_ret_int = std::make_shared<name_info::function_info>(
                std::vector<std::shared_ptr<resolved_type::resolved_type>> {float_type}, int_type);
        sym_tab.add_symbol("to_int", one_float_ret_int);

        const std::shared_ptr<name_info::function_info> one_int_ret_float = std::make_shared<name_info::function_info>(
                std::vector<std::shared_ptr<resolved_type::resolved_type>> {int_type}, float_type);
        sym_tab.add_symbol("to_float", one_int_ret_float);
    }

    std::shared_ptr<resolved_type::resolved_type> resolve_type(const std::shared_ptr<ast_node::type_node>& type,
                                                               const symbol_table::symbol_table& sym_tab) {
        std::shared_ptr<ast_node::array_type_node> array_type;
        std::shared_ptr<ast_node::tuple_type_node> tuple_type;
        std::shared_ptr<ast_node::variable_type_node> variable_type;
        std::shared_ptr<name_info::name_info> info;
        std::shared_ptr<name_info::variable_info> var_info;
        std::vector<std::shared_ptr<resolved_type::resolved_type>> tuple_element_types;
        switch (type->type) {
            case ast_node::node_type::ARRAY_TYPE:
                array_type = std::reinterpret_pointer_cast<ast_node::array_type_node>(type);
                return std::make_shared<resolved_type::array_resolved_type>(
                        resolve_type(array_type->main_type, sym_tab), array_type->dimensions);
            case ast_node::node_type::BOOL_TYPE:
                return std::make_shared<resolved_type::resolved_type>(resolved_type::resolved_type_type::BOOL_TYPE);
            case ast_node::node_type::FLOAT_TYPE:
                return std::make_shared<resolved_type::resolved_type>(resolved_type::resolved_type_type::FLOAT_TYPE);
            case ast_node::node_type::INT_TYPE:
                return std::make_shared<resolved_type::resolved_type>(resolved_type::resolved_type_type::INT_TYPE);
            case ast_node::node_type::TUPLE_TYPE:
                tuple_type = std::reinterpret_pointer_cast<ast_node::tuple_type_node>(type);
                for (const auto& one_type : tuple_type->types) {
                    tuple_element_types.push_back(resolve_type(one_type, sym_tab));
                }
                return std::make_shared<resolved_type::tuple_resolved_type>(tuple_element_types);
            case ast_node::node_type::VARIABLE_TYPE:
                variable_type = std::reinterpret_pointer_cast<ast_node::variable_type_node>(type);
                if (!sym_tab.has_symbol(variable_type->name))
                    throw type_check_exception("`resolve_type`: undefined variable: \"" + variable_type->name + "\"");
                info = sym_tab[variable_type->name];
                if (info->name_info_type != name_info::name_info_class::TYPE_INFO)
                    throw type_check_exception("`resolve_type`: not a type: \"" + variable_type->name + "\"");
                var_info = std::reinterpret_pointer_cast<name_info::variable_info>(info);
                return var_info->r_type;
            default:
                throw type_check_exception("`resolve_type`: not a type: \"" + type->s_expression() + "\"");
        }
    }

    bool resolved_types_are_equal(const std::shared_ptr<resolved_type::resolved_type>& type_1,
                                  const std::shared_ptr<resolved_type::resolved_type>& type_2) {
        if (type_1->type != type_2->type) return false;

        if (type_1->type == resolved_type::resolved_type_type::ARRAY_TYPE) {
            const std::shared_ptr<resolved_type::array_resolved_type> array_type_1
                    = std::reinterpret_pointer_cast<resolved_type::array_resolved_type>(type_1);
            const std::shared_ptr<resolved_type::array_resolved_type> array_type_2
                    = std::reinterpret_pointer_cast<resolved_type::array_resolved_type>(type_2);

            if (array_type_1->rank != array_type_2->rank) return false;

            return resolved_types_are_equal(array_type_1->element_type, array_type_2->element_type);
        }

        if (type_1->type == resolved_type::resolved_type_type::TUPLE_TYPE) {
            const std::shared_ptr<resolved_type::tuple_resolved_type> tuple_type_1
                    = std::reinterpret_pointer_cast<resolved_type::tuple_resolved_type>(type_1);
            const std::shared_ptr<resolved_type::tuple_resolved_type> tuple_type_2
                    = std::reinterpret_pointer_cast<resolved_type::tuple_resolved_type>(type_2);

            if (tuple_type_1->element_types.size() != tuple_type_2->element_types.size()) return false;

            for (unsigned int index = 0; index < (unsigned int)tuple_type_1->element_types.size(); index++) {
                if (!resolved_types_are_equal(tuple_type_1->element_types[index], tuple_type_2->element_types[index]))
                    return false;
            }
        }

        return true;
    }

    void check(const parser::node_list_t& nodes) {
        symbol_table::symbol_table global_symbol_table;
        initialize_symbol_table(global_symbol_table);

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
                throw type_check_exception("`check_cmd`: not a command: \"" + command->s_expression() + "\"");
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
                throw type_check_exception("`check_expr`: not an expression: \"" + expression->s_expression() + "\"");
        }
    }

    bool check_stmt(const std::shared_ptr<ast_node::stmt_node>& statement, symbol_table::symbol_table& sym_tab,
                    const std::shared_ptr<resolved_type::resolved_type>& r_type) {
        switch (statement->type) {
            case ast_node::node_type::ASSERT_STMT:
                check_stmt_assert(std::reinterpret_pointer_cast<ast_node::assert_stmt_node>(statement), sym_tab);
                break;
            case ast_node::node_type::LET_STMT:
                check_stmt_let(std::reinterpret_pointer_cast<ast_node::let_stmt_node>(statement), sym_tab);
                break;
            case ast_node::node_type::RETURN_STMT:
                check_stmt_return(std::reinterpret_pointer_cast<ast_node::return_stmt_node>(statement), sym_tab,
                                  r_type);
                return true;
            default:
                throw type_check_exception("`check_stmt`: not a statement: \"" + statement->s_expression() + "\"");
        }

        return false;
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
        if (check_expr(command->condition, sym_tab)->type != resolved_type::BOOL_TYPE)
            throw type_check_exception("`check_cmd_assert`: non-boolean expression: "
                                       + command->condition->s_expression());
    }

    void check_cmd_fn(const std::shared_ptr<ast_node::fn_cmd_node>& command, symbol_table::symbol_table& sym_tab) {
        const std::string& func_name = command->name;

        if (sym_tab.has_symbol(func_name))
            throw type_check_exception("`check_cmd_fn`: symbol table already has entry for \"" + func_name + "\"");

        std::vector<std::shared_ptr<resolved_type::resolved_type>> call_args;

        symbol_table::symbol_table new_scope(&sym_tab);

        for (const std::shared_ptr<ast_node::binding_node>& binding : command->bindings) {
            const std::shared_ptr<resolved_type::resolved_type> r_type = add_binding(binding, new_scope);
            call_args.push_back(r_type);
        }

        const std::shared_ptr<resolved_type::resolved_type> return_type = resolve_type(command->return_type, sym_tab);
        const std::shared_ptr<name_info::function_info> func_info = std::make_shared<name_info::function_info>(
                call_args, return_type);
        sym_tab.add_symbol(func_name, func_info);

        bool did_return = false;
        for (const std::shared_ptr<ast_node::stmt_node>& statement : command->statements) {
            did_return = check_stmt(statement, new_scope, return_type) || did_return;
        }

        const bool function_needs_return = return_type->type != resolved_type::resolved_type_type::TUPLE_TYPE
                                        || !std::reinterpret_pointer_cast<resolved_type::tuple_resolved_type>(
                                                    return_type)
                                                    ->element_types.empty();

        if (!did_return && function_needs_return)
            throw type_check_exception("Function + \"" + func_name + "\" did not return type \""
                                       + return_type->s_expression() + "\"");
    }

    void check_cmd_let(const std::shared_ptr<ast_node::let_cmd_node>& command, symbol_table::symbol_table& sym_tab) {
        const std::shared_ptr<resolved_type::resolved_type> r_type = check_expr(command->expr, sym_tab);
        bind_lvalue(command->lvalue, r_type, sym_tab);
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

        if (command->read_dest->type == ast_node::node_type::ARRAY_ARGUMENT) {
            //  Check array arguments:
            //  ----------------------
            const std::shared_ptr<ast_node::array_argument_node> array_argument
                    = std::reinterpret_pointer_cast<ast_node::array_argument_node>(command->read_dest);

            //  Make sure that it's a rank-2 array.
            if (array_argument->dimension_vars.size() != 2)
                throw type_check_exception("`check_cmd_read`: can't bind an image to a rank-"
                                           + std::to_string(array_argument->dimension_vars.size()) + " array");

            //  Add each dimension variable to the symbol table.
            const std::shared_ptr<resolved_type::resolved_type> int_type
                    = std::make_shared<resolved_type::resolved_type>(resolved_type::INT_TYPE);
            for (const token::token& var : array_argument->dimension_vars) {
                if (sym_tab.has_symbol(var.text))
                    throw type_check_exception("`check_cmd_read`: symbol table already has entry for \"" + var.text
                                               + "\"");
                sym_tab.add_symbol(var.text, std::make_shared<name_info::variable_info>(int_type));
            }

            //  Add the main variable to the symbol table.
            const std::string variable = array_argument->name;
            if (sym_tab.has_symbol(variable))
                throw type_check_exception("`check_cmd_read`: symbol table already has entry for \"" + variable + "\"");
            sym_tab.add_symbol(variable, std::make_shared<name_info::variable_info>(img_type));
        } else if (command->read_dest->type == ast_node::node_type::VARIABLE_ARGUMENT) {
            //  Check variable arguments:
            //  -------------------------
            const std::shared_ptr<ast_node::variable_argument_node> variable_argument
                    = std::reinterpret_pointer_cast<ast_node::variable_argument_node>(command->read_dest);

            //  Add the variable to the symbol table.
            const std::string variable = variable_argument->name;
            if (sym_tab.has_symbol(variable))
                throw type_check_exception("`check_cmd_read`: symbol table already has entry for \"" + variable + "\"");
            sym_tab.add_symbol(variable, std::make_shared<name_info::variable_info>(img_type));
        } else {
            throw type_check_exception("`check_cmd_read`: not an argument lvalue command: \""
                                       + command->read_dest->s_expression() + "\"");
        }
    }

    void check_cmd_show(const std::shared_ptr<ast_node::show_cmd_node>& command, symbol_table::symbol_table& sym_tab) {
        check_expr(command->expr, sym_tab);
    }

    void check_cmd_time(const std::shared_ptr<ast_node::time_cmd_node>& command, symbol_table::symbol_table& sym_tab) {
        check_cmd(command->command, sym_tab);
    }

    void check_cmd_type(const std::shared_ptr<ast_node::type_cmd_node>& command, symbol_table::symbol_table& sym_tab) {
        const std::string& type_name = command->name;
        if (sym_tab.has_symbol(type_name))
            throw type_check_exception("`check_cmd_type`: symbol table already has entry for \"" + type_name + "\"");

        const std::shared_ptr<resolved_type::resolved_type> r_type = resolve_type(command->new_type, sym_tab);

        sym_tab.add_symbol(type_name, std::make_shared<name_info::type_info>(r_type));
    }

    void check_cmd_write(const std::shared_ptr<ast_node::write_cmd_node>& command,
                         symbol_table::symbol_table& sym_tab) {
        const std::shared_ptr<resolved_type::resolved_type> expr_type = check_expr(command->expr, sym_tab);
        if (expr_type->type != resolved_type::ARRAY_TYPE)
            throw type_check_exception("`check_cmd_write`: not an array: " + command->expr->s_expression());

        const std::shared_ptr<resolved_type::array_resolved_type> array_type
                = std::reinterpret_pointer_cast<resolved_type::array_resolved_type>(expr_type);
        if (array_type->rank != 2)
            throw type_check_exception("`check_cmd_write`: not a rank-2 array: " + command->expr->s_expression());

        if (array_type->element_type->type != resolved_type::resolved_type_type::TUPLE_TYPE)
            throw type_check_exception("`check_cmd_write`: not an array of tuples: \"" + command->expr->s_expression()
                                       + "\"");

        const std::shared_ptr<resolved_type::tuple_resolved_type> tuple_type
                = std::reinterpret_pointer_cast<resolved_type::tuple_resolved_type>(array_type->element_type);
        if (tuple_type->element_types.size() != 4)
            throw type_check_exception("`check_cmd_write`: not an array of length-4 tuples: \""
                                       + command->expr->s_expression() + "\"");

        for (const std::shared_ptr<resolved_type::resolved_type>& element_type : tuple_type->element_types) {
            if (element_type->type != resolved_type::resolved_type_type::FLOAT_TYPE)
                throw type_check_exception("`check_cmd_write`: not an array of length-4 tuples of floats: \""
                                           + command->expr->s_expression() + "\"");
        }
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
            if (element_type->type != type->type)
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
        //  First, make sure that there are array dimension arguments:
        if (expression->binding_pairs.empty())
            throw type_check_exception("`check_expr_array_loop`: no array dimension arguments");

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

        if (!resolved_types_are_equal(left_type, right_type))
            throw type_check_exception("`check_expr_binop`: mismatched types: " + expression->s_expression());

        std::shared_ptr<resolved_type::resolved_type> r_type;
        switch (expression->operator_type) {
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

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_call(const std::shared_ptr<ast_node::call_expr_node>& expression,
                    const symbol_table::symbol_table& sym_tab) {
        const std::string function_name = expression->name;
        if (!sym_tab.has_symbol(function_name))
            throw type_check_exception("`check_expr_call`: undefined function: \"" + expression->s_expression() + "\"");
        const std::shared_ptr<name_info::name_info> info = sym_tab[function_name];
        if (info->name_info_type != name_info::name_info_class::FUNCTION_INFO)
            throw type_check_exception("`check_expr_call`: not a function: \"" + expression->s_expression() + "\"");
        const std::shared_ptr<name_info::function_info> func_info
                = std::reinterpret_pointer_cast<name_info::function_info>(info);

        const unsigned int num_call_args = func_info->call_args.size();
        if (expression->call_args.size() != num_call_args)
            throw type_check_exception("`check_expr_call`: incorrect number of call arguments: \""
                                       + expression->s_expression() + "\"");

        for (unsigned int index = 0; index < num_call_args; index++) {
            const std::shared_ptr<resolved_type::resolved_type> expr_r_type = check_expr(expression->call_args[index],
                                                                                         sym_tab);
            const std::shared_ptr<resolved_type::resolved_type> func_r_type = func_info->call_args[index];
            if (expr_r_type->type != func_r_type->type || expr_r_type->s_expression() != func_r_type->s_expression())
                throw type_check_exception("`check_expr_call`: argument type mismatch: \"" + expression->s_expression()
                                           + "\"");
        }

        const std::shared_ptr<resolved_type::resolved_type> r_type = func_info->r_type;

        expression->r_type = r_type;

        return r_type;
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

        if (!resolved_types_are_equal(affirmative_type, negative_type))
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
        //  First, make sure that there are sum dimension arguments:
        if (expression->binding_pairs.empty())
            throw type_check_exception("`check_expr_sum_loop`: no sum dimension arguments");

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

        if (expression->operator_type == ast_node::op_type::UNOP_NEG && r_type->type != resolved_type::INT_TYPE
            && r_type->type != resolved_type::FLOAT_TYPE)
            throw type_check_exception("`check_expr_unop`: not a number: " + expression->s_expression());

        if (expression->operator_type == ast_node::op_type::UNOP_INV && r_type->type != resolved_type::BOOL_TYPE)
            throw type_check_exception("`check_expr_unop`: not a boolean: " + expression->s_expression());

        expression->r_type = r_type;

        return r_type;
    }

    std::shared_ptr<resolved_type::resolved_type>
    check_expr_variable(const std::shared_ptr<ast_node::variable_expr_node>& expression,
                        const symbol_table::symbol_table& sym_tab) {
        const std::string variable = expression->name;

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

    void check_stmt_assert(const std::shared_ptr<ast_node::assert_stmt_node>& statement,
                           symbol_table::symbol_table& sym_tab) {
        if (check_expr(statement->expr, sym_tab)->type != resolved_type::BOOL_TYPE)
            throw type_check_exception("`check_stmt_assert`: non-boolean expression: "
                                       + statement->expr->s_expression());
    }

    void check_stmt_let(const std::shared_ptr<ast_node::let_stmt_node>& statement,
                        symbol_table::symbol_table& sym_tab) {
        const std::shared_ptr<resolved_type::resolved_type> r_type = check_expr(statement->expr, sym_tab);
        bind_lvalue(statement->lvalue, r_type, sym_tab);
    }

    void check_stmt_return(const std::shared_ptr<ast_node::return_stmt_node>& statement,
                           symbol_table::symbol_table& sym_tab,
                           const std::shared_ptr<resolved_type::resolved_type>& r_type) {
        const std::shared_ptr<resolved_type::resolved_type> expr_r_type = check_expr(statement->return_val, sym_tab);
        if (!resolved_types_are_equal(r_type, expr_r_type))
            throw type_check_exception("`check_cmd_return`: incorrect return type: \""
                                       + statement->return_val->s_expression() + "\"");
        statement->return_val->r_type = expr_r_type;
    }
}  // namespace type_checker
