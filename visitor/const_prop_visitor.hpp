/**
 * @file const_prop_visitor.hpp
 * @author Cayden Lund (u1182408)
 * @brief Defines the constant propagation visitor class.
 *
 */

#ifndef CONST_PROP_VISITOR_HPP
#define CONST_PROP_VISITOR_HPP

#include "ast_node/ast_node.hpp"
#include "visitor.hpp"

#include <unordered_map>

namespace visitor {

    /**
     * @brief Propagates constants.
     *
     */
    class const_prop_visitor : public visitor {
    public:
        /**
         * @brief Visits the given expression node.
         *
         * @param node The expression node to handle.
         * @return A new node if a change was made; null otherwise.
         */
        std::shared_ptr<ast_node::ast_node> handle_node(const std::shared_ptr<ast_node::ast_node>& node) override;

    private:
        /**
         * @brief Mapping from a variable name to a `cp_value`.
         *
         */
        std::unordered_map<std::string, ast_node::cp_value> context;

        /**
         * @brief Handles the given `let` command node.
         *
         * @param node The node to handle.
         * @return A new node if a change was made; null otherwise.
         */
        std::shared_ptr<ast_node::ast_node> handle_node_cmd_let(const std::shared_ptr<ast_node::let_cmd_node>& node);

        /**
         * @brief Handles the given `let` statement node.
         *
         * @param node The node to handle.
         * @return A new node if a change was made; null otherwise.
         */
        std::shared_ptr<ast_node::ast_node> handle_node_stmt_let(const std::shared_ptr<ast_node::let_stmt_node>& node);

        /**
         * @brief Handles the given array literal expression node.
         *
         * @param node The node to handle.
         * @return A new node if a change was made; null otherwise.
         */
        std::shared_ptr<ast_node::ast_node>
        handle_node_expr_array_literal(const std::shared_ptr<ast_node::array_literal_expr_node>& node);

        /**
         * @brief Handles the given array loop expression node.
         *
         * @param node The node to handle.
         * @return A new node if a change was made; null otherwise.
         */
        std::shared_ptr<ast_node::ast_node>
        handle_node_expr_array_loop(const std::shared_ptr<ast_node::array_loop_expr_node>& node);

        /**
         * @brief Handles the given integer expression node.
         *
         * @param node The node to handle.
         * @return A new node if a change was made; null otherwise.
         */
        std::shared_ptr<ast_node::ast_node>
        handle_node_expr_integer(const std::shared_ptr<ast_node::integer_expr_node>& node);

        /**
         * @brief Handles the given variable expression node.
         *
         * @param node The node to handle.
         * @return A new node if a change was made; null otherwise.
         */
        std::shared_ptr<ast_node::ast_node>
        handle_node_expr_variable(const std::shared_ptr<ast_node::variable_expr_node>& node);
    };

}  //  namespace visitor

#endif  //  CONST_PROP_VISITOR_HPP
