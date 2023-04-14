/**
 * @file const_prop_visitor.hpp
 * @author Cayden Lund (u1182408)
 * @brief Implements the constant propagation visitor class.
 * @details See header file for documentation.
 *
 */

#include "const_prop_visitor.hpp"

namespace visitor {

    std::shared_ptr<ast_node::ast_node>
    const_prop_visitor::handle_node(const std::shared_ptr<ast_node::ast_node>& node) {
        switch (node->type) {
            case ast_node::node_type::LET_CMD:
                return this->handle_node_cmd_let(std::reinterpret_pointer_cast<ast_node::let_cmd_node>(node));
            case ast_node::node_type::LET_STMT:
                return this->handle_node_stmt_let(std::reinterpret_pointer_cast<ast_node::let_stmt_node>(node));
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
    const_prop_visitor::handle_node_cmd_let(const std::shared_ptr<ast_node::let_cmd_node>& node) {
        if (node->lvalue->type == ast_node::node_type::ARGUMENT_LVALUE) {
            const ast_node::cp_value& cp_val = node->expr->cp_val;
            if (cp_val.type == ast_node::NULL_VALUE) return {};

            const std::shared_ptr<ast_node::argument_node> argument
                    = std::reinterpret_pointer_cast<ast_node::argument_lvalue_node>(node->lvalue)->argument;

            if (argument->type == ast_node::node_type::VARIABLE_ARGUMENT) {
                const std::shared_ptr<ast_node::variable_argument_node> variable_argument
                        = std::reinterpret_pointer_cast<ast_node::variable_argument_node>(argument);

                this->context[variable_argument->name] = cp_val;
            }
        }

        return {};
    }

    std::shared_ptr<ast_node::ast_node>
    const_prop_visitor::handle_node_stmt_let(const std::shared_ptr<ast_node::let_stmt_node>& node) {
        if (node->lvalue->type == ast_node::node_type::ARGUMENT_LVALUE) {
            if (node->expr->cp_val.type == ast_node::NULL_VALUE) return {};

            const std::shared_ptr<ast_node::argument_node> argument
                    = std::reinterpret_pointer_cast<ast_node::argument_lvalue_node>(node->lvalue)->argument;

            if (argument->type == ast_node::node_type::VARIABLE_ARGUMENT) {
                const std::shared_ptr<ast_node::variable_argument_node> variable_argument
                        = std::reinterpret_pointer_cast<ast_node::variable_argument_node>(argument);

                this->context[variable_argument->name] = node->expr->cp_val;
            }
        }

        return {};
    }

    std::shared_ptr<ast_node::ast_node>
    const_prop_visitor::handle_node_expr_integer(  //  NOLINT(readability-convert-member-functions-to-static)
            const std::shared_ptr<ast_node::integer_expr_node>& node) {
        node->cp_val = node->value;

        return {};
    }

    std::shared_ptr<ast_node::ast_node>
    const_prop_visitor::handle_node_expr_variable(const std::shared_ptr<ast_node::variable_expr_node>& node) {
        const ast_node::cp_value cp_val = this->context[node->name];

        if (cp_val.type == ast_node::NULL_VALUE) return {};

        node->cp_val = cp_val;

        return {};
    }

}  //  namespace visitor
