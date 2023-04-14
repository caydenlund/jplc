/**
 * @file cp_value.cpp
 * @package ast_node
 * @author Cayden Lund (u1182408)
 * @brief Implements the `cp_value` class for constant propagation.
 * @details See header file for documentation.
 *
 */

#include "cp_value.hpp"

namespace ast_node {

    cp_value::cp_value(cp_value_type type) : type(type), int_value(0) {}

    cp_value::cp_value(long value) : type(INT_VALUE), int_value(value) {}

}  //  namespace ast_node
