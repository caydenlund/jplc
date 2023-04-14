/**
 * @file cp_value.hpp
 * @package ast_node
 * @author Cayden Lund (u1182408)
 * @brief Defines the `cp_value` class for constant propagation.
 *
 */

#ifndef CP_VALUE_HPP
#define CP_VALUE_HPP

namespace ast_node {

    /**
     * @brief Defines the type of a `cp_value`.
     *
     */
    enum cp_value_type { NULL_VALUE, INT_VALUE };

    /**
     * @brief Defines a value for constant propagation.
     *
     */
    struct cp_value {
        /**
         * @brief Class constructor.
         * @details Default constructor.
         *
         */
        cp_value(cp_value_type type = NULL_VALUE);

        /**
         * @brief Class constructor.
         * @details Constructor for integers.
         *
         * @param value The integer value of this instance.
         */
        cp_value(long value);

        /**
         * @brief The type of this instance.
         *
         */
        cp_value_type type;

        /**
         * @brief The integer value of this instance, if this instance is an integer.
         *
         */
        long int_value;
    };

}  //  namespace ast_node

#endif  //  CP_VALUE_HPP
