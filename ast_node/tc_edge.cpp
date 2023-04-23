/**
 * @file tc_edge.cpp
 * @author Cayden Lund (u1182408)
 * @brief Implements the AST node tensor contraction edge struct methods.
 * @details See the header file for documentation.
 *
 */

#include "tc_edge.hpp"

namespace ast_node {
    tc_edge::tc_edge(const std::string& from_node, const std::string& to_node)
        : from_node(from_node), to_node(to_node), hash(from_node + " -> " + to_node) {}

    bool tc_edge::operator==(const tc_edge& right_node) const { return hash == right_node.hash; }
}  //  namespace ast_node

namespace std {
    size_t hash<ast_node::tc_edge>::operator()(const ast_node::tc_edge& edge) const {
        return hash<std::string>()(edge.hash);
    }
}  //  namespace std
