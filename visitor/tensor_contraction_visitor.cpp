/**
 * @file tensor_contraction_visitor.hpp
 * @author Cayden Lund (u1182408)
 * @brief Implements the tensor contraction visitor class.
 * @details See header file for documentation.
 *
 */

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

    std::shared_ptr<ast_node::ast_node>
    tensor_contraction_visitor::handle_node_expr_array_loop(  //  NOLINT(readability-convert-member-functions-to-static)
            const std::shared_ptr<ast_node::array_loop_expr_node>& node) {
        //  The expression to evaluate must be a `tc_sum`.
        node->is_tc = node->item_expr->is_tc_sum;

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
