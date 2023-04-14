#include "visitor.hpp"

namespace visitor {

    std::shared_ptr<ast_node::ast_node> visitor::visit_node(const std::shared_ptr<ast_node::ast_node>& node) {
        switch (node->type) {
            case ast_node::ARRAY_ARGUMENT: {
                const std::shared_ptr<ast_node::array_argument_node> array_argument
                        = std::reinterpret_pointer_cast<ast_node::array_argument_node>(node);
                return this->handle_node(array_argument);
            }
            case ast_node::VARIABLE_ARGUMENT: {
                const std::shared_ptr<ast_node::variable_argument_node> variable_argument
                        = std::reinterpret_pointer_cast<ast_node::variable_argument_node>(node);
                return this->handle_node(variable_argument);
            }
            case ast_node::TUPLE_BINDING: {
                std::shared_ptr<ast_node::tuple_binding_node> tuple_binding
                        = std::reinterpret_pointer_cast<ast_node::tuple_binding_node>(node);
                std::vector<std::shared_ptr<ast_node::binding_node>> binding_nodes;
                std::shared_ptr<ast_node::ast_node> result;
                bool did_change = false;
                for (const std::shared_ptr<ast_node::binding_node>& binding : tuple_binding->bindings) {
                    result = this->visit_node(binding);
                    if (result == nullptr) {
                        binding_nodes.push_back(binding);
                    } else {
                        binding_nodes.push_back(std::reinterpret_pointer_cast<ast_node::binding_node>(result));
                        did_change = true;
                    }
                }
                if (did_change) tuple_binding = std::make_shared<ast_node::tuple_binding_node>(binding_nodes);
                result = this->handle_node(tuple_binding);
                if (result != nullptr) {
                    tuple_binding = std::reinterpret_pointer_cast<ast_node::tuple_binding_node>(result);
                    did_change = true;
                }
                if (did_change) return tuple_binding;
                return {};
            }
            case ast_node::VAR_BINDING: {
                std::shared_ptr<ast_node::var_binding_node> var_binding
                        = std::reinterpret_pointer_cast<ast_node::var_binding_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result = this->visit_node(var_binding->binding_arg);
                if (result != nullptr) {
                    var_binding = std::make_shared<ast_node::var_binding_node>(
                            std::reinterpret_pointer_cast<ast_node::argument_node>(result), var_binding->binding_type);
                    did_change = true;
                }
                result = this->visit_node(var_binding->binding_type);
                if (result != nullptr) {
                    var_binding = std::make_shared<ast_node::var_binding_node>(
                            var_binding->binding_arg, std::reinterpret_pointer_cast<ast_node::type_node>(result));
                    did_change = true;
                }
                result = this->handle_node(var_binding);
                if (result != nullptr) {
                    var_binding = std::reinterpret_pointer_cast<ast_node::var_binding_node>(result);
                    did_change = true;
                }
                if (did_change) return var_binding;
                return {};
            }
            case ast_node::ASSERT_CMD: {
                std::shared_ptr<ast_node::assert_cmd_node> assert_cmd
                        = std::reinterpret_pointer_cast<ast_node::assert_cmd_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result = this->visit_node(assert_cmd->condition);
                if (result != nullptr) {
                    assert_cmd = std::make_shared<ast_node::assert_cmd_node>(
                            std::reinterpret_pointer_cast<ast_node::expr_node>(result), assert_cmd->text);
                    did_change = true;
                }
                result = this->handle_node(assert_cmd);
                if (result != nullptr) {
                    assert_cmd = std::reinterpret_pointer_cast<ast_node::assert_cmd_node>(result);
                    did_change = true;
                }
                if (did_change) return assert_cmd;
                return {};
            }
            case ast_node::FN_CMD: {
                std::shared_ptr<ast_node::fn_cmd_node> fn_cmd = std::reinterpret_pointer_cast<ast_node::fn_cmd_node>(
                        node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result;
                std::vector<std::shared_ptr<ast_node::binding_node>> bindings;
                for (const std::shared_ptr<ast_node::binding_node>& binding : fn_cmd->bindings) {
                    result = this->visit_node(binding);
                    if (result != nullptr) {
                        bindings.emplace_back(std::reinterpret_pointer_cast<ast_node::binding_node>(result));
                        did_change = true;
                    } else {
                        bindings.emplace_back(binding);
                    }
                }
                if (did_change)
                    fn_cmd = std::make_shared<ast_node::fn_cmd_node>(fn_cmd->name, bindings, fn_cmd->return_type,
                                                                     fn_cmd->statements);
                result = this->visit_node(fn_cmd->return_type);
                if (result != nullptr) {
                    fn_cmd = std::make_shared<ast_node::fn_cmd_node>(
                            fn_cmd->name, fn_cmd->bindings, std::reinterpret_pointer_cast<ast_node::type_node>(result),
                            fn_cmd->statements);
                    did_change = true;
                }
                std::vector<std::shared_ptr<ast_node::stmt_node>> statements;
                for (const std::shared_ptr<ast_node::stmt_node>& statement : fn_cmd->statements) {
                    result = this->visit_node(statement);
                    if (result != nullptr) {
                        statements.emplace_back(std::reinterpret_pointer_cast<ast_node::stmt_node>(result));
                        did_change = true;
                    } else {
                        statements.emplace_back(statement);
                    }
                }
                if (did_change)
                    fn_cmd = std::make_shared<ast_node::fn_cmd_node>(fn_cmd->name, fn_cmd->bindings,
                                                                     fn_cmd->return_type, statements);
                result = this->handle_node(fn_cmd);
                if (result != nullptr) {
                    fn_cmd = std::reinterpret_pointer_cast<ast_node::fn_cmd_node>(result);
                    did_change = true;
                }
                if (did_change) return fn_cmd;
                return {};
            }
            case ast_node::LET_CMD: {
                std::shared_ptr<ast_node::let_cmd_node> let_cmd = std::reinterpret_pointer_cast<ast_node::let_cmd_node>(
                        node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result = this->visit_node(let_cmd->lvalue);
                if (result != nullptr) {
                    let_cmd = std::make_shared<ast_node::let_cmd_node>(
                            std::reinterpret_pointer_cast<ast_node::lvalue_node>(result), let_cmd->expr);
                    did_change = true;
                }
                result = this->visit_node(let_cmd->expr);
                if (result != nullptr) {
                    let_cmd = std::make_shared<ast_node::let_cmd_node>(
                            let_cmd->lvalue, std::reinterpret_pointer_cast<ast_node::expr_node>(result));
                    did_change = true;
                }
                result = this->handle_node(let_cmd);
                if (result != nullptr) {
                    let_cmd = std::reinterpret_pointer_cast<ast_node::let_cmd_node>(result);
                    did_change = true;
                }
                if (did_change) return let_cmd;
                return {};
            }
            case ast_node::PRINT_CMD: {
                const std::shared_ptr<ast_node::print_cmd_node> print_cmd
                        = std::reinterpret_pointer_cast<ast_node::print_cmd_node>(node);
                return this->handle_node(print_cmd);
            }
            case ast_node::READ_CMD: {
                std::shared_ptr<ast_node::read_cmd_node> read_cmd
                        = std::reinterpret_pointer_cast<ast_node::read_cmd_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result = this->visit_node(read_cmd->read_dest);
                if (result != nullptr) {
                    read_cmd = std::make_shared<ast_node::read_cmd_node>(
                            read_cmd->file_name, std::reinterpret_pointer_cast<ast_node::argument_node>(result));
                    did_change = true;
                }
                result = this->handle_node(read_cmd);
                if (result != nullptr) {
                    read_cmd = std::reinterpret_pointer_cast<ast_node::read_cmd_node>(result);
                    did_change = true;
                }
                if (did_change) return read_cmd;
                return {};
            }
            case ast_node::SHOW_CMD: {
                std::shared_ptr<ast_node::show_cmd_node> show_cmd
                        = std::reinterpret_pointer_cast<ast_node::show_cmd_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result = this->visit_node(show_cmd->expr);
                if (result != nullptr) {
                    show_cmd = std::make_shared<ast_node::show_cmd_node>(
                            std::reinterpret_pointer_cast<ast_node::expr_node>(result));
                    did_change = true;
                }
                result = this->handle_node(show_cmd);
                if (result != nullptr) {
                    show_cmd = std::reinterpret_pointer_cast<ast_node::show_cmd_node>(result);
                    did_change = true;
                }
                if (did_change) return show_cmd;
                return {};
            }
            case ast_node::TIME_CMD: {
                std::shared_ptr<ast_node::time_cmd_node> time_cmd
                        = std::reinterpret_pointer_cast<ast_node::time_cmd_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result = this->visit_node(time_cmd->command);
                if (result != nullptr) {
                    time_cmd = std::make_shared<ast_node::time_cmd_node>(
                            std::reinterpret_pointer_cast<ast_node::cmd_node>(result));
                    did_change = true;
                }
                result = this->handle_node(time_cmd);
                if (result != nullptr) {
                    time_cmd = std::reinterpret_pointer_cast<ast_node::time_cmd_node>(result);
                    did_change = true;
                }
                if (did_change) return time_cmd;
                return {};
            }
            case ast_node::TYPE_CMD: {
                std::shared_ptr<ast_node::type_cmd_node> type_cmd
                        = std::reinterpret_pointer_cast<ast_node::type_cmd_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result = this->visit_node(type_cmd->new_type);
                if (result != nullptr) {
                    type_cmd = std::make_shared<ast_node::type_cmd_node>(
                            type_cmd->name, std::reinterpret_pointer_cast<ast_node::type_node>(result));
                    did_change = true;
                }
                result = this->handle_node(type_cmd);
                if (result != nullptr) {
                    type_cmd = std::reinterpret_pointer_cast<ast_node::type_cmd_node>(result);
                    did_change = true;
                }
                if (did_change) return type_cmd;
                return {};
            }
            case ast_node::WRITE_CMD: {
                std::shared_ptr<ast_node::write_cmd_node> write_cmd
                        = std::reinterpret_pointer_cast<ast_node::write_cmd_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result = this->visit_node(write_cmd->expr);
                if (result != nullptr) {
                    write_cmd = std::make_shared<ast_node::write_cmd_node>(
                            std::reinterpret_pointer_cast<ast_node::expr_node>(result), write_cmd->file_name);
                    did_change = true;
                }
                result = this->handle_node(write_cmd);
                if (result != nullptr) {
                    write_cmd = std::reinterpret_pointer_cast<ast_node::write_cmd_node>(result);
                    did_change = true;
                }
                if (did_change) return write_cmd;
                return {};
            }
            case ast_node::ARRAY_INDEX_EXPR: {
                std::shared_ptr<ast_node::array_index_expr_node> array_index_expr
                        = std::reinterpret_pointer_cast<ast_node::array_index_expr_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result;
                std::vector<std::shared_ptr<ast_node::expr_node>> params;
                for (const std::shared_ptr<ast_node::expr_node>& param : array_index_expr->params) {
                    result = this->visit_node(param);
                    if (result != nullptr) {
                        params.emplace_back(std::reinterpret_pointer_cast<ast_node::expr_node>(result));
                        did_change = true;
                    } else {
                        params.emplace_back(param);
                    }
                }
                if (did_change)
                    array_index_expr = std::make_shared<ast_node::array_index_expr_node>(array_index_expr->array,
                                                                                         params);
                result = this->visit_node(array_index_expr->array);
                if (result != nullptr) {
                    array_index_expr = std::make_shared<ast_node::array_index_expr_node>(
                            std::reinterpret_pointer_cast<ast_node::expr_node>(result), array_index_expr->params);
                    did_change = true;
                }
                result = this->handle_node(array_index_expr);
                if (result != nullptr) {
                    array_index_expr = std::reinterpret_pointer_cast<ast_node::array_index_expr_node>(result);
                    did_change = true;
                }
                if (did_change) return array_index_expr;
                return {};
            }
            case ast_node::ARRAY_LITERAL_EXPR: {
                std::shared_ptr<ast_node::array_literal_expr_node> array_literal_expr
                        = std::reinterpret_pointer_cast<ast_node::array_literal_expr_node>(node);
                bool did_change = false;
                std::vector<std::shared_ptr<ast_node::expr_node>> expressions;
                std::shared_ptr<ast_node::ast_node> result;
                for (const std::shared_ptr<ast_node::expr_node>& expression : array_literal_expr->expressions) {
                    result = this->visit_node(expression);
                    if (result != nullptr) {
                        expressions.emplace_back(std::reinterpret_pointer_cast<ast_node::expr_node>(result));
                        did_change = true;
                    } else {
                        expressions.emplace_back(expression);
                    }
                }
                if (did_change) array_literal_expr = std::make_shared<ast_node::array_literal_expr_node>(expressions);
                result = this->handle_node(array_literal_expr);
                if (result != nullptr) {
                    array_literal_expr = std::reinterpret_pointer_cast<ast_node::array_literal_expr_node>(result);
                    did_change = true;
                }
                if (did_change) return array_literal_expr;
                return {};
            }
            case ast_node::ARRAY_LOOP_EXPR: {
                std::shared_ptr<ast_node::array_loop_expr_node> array_loop_expr
                        = std::reinterpret_pointer_cast<ast_node::array_loop_expr_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result;
                std::vector<ast_node::array_loop_expr_node::binding_pair_t> binding_pairs;
                for (const auto& binding_pair : array_loop_expr->binding_pairs) {
                    result = this->visit_node(std::get<1>(binding_pair));
                    if (result != nullptr) {
                        binding_pairs.emplace_back(std::get<0>(binding_pair),
                                                   std::reinterpret_pointer_cast<ast_node::expr_node>(result));
                        did_change = true;
                    } else {
                        binding_pairs.emplace_back(binding_pair);
                    }
                }
                if (did_change)
                    array_loop_expr = std::make_shared<ast_node::array_loop_expr_node>(binding_pairs,
                                                                                       array_loop_expr->item_expr);
                result = this->visit_node(array_loop_expr->item_expr);
                if (result != nullptr) {
                    array_loop_expr = std::make_shared<ast_node::array_loop_expr_node>(
                            array_loop_expr->binding_pairs, std::reinterpret_pointer_cast<ast_node::expr_node>(result));
                    did_change = true;
                }
                result = this->handle_node(array_loop_expr);
                if (result != nullptr) {
                    array_loop_expr = std::reinterpret_pointer_cast<ast_node::array_loop_expr_node>(result);
                    did_change = true;
                }
                if (did_change) return array_loop_expr;
                return {};
            }
            case ast_node::BINOP_EXPR: {
                std::shared_ptr<ast_node::binop_expr_node> binop_expr
                        = std::reinterpret_pointer_cast<ast_node::binop_expr_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result = this->visit_node(binop_expr->left_operand);
                if (result != nullptr) {
                    binop_expr = std::make_shared<ast_node::binop_expr_node>(
                            binop_expr->operator_type, std::reinterpret_pointer_cast<ast_node::expr_node>(result),
                            binop_expr->right_operand);
                    did_change = true;
                }
                result = this->visit_node(binop_expr->right_operand);
                if (result != nullptr) {
                    binop_expr = std::make_shared<ast_node::binop_expr_node>(
                            binop_expr->operator_type, binop_expr->left_operand,
                            std::reinterpret_pointer_cast<ast_node::expr_node>(result));
                    did_change = true;
                }
                result = this->handle_node(binop_expr);
                if (result != nullptr) {
                    binop_expr = std::reinterpret_pointer_cast<ast_node::binop_expr_node>(result);
                    did_change = true;
                }
                if (did_change) return binop_expr;
                return {};
            }
            case ast_node::CALL_EXPR: {
                std::shared_ptr<ast_node::call_expr_node> call_expr
                        = std::reinterpret_pointer_cast<ast_node::call_expr_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result;
                std::vector<std::shared_ptr<ast_node::expr_node>> call_args;
                for (const std::shared_ptr<ast_node::expr_node>& arg : call_expr->call_args) {
                    result = this->visit_node(arg);
                    if (result != nullptr) {
                        call_args.emplace_back(std::reinterpret_pointer_cast<ast_node::expr_node>(result));
                        did_change = true;
                    } else {
                        call_args.emplace_back(arg);
                    }
                }
                if (did_change) call_expr = std::make_shared<ast_node::call_expr_node>(call_expr->name, call_args);
                result = this->handle_node(call_expr);
                if (result != nullptr) {
                    call_expr = std::reinterpret_pointer_cast<ast_node::call_expr_node>(result);
                    did_change = true;
                }
                if (did_change) return call_expr;
                return {};
            }
            case ast_node::FALSE_EXPR: {
                const std::shared_ptr<ast_node::false_expr_node> false_expr
                        = std::reinterpret_pointer_cast<ast_node::false_expr_node>(node);
                return this->handle_node(false_expr);
            }
            case ast_node::FLOAT_EXPR: {
                const std::shared_ptr<ast_node::float_expr_node> float_expr
                        = std::reinterpret_pointer_cast<ast_node::float_expr_node>(node);
                return this->handle_node(float_expr);
            }
            case ast_node::IF_EXPR: {
                std::shared_ptr<ast_node::if_expr_node> if_expr = std::reinterpret_pointer_cast<ast_node::if_expr_node>(
                        node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result = this->visit_node(if_expr->conditional_expr);
                if (result != nullptr) {
                    if_expr = std::make_shared<ast_node::if_expr_node>(
                            std::reinterpret_pointer_cast<ast_node::expr_node>(result), if_expr->affirmative_expr,
                            if_expr->negative_expr);
                    did_change = true;
                }
                result = this->visit_node(if_expr->affirmative_expr);
                if (result != nullptr) {
                    if_expr = std::make_shared<ast_node::if_expr_node>(
                            if_expr->conditional_expr, std::reinterpret_pointer_cast<ast_node::expr_node>(result),
                            if_expr->negative_expr);
                    did_change = true;
                }
                result = this->visit_node(if_expr->negative_expr);
                if (result != nullptr) {
                    if_expr = std::make_shared<ast_node::if_expr_node>(
                            if_expr->conditional_expr, if_expr->affirmative_expr,
                            std::reinterpret_pointer_cast<ast_node::expr_node>(result));
                    did_change = true;
                }
                result = this->handle_node(if_expr);
                if (result != nullptr) {
                    if_expr = std::reinterpret_pointer_cast<ast_node::if_expr_node>(result);
                    did_change = true;
                }
                if (did_change) return if_expr;
                return {};
            }
            case ast_node::INTEGER_EXPR: {
                const std::shared_ptr<ast_node::integer_expr_node> integer_expr
                        = std::reinterpret_pointer_cast<ast_node::integer_expr_node>(node);
                return this->handle_node(integer_expr);
            }
            case ast_node::SUM_LOOP_EXPR: {
                std::shared_ptr<ast_node::sum_loop_expr_node> sum_loop_expr
                        = std::reinterpret_pointer_cast<ast_node::sum_loop_expr_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result;
                std::vector<ast_node::array_loop_expr_node::binding_pair_t> binding_pairs;
                for (const auto& binding_pair : sum_loop_expr->binding_pairs) {
                    result = this->visit_node(std::get<1>(binding_pair));
                    if (result != nullptr) {
                        binding_pairs.emplace_back(std::get<0>(binding_pair),
                                                   std::reinterpret_pointer_cast<ast_node::expr_node>(result));
                        did_change = true;
                    } else {
                        binding_pairs.emplace_back(binding_pair);
                    }
                }
                if (did_change)
                    sum_loop_expr = std::make_shared<ast_node::sum_loop_expr_node>(
                            binding_pairs, std::reinterpret_pointer_cast<ast_node::expr_node>(sum_loop_expr->sum_expr));

                result = this->visit_node(sum_loop_expr->sum_expr);
                if (result != nullptr) {
                    sum_loop_expr = std::make_shared<ast_node::sum_loop_expr_node>(
                            binding_pairs, std::reinterpret_pointer_cast<ast_node::expr_node>(result));
                    did_change = true;
                }
                result = this->handle_node(sum_loop_expr);
                if (result != nullptr) {
                    sum_loop_expr = std::reinterpret_pointer_cast<ast_node::sum_loop_expr_node>(result);
                    did_change = true;
                }
                if (did_change) return sum_loop_expr;
                return {};
            }
            case ast_node::TRUE_EXPR: {
                const std::shared_ptr<ast_node::true_expr_node> true_expr
                        = std::reinterpret_pointer_cast<ast_node::true_expr_node>(node);
                return this->handle_node(true_expr);
            }
            case ast_node::TUPLE_INDEX_EXPR: {
                std::shared_ptr<ast_node::tuple_index_expr_node> tuple_index_expr
                        = std::reinterpret_pointer_cast<ast_node::tuple_index_expr_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result = this->visit_node(tuple_index_expr->expr);
                if (result != nullptr) {
                    tuple_index_expr = std::make_shared<ast_node::tuple_index_expr_node>(
                            std::reinterpret_pointer_cast<ast_node::expr_node>(result), tuple_index_expr->index);
                    did_change = true;
                }
                result = this->visit_node(tuple_index_expr->index);
                if (result != nullptr) {
                    tuple_index_expr = std::make_shared<ast_node::tuple_index_expr_node>(
                            tuple_index_expr->expr, std::reinterpret_pointer_cast<ast_node::integer_expr_node>(result));
                    did_change = true;
                }
                result = this->handle_node(tuple_index_expr);
                if (result != nullptr) {
                    tuple_index_expr = std::reinterpret_pointer_cast<ast_node::tuple_index_expr_node>(result);
                    did_change = true;
                }
                if (did_change) return tuple_index_expr;
                return {};
            }
            case ast_node::TUPLE_LITERAL_EXPR: {
                std::shared_ptr<ast_node::tuple_literal_expr_node> tuple_literal_expr
                        = std::reinterpret_pointer_cast<ast_node::tuple_literal_expr_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result;
                std::vector<std::shared_ptr<ast_node::expr_node>> exprs;
                for (const std::shared_ptr<ast_node::expr_node>& expr : tuple_literal_expr->exprs) {
                    result = this->visit_node(expr);
                    if (result != nullptr) {
                        exprs.emplace_back(std::reinterpret_pointer_cast<ast_node::expr_node>(result));
                        did_change = true;
                    } else {
                        exprs.emplace_back(expr);
                    }
                }
                if (did_change) tuple_literal_expr = std::make_shared<ast_node::tuple_literal_expr_node>(exprs);
                result = this->handle_node(tuple_literal_expr);
                if (result != nullptr) {
                    tuple_literal_expr = std::reinterpret_pointer_cast<ast_node::tuple_literal_expr_node>(result);
                    did_change = true;
                }
                if (did_change) return tuple_literal_expr;
                return {};
            }
            case ast_node::UNOP_EXPR: {
                std::shared_ptr<ast_node::unop_expr_node> unop_expr
                        = std::reinterpret_pointer_cast<ast_node::unop_expr_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result = this->visit_node(unop_expr->operand);
                if (result != nullptr) {
                    unop_expr = std::make_shared<ast_node::unop_expr_node>(
                            unop_expr->operator_type, std::reinterpret_pointer_cast<ast_node::expr_node>(result));
                    did_change = true;
                }
                result = this->handle_node(unop_expr);
                if (result != nullptr) {
                    unop_expr = std::reinterpret_pointer_cast<ast_node::unop_expr_node>(result);
                    did_change = true;
                }
                if (did_change) return unop_expr;
                return {};
            }
            case ast_node::VARIABLE_EXPR: {
                const std::shared_ptr<ast_node::variable_expr_node> variable_expr
                        = std::reinterpret_pointer_cast<ast_node::variable_expr_node>(node);
                return this->handle_node(variable_expr);
            }
            case ast_node::ARGUMENT_LVALUE: {
                std::shared_ptr<ast_node::argument_lvalue_node> argument_lvalue
                        = std::reinterpret_pointer_cast<ast_node::argument_lvalue_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result = this->visit_node(argument_lvalue->argument);
                if (result != nullptr) {
                    argument_lvalue = std::make_shared<ast_node::argument_lvalue_node>(
                            std::reinterpret_pointer_cast<ast_node::argument_node>(result));
                    did_change = true;
                }
                result = this->handle_node(argument_lvalue);
                if (result != nullptr) {
                    argument_lvalue = std::reinterpret_pointer_cast<ast_node::argument_lvalue_node>(result);
                    did_change = true;
                }
                if (did_change) return argument_lvalue;
                return {};
            }
            case ast_node::TUPLE_LVALUE: {
                std::shared_ptr<ast_node::tuple_lvalue_node> tuple_lvalue
                        = std::reinterpret_pointer_cast<ast_node::tuple_lvalue_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result;
                std::vector<std::shared_ptr<ast_node::lvalue_node>> lvalues;
                for (const std::shared_ptr<ast_node::lvalue_node>& lvalue : tuple_lvalue->lvalues) {
                    result = this->visit_node(lvalue);
                    if (result != nullptr) {
                        lvalues.emplace_back(std::reinterpret_pointer_cast<ast_node::lvalue_node>(result));
                        did_change = true;
                    } else {
                        lvalues.emplace_back(lvalue);
                    }
                }
                if (did_change) tuple_lvalue = std::make_shared<ast_node::tuple_lvalue_node>(lvalues);
                result = this->handle_node(tuple_lvalue);
                if (result != nullptr) {
                    tuple_lvalue = std::reinterpret_pointer_cast<ast_node::tuple_lvalue_node>(result);
                    did_change = true;
                }
                if (did_change) return tuple_lvalue;
                return {};
            }
            case ast_node::ASSERT_STMT: {
                std::shared_ptr<ast_node::assert_stmt_node> assert_stmt
                        = std::reinterpret_pointer_cast<ast_node::assert_stmt_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result = this->visit_node(assert_stmt->expr);
                if (result != nullptr) {
                    assert_stmt = std::make_shared<ast_node::assert_stmt_node>(
                            std::reinterpret_pointer_cast<ast_node::expr_node>(result), assert_stmt->text);
                    did_change = true;
                }
                result = this->handle_node(assert_stmt);
                if (result != nullptr) {
                    assert_stmt = std::reinterpret_pointer_cast<ast_node::assert_stmt_node>(result);
                    did_change = true;
                }
                if (did_change) return assert_stmt;
                return {};
            }
            case ast_node::LET_STMT: {
                std::shared_ptr<ast_node::let_stmt_node> let_stmt
                        = std::reinterpret_pointer_cast<ast_node::let_stmt_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result = this->visit_node(let_stmt->lvalue);
                if (result != nullptr) {
                    let_stmt = std::make_shared<ast_node::let_stmt_node>(
                            std::reinterpret_pointer_cast<ast_node::lvalue_node>(result), let_stmt->expr);
                    did_change = true;
                }
                result = this->visit_node(let_stmt->expr);
                if (result != nullptr) {
                    let_stmt = std::make_shared<ast_node::let_stmt_node>(
                            let_stmt->lvalue, std::reinterpret_pointer_cast<ast_node::expr_node>(result));
                    did_change = true;
                }
                result = this->handle_node(let_stmt);
                if (result != nullptr) {
                    let_stmt = std::reinterpret_pointer_cast<ast_node::let_stmt_node>(result);
                    did_change = true;
                }
                if (did_change) return let_stmt;
                return {};
            }
            case ast_node::RETURN_STMT: {
                std::shared_ptr<ast_node::return_stmt_node> return_stmt
                        = std::reinterpret_pointer_cast<ast_node::return_stmt_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result = this->visit_node(return_stmt->return_val);
                if (result != nullptr) {
                    return_stmt = std::make_shared<ast_node::return_stmt_node>(
                            std::reinterpret_pointer_cast<ast_node::expr_node>(result));
                    did_change = true;
                }
                result = this->handle_node(return_stmt);
                if (result != nullptr) {
                    return_stmt = std::reinterpret_pointer_cast<ast_node::return_stmt_node>(result);
                    did_change = true;
                }
                if (did_change) return return_stmt;
                return {};
            }
            case ast_node::ARRAY_TYPE: {
                std::shared_ptr<ast_node::array_type_node> array_type
                        = std::reinterpret_pointer_cast<ast_node::array_type_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result = this->visit_node(array_type->main_type);
                if (result != nullptr) {
                    array_type = std::make_shared<ast_node::array_type_node>(
                            std::reinterpret_pointer_cast<ast_node::type_node>(result), array_type->dimensions);
                    did_change = true;
                }
                result = this->handle_node(array_type);
                if (result != nullptr) {
                    array_type = std::reinterpret_pointer_cast<ast_node::array_type_node>(result);
                    did_change = true;
                }
                if (did_change) return array_type;
                return {};
            }
            case ast_node::BOOL_TYPE: {
                const std::shared_ptr<ast_node::bool_type_node> bool_type
                        = std::reinterpret_pointer_cast<ast_node::bool_type_node>(node);
                return this->handle_node(bool_type);
            }
            case ast_node::FLOAT_TYPE: {
                const std::shared_ptr<ast_node::float_type_node> float_type
                        = std::reinterpret_pointer_cast<ast_node::float_type_node>(node);
                return this->handle_node(float_type);
            }
            case ast_node::INT_TYPE: {
                const std::shared_ptr<ast_node::int_type_node> int_type
                        = std::reinterpret_pointer_cast<ast_node::int_type_node>(node);
                return this->handle_node(int_type);
            }
            case ast_node::TUPLE_TYPE: {
                std::shared_ptr<ast_node::tuple_type_node> tuple_type
                        = std::reinterpret_pointer_cast<ast_node::tuple_type_node>(node);
                bool did_change = false;
                std::shared_ptr<ast_node::ast_node> result;
                std::vector<std::shared_ptr<ast_node::type_node>> types;
                for (const std::shared_ptr<ast_node::type_node>& type : tuple_type->types) {
                    result = this->visit_node(type);
                    if (result != nullptr) {
                        types.emplace_back(std::reinterpret_pointer_cast<ast_node::type_node>(result));
                        did_change = true;
                    } else {
                        types.emplace_back(type);
                    }
                }
                if (did_change) tuple_type = std::make_shared<ast_node::tuple_type_node>(types);
                result = this->handle_node(tuple_type);
                if (result != nullptr) {
                    tuple_type = std::reinterpret_pointer_cast<ast_node::tuple_type_node>(result);
                    did_change = true;
                }
                if (did_change) return tuple_type;
                return {};
            }
            case ast_node::VARIABLE_TYPE: {
                const std::shared_ptr<ast_node::variable_type_node> variable_type
                        = std::reinterpret_pointer_cast<ast_node::variable_type_node>(node);
                return this->handle_node(variable_type);
            }
            default:
                break;
        }

        throw std::runtime_error("`visit_node`: invalid node: \"" + node->s_expression() + "\"");
    }

    std::shared_ptr<ast_node::ast_node> visitor::handle_node(const std::shared_ptr<ast_node::ast_node>&) { return {}; }

}  //  namespace visitor
