/**
 * @file visitor.hpp
 * @author Cayden Lund (u1182408)
 * @brief Defines the AST node visitor class.
 *
 */

#ifndef VISITOR_HPP
#define VISITOR_HPP

#include "ast_node/ast_node.hpp"

namespace visitor {

    /**
     * @brief This is the main visitor class. Override it to create visitors with specific tasks.
     *
     */
    class visitor {
    public:
        /**
         * @brief Class destructor.
         *
         */
        virtual ~visitor() = default;

        /**
         * @brief Visits the given AST node.
         *
         * @param node The AST node to visit.
         * @return A new node if a change was made; null otherwise.
         */
        std::shared_ptr<ast_node::ast_node> visit_node(const std::shared_ptr<ast_node::ast_node>& node);

        /**
         * @brief Handles the given AST node.
         * @details Override this method to handle the given AST node.
         *
         * @param node The AST node to handle.
         * @return A new node if a change was made; null otherwise.
         */
        virtual std::shared_ptr<ast_node::ast_node> handle_node(const std::shared_ptr<ast_node::ast_node>& node);
    };

}  //  namespace visitor

#endif  //  VISITOR_HPP
