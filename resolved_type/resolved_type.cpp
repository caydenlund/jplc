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

    unsigned int resolved_type::size() const {
        if (this->type != BOOL_TYPE && this->type != FLOAT_TYPE && this->type != INT_TYPE)
            throw std::runtime_error("Tried to call `type_to_string` on invalid type");

        constexpr unsigned int num_bytes = 8;
        return num_bytes;
    }

    std::string array_resolved_type::s_expression() const {
        std::stringstream result;
        result << "(ArrayType " << this->element_type->s_expression() << " " << this->rank << ")";
        return result.str();
    }

    unsigned int array_resolved_type::size() const {
        constexpr unsigned int num_bytes = 8;
        const unsigned int result = num_bytes * (this->rank + 1);  //  Pointer + one size variable for each dimension.

        //  Check for multiplication overflow:
        if ((long long)(num_bytes) * (long long)(this->rank + 1) != result)
            throw std::runtime_error("Overflow error on array construction");

        return result;
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

    unsigned int tuple_resolved_type::size() const {
        unsigned int total = 0;
        for (const std::shared_ptr<resolved_type>& type : this->element_types) { total += type->size(); }
        return total;
    }
}  // namespace resolved_type
