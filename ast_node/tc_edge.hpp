/**
 * @file tc_edge.hpp
 * @author Cayden Lund (u1182408)
 * @brief Defines the AST node tensor contraction edge struct.
 *
 */

#ifndef TC_EDGE_HPP
#define TC_EDGE_HPP

#include <string>

namespace ast_node {
    /**
     * @brief Represents a tensor contraction edge.
     *
     */
    struct tc_edge {
        /**
         * @brief Class constructor.
         *
         * @param from_node The "from" node.
         * @param to_node The "to" node.
         */
        tc_edge(const std::string& from_node, const std::string& to_node);

        /**
         * @brief Equality operator.
         *
         * @param left_node The left node.
         * @param right_node The right node.
         * @return True if the two nodes are equal; false otherwise.
         */
        bool operator==(const tc_edge& right_node) const;

        /**
         * @brief The "from" node.
         *
         */
        std::string from_node;

        /**
         * @brief The "to" node.
         *
         */
        std::string to_node;

        /**
         * @brief The string for hashing.
         *
         */
        std::string hash;
    };
}  //  namespace ast_node

namespace std {
    /**
     * @brief Defines the hashing function for the edge tensor struct.
     *
     */
    template<>
    struct hash<ast_node::tc_edge> {
        /**
         * @brief The functor call operator.
         *
         * @param edge The edge tensor to hash.
         * @return A unique hash of the edge tensor.
         */
        size_t operator()(const ast_node::tc_edge& edge) const;
    };
}  //  namespace std

#endif  //  TC_EDGE_HPP
