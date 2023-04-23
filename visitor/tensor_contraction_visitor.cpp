/**
 * @file tensor_contraction_visitor.hpp
 * @author Cayden Lund (u1182408)
 * @brief Implements the tensor contraction visitor class.
 * @details See header file for documentation.
 *
 */

#include <algorithm>

#include "tensor_contraction_visitor.hpp"

namespace visitor {
    std::shared_ptr<ast_node::ast_node>
    tensor_contraction_visitor::handle_node(const std::shared_ptr<ast_node::ast_node>& node) {
        switch (node->type) {
            //  Full tensor contraction:
            case ast_node::ARRAY_LOOP_EXPR:
                return this->handle_node_expr_array_loop(
                        std::reinterpret_pointer_cast<ast_node::array_loop_expr_node>(node));

            //  Sum loop:
            case ast_node::SUM_LOOP_EXPR:
                return this->handle_node_expr_sum_loop(
                        std::reinterpret_pointer_cast<ast_node::sum_loop_expr_node>(node));

            //  Body:
            case ast_node::ARRAY_INDEX_EXPR:
                return this->handle_node_expr_array_index(
                        std::reinterpret_pointer_cast<ast_node::array_index_expr_node>(node));
            case ast_node::BINOP_EXPR:
                return this->handle_node_expr_binop(std::reinterpret_pointer_cast<ast_node::binop_expr_node>(node));
            case ast_node::FLOAT_EXPR:
                return this->handle_node_expr_float(std::reinterpret_pointer_cast<ast_node::float_expr_node>(node));
            case ast_node::INTEGER_EXPR:
                return this->handle_node_expr_integer(std::reinterpret_pointer_cast<ast_node::integer_expr_node>(node));
            case ast_node::VARIABLE_EXPR:
                return this->handle_node_expr_variable(
                        std::reinterpret_pointer_cast<ast_node::variable_expr_node>(node));
            default:
                break;
        }

        return {};
    }

    std::vector<ast_node::tc_edge>
    tensor_contraction_visitor::get_edges(const std::shared_ptr<ast_node::expr_node>& node,
                                          std::vector<std::string>& tc_nodes) {
        using edge_t = ast_node::tc_edge;

        std::vector<edge_t> edges;

        switch (node->type) {
            case ast_node::ARRAY_INDEX_EXPR: {
                const std::shared_ptr<ast_node::array_index_expr_node> array_index_expr
                        = std::reinterpret_pointer_cast<ast_node::array_index_expr_node>(node);

                const unsigned long num_variables = array_index_expr->params.size();
                for (unsigned long left = 0; left < num_variables - 1; ++left) {
                    const std::shared_ptr<ast_node::expr_node> left_param = array_index_expr->params[left];

                    if (left_param->type == ast_node::VARIABLE_EXPR) {
                        const std::shared_ptr<ast_node::variable_expr_node> left_variable
                                = std::reinterpret_pointer_cast<ast_node::variable_expr_node>(left_param);

                        //  The variable must be a TC node.
                        if (std::count(tc_nodes.begin(), tc_nodes.end(), left_variable->name) == 0) break;

                        for (unsigned long right = left + 1; right < num_variables; ++right) {
                            const std::shared_ptr<ast_node::expr_node> right_param = array_index_expr->params[right];

                            if (right_param->type == ast_node::VARIABLE_EXPR) {
                                const std::shared_ptr<ast_node::variable_expr_node> right_variable
                                        = std::reinterpret_pointer_cast<ast_node::variable_expr_node>(right_param);

                                //  The variable must be a TC node.
                                if (std::count(tc_nodes.begin(), tc_nodes.end(), right_variable->name) == 0) break;

                                edges.emplace_back(left_variable->name, right_variable->name);
                            }
                        }
                    }
                }
            } break;
            case ast_node::ARRAY_LOOP_EXPR: {
                const std::shared_ptr<ast_node::array_loop_expr_node> array_loop_expr
                        = std::reinterpret_pointer_cast<ast_node::array_loop_expr_node>(node);

                const unsigned long num_variables = array_loop_expr->binding_pairs.size();
                for (unsigned long left = 0; left < num_variables - 1; ++left) {
                    for (unsigned long right = left + 1; right < num_variables; ++right) {
                        edges.emplace_back(std::get<0>(array_loop_expr->binding_pairs[left]).text,
                                           std::get<0>(array_loop_expr->binding_pairs[right]).text);
                    }
                }

                const std::vector<edge_t> sum_edges = tensor_contraction_visitor::get_edges(array_loop_expr->item_expr,
                                                                                            tc_nodes);
                edges.insert(edges.end(), sum_edges.begin(), sum_edges.end());
            } break;
            case ast_node::BINOP_EXPR: {
                const std::shared_ptr<ast_node::binop_expr_node> binop_expr
                        = std::reinterpret_pointer_cast<ast_node::binop_expr_node>(node);

                const std::vector<edge_t> left_edges = tensor_contraction_visitor::get_edges(binop_expr->left_operand,
                                                                                             tc_nodes);
                edges.insert(edges.end(), left_edges.begin(), left_edges.end());

                const std::vector<edge_t> right_edges = tensor_contraction_visitor::get_edges(binop_expr->right_operand,
                                                                                              tc_nodes);
                edges.insert(edges.end(), right_edges.begin(), right_edges.end());
            } break;
            case ast_node::SUM_LOOP_EXPR: {
                const std::shared_ptr<ast_node::sum_loop_expr_node> sum_loop_expr
                        = std::reinterpret_pointer_cast<ast_node::sum_loop_expr_node>(node);

                if (sum_loop_expr->r_type->type == resolved_type::resolved_type_type::FLOAT_TYPE) {
                    const unsigned long num_variables = sum_loop_expr->binding_pairs.size();
                    for (unsigned long left = 0; left < num_variables - 1; ++left) {
                        for (unsigned long right = left + 1; right < num_variables; ++right) {
                            edges.emplace_back(std::get<0>(sum_loop_expr->binding_pairs[left]).text,
                                               std::get<0>(sum_loop_expr->binding_pairs[right]).text);
                        }
                    }
                }

                const std::vector<edge_t> item_edges = tensor_contraction_visitor::get_edges(sum_loop_expr->sum_expr,
                                                                                             tc_nodes);
                edges.insert(edges.end(), item_edges.begin(), item_edges.end());
            } break;
            default:
                break;
        }

        return edges;
    }

