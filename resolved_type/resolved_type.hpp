/**
 * @file resolved_type.hpp
 * @package Assignment 6
 * @author Cayden Lund (u1182408)
 * @brief Defines the `resolved_type` class and subclasses.
 *
 */

#ifndef RESOLVED_TYPE_HPP
#define RESOLVED_TYPE_HPP

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace resolved_type {
    /**
     * @brief An enumeration of all resolved types.
     *
     */
    enum resolved_type_type {
        //  Basic types:
        //  ------------
        BOOL_TYPE,   //  Boolean expressions.
        FLOAT_TYPE,  //  Floating-point expressions.
        INT_TYPE,    //  Integer expressions.

        //  Compound types:
        //  ---------------
        ARRAY_TYPE,  //  Array (of variable rank).
        TUPLE_TYPE,  //  Tuple (of variable width).
    };

    /**
     * @brief The resolved type of a single expression AST node.
     *
     */
    struct resolved_type {
        /**
         * @brief The type of this resolved type.
         *
         */
        resolved_type_type type;

        /**
         * @brief Class constructor.
         *
         * @param type The type of this resolved type.
         */
        resolved_type(resolved_type_type type) : type(type) {}

        /**
         * @brief Class destructor.
         *
         */
        virtual ~resolved_type() = default;

        /**
         * @brief Returns the s-expression of this type.
         *
         * @return The s-expression of this type.
         */
        [[nodiscard]] virtual std::string s_expression() const;

        /**
         * @brief Returns the size of this type.
         *
         * @return The size of this type.
         */
        [[nodiscard]] virtual unsigned int size() const;
    };

    /**
     * @brief The resolved type of an array.
     *
     */
    struct array_resolved_type : public resolved_type {
        /**
         * @brief The type of the elements of the array.
         *
         */
        std::shared_ptr<resolved_type> element_type;

        /**
         * @brief The rank of the array.
         *
         */
        unsigned int rank;

        /**
         * @brief Class constructor.
         *
         */
        array_resolved_type(const std::shared_ptr<resolved_type>& element_type, unsigned int rank)
            : resolved_type(resolved_type_type::ARRAY_TYPE), element_type(element_type), rank(rank) {}

        /**
         * @brief Returns the s-expression of this type.
         *
         * @return The s-expression of this type.
         */
        [[nodiscard]] std::string s_expression() const override;

        /**
         * @brief Returns the size of this type.
         *
         * @return The size of this type.
         */
        [[nodiscard]] unsigned int size() const override;
    };

    /**
     * @brief The resolved type of a tuple.
     *
     */
    struct tuple_resolved_type : public resolved_type {
        /**
         * @brief The set of types that make up the tuple type.
         *
         */
        std::vector<std::shared_ptr<resolved_type>> element_types;

        /**
         * @brief Class constructor.
         *
         */
        tuple_resolved_type(const std::vector<std::shared_ptr<resolved_type>>& element_types)
            : resolved_type(resolved_type_type::TUPLE_TYPE), element_types(element_types) {}

        /**
         * @brief Returns the number of offset bytes to access the given element in the tuple.
         *
         * @param index The index of the element to access.
         * @return The number of offset bytes to access the given element.
         */
        [[nodiscard]] unsigned int offset(unsigned int index) const;

        /**
         * @brief Returns the s-expression of this type.
         *
         * @return The s-expression of this type.
         */
        [[nodiscard]] std::string s_expression() const override;

        /**
         * @brief Returns the size of this type.
         *
         * @return The size of this type.
         */
        [[nodiscard]] unsigned int size() const override;
    };
}  //  namespace resolved_type

#endif
