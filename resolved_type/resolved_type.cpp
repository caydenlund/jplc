/**
 * @file resolved_type.cpp
 * @package Assignment 6
 * @author Cayden Lund (u1182408)
 * @brief Implements the `resolved_type` class and subclasses.
 * @details See the header file for documentation.
 *
 */

#include <sstream>

#include "resolved_type.hpp"

namespace resolved_type {
    bool resolved_type::operator==(const resolved_type& other) const {
        if (this->type != other.type) return false;

        return this->s_expression() == other.s_expression();
    }

    bool resolved_type::operator!=(const resolved_type& other) const { return !(*this == other); }

    std::string resolved_type::s_expression() const {
        switch (type) {
            case BOOL_TYPE:
                return "(BoolType)";
            case FLOAT_TYPE:
                return "(FloatType)";
            case INT_TYPE:
                return "(IntType)";
            default:
                throw std::runtime_error("Tried to call `type_to_string` on invalid type");
        }
    }

    std::string array_resolved_type::s_expression() const {
        std::stringstream result;
        result << "(ArrayType " << this->element_type->s_expression() << " " << this->rank << ")";
        return result.str();
    }

    std::string tuple_resolved_type::s_expression() const {
        std::stringstream result;
        result << "(TupleType";
        for (const std::shared_ptr<resolved_type>& element_type : this->element_types) {
            result << " " << element_type->s_expression();
        }
        result << ")";
        return result.str();
    }
}  //  namespace resolved_type
