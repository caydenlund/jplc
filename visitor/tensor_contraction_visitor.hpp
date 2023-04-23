/**
 * @file tensor_contraction_visitor.hpp
 * @author Cayden Lund (u1182408)
 * @brief Defines the AST node visitor class.
 *
 */

#ifndef TENSOR_CONTRACTION_VISITOR_HPP
#define TENSOR_CONTRACTION_VISITOR_HPP

#include "ast_node/ast_node.hpp"
#include "visitor.hpp"

namespace visitor {
    /**
     * @brief Identifies tensor contractions.
     *
     */
    class tensor_contraction_visitor : public visitor {
    public:
        /**
         * @brief Visits the given AST node.
         *
         * @param node The AST node to handle.
         * @return A new node if a change was made; null otherwise.
         */
        std::shared_ptr<ast_node::ast_node> handle_node(const std::shared_ptr<ast_node::ast_node>& node) override;

    private:
        /**
         * @brief Gets all variable edges from the given TC node.
         *
         * @param node The AST node from which to get the variable edges.
         * @param tc_nodes A reference to a vector of variables for array indexing.
         * @return All variables from the given AST node.
         */
        static std::vector<ast_node::tc_edge>
        get_edges(const std::shared_ptr<ast_node::expr_node>& node, std::vector<std::string>& tc_nodes);

        /**
         * @brief Topologically sorts the variables of the given TC node.
         *
         * @param node The TC node to be sorted.
         */
        static void sort_node(const std::shared_ptr<ast_node::array_loop_expr_node>& node);

        /**
         * @brief Visits the given array loop expression node.
         *
         * @param node The array loop node to handle.
         * @return A new node if a change was made; null otherwise.
         */
        std::shared_ptr<ast_node::ast_node>
        handle_node_expr_array_loop(const std::shared_ptr<ast_node::array_loop_expr_node>& node);

        /**
         * @brief Visits the given array index expression node.
         *
         * @param node The array index node to handle.
         * @return A new node if a change was made; null otherwise.
         */
        std::shared_ptr<ast_node::ast_node>
        handle_node_expr_array_index(const std::shared_ptr<ast_node::array_index_expr_node>& node);

        /**
         * @brief Visits the given binary operation expression node.
         *
         * @param node The binary operation expression node to handle.
         * @return A new node if a change was made; null otherwise.
         */
        std::shared_ptr<ast_node::ast_node>
        handle_node_expr_binop(const std::shared_ptr<ast_node::binop_expr_node>& node);

        /**
         * @brief Visits the given float expression node.
         *
         * @param node The float expression node to handle.
         * @return A new node if a change was made; null otherwise.
         */
        std::shared_ptr<ast_node::ast_node>
        handle_node_expr_float(const std::shared_ptr<ast_node::float_expr_node>& node);

        /**
         * @brief Visits the given integer expression node.
         *
         * @param node The integer expression node to handle.
         * @return A new node if a change was made; null otherwise.
         */
        std::shared_ptr<ast_node::ast_node>
        handle_node_expr_integer(const std::shared_ptr<ast_node::integer_expr_node>& node);

        /**
         * @brief Visits the given sum loop expression node.
         *
         * @param node The sum loop node to handle.
         * @return A new node if a change was made; null otherwise.
         */
        std::shared_ptr<ast_node::ast_node>
        handle_node_expr_sum_loop(const std::shared_ptr<ast_node::sum_loop_expr_node>& node);

        /**
         * @brief Visits the given variable expression node.
         *
         * @param node The variable expression node to handle.
         * @return A new node if a change was made; null otherwise.
         */
        std::shared_ptr<ast_node::ast_node>
        handle_node_expr_variable(const std::shared_ptr<ast_node::variable_expr_node>& node);
    };
}  //  namespace visitor

#endif  // TENSOR_CONTRACTION_VISITOR_HPP