    std::shared_ptr<ast_node::ast_node>
    tensor_contraction_visitor::handle_node_expr_array_loop(  //  NOLINT(readability-convert-member-functions-to-static)
            const std::shared_ptr<ast_node::array_loop_expr_node>& node) {
        //  The expression to evaluate must be a `tc_sum`.
        const bool is_tc = node->item_expr->is_tc_sum;

        using binding_t = ast_node::array_loop_expr_node::binding_pair_t;
        using edge_t = ast_node::tc_edge;

        node->is_tc = is_tc;

        if (is_tc) {
            //  Add all array variables.
            for (const binding_t& binding : node->binding_pairs) {
                node->tc_nodes.emplace_back(std::get<0>(binding).text);
            }

            //  Add all sum variables.
            const std::shared_ptr<ast_node::sum_loop_expr_node> sum_node
                    = std::reinterpret_pointer_cast<ast_node::sum_loop_expr_node>(node->item_expr);
            for (const binding_t& binding : sum_node->binding_pairs) {
                node->tc_nodes.emplace_back(std::get<0>(binding).text);
            }

            //  Add all edges.
            const std::vector<edge_t> edges = tensor_contraction_visitor::get_edges(node, node->tc_nodes);
            node->tc_edges = {edges.begin(), edges.end()};
        }

        return {};
    }

    std::shared_ptr<ast_node::ast_node> tensor_contraction_visitor::
            handle_node_expr_array_index(  //  NOLINT(readability-convert-member-functions-to-static)
                    const std::shared_ptr<ast_node::array_index_expr_node>& node) {
        //  Check the main array.
        if (!node->array->is_tc_primitive) return {};

        //  Check each index expression.
        for (const std::shared_ptr<ast_node::expr_node>& param : node->params) {
            if (!param->is_tc_primitive) return {};
        }

        //  If we made it this far, then this expression is a `tc_body`.
        node->is_tc_body = true;

        return {};
    }

    std::shared_ptr<ast_node::ast_node>
    tensor_contraction_visitor::handle_node_expr_binop(  //  NOLINT(readability-convert-member-functions-to-static)
            const std::shared_ptr<ast_node::binop_expr_node>& node) {
        //  This node is a valid `tc_body` if both operands are `tc_body`s.
        node->is_tc_body = node->left_operand->is_tc_body && node->right_operand->is_tc_body;

        return {};
    }

    std::shared_ptr<ast_node::ast_node>
    tensor_contraction_visitor::handle_node_expr_float(  //  NOLINT(readability-convert-member-functions-to-static)
            const std::shared_ptr<ast_node::float_expr_node>& node) {
        //  Floating-point literals are always `tc_body`s and `tc_primitive`s.
        node->is_tc_body = true;
        node->is_tc_primitive = true;

        return {};
    }

    std::shared_ptr<ast_node::ast_node>
    tensor_contraction_visitor::handle_node_expr_integer(  //  NOLINT(readability-convert-member-functions-to-static)
            const std::shared_ptr<ast_node::integer_expr_node>& node) {
        //  Integer literals are always `tc_body`s and `tc_primitive`s.
        node->is_tc_body = true;
        node->is_tc_primitive = true;

        return {};
    }

    std::shared_ptr<ast_node::ast_node>
    tensor_contraction_visitor::handle_node_expr_sum_loop(  //  NOLINT(readability-convert-member-functions-to-static)
            const std::shared_ptr<ast_node::sum_loop_expr_node>& node) {
        //  The expression to evaluate for each index must be a `tc_body`.
        node->is_tc_sum = node->sum_expr->is_tc_body;

        return {};
    }

    std::shared_ptr<ast_node::ast_node>
    tensor_contraction_visitor::handle_node_expr_variable(  //  NOLINT(readability-convert-member-functions-to-static)
            const std::shared_ptr<ast_node::variable_expr_node>& node) {
        //  Variables are always `tc_body`s and `tc_primitive`s.
        node->is_tc_body = true;
        node->is_tc_primitive = true;

        return {};
    }
}  //  namespace visitor
