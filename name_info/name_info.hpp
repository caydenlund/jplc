/**
 * @file name_info.hpp
 * @package Assignment 7
 * @author Cayden Lund (u1182408)
 * @brief Defines the `name_info` class and subtypes.
 *
 */

#ifndef NAME_INFO_HPP
#define NAME_INFO_HPP

#include "resolved_type/resolved_type.hpp"

namespace name_info {
    /**
     * @brief An enumeration of the different `name_info` classes.
     *
     */
    enum name_info_class {
        FUNCTION_INFO,  //  The `function_info` class.
        TYPE_INFO,      //  The `type_info` class.
        VARIABLE_INFO   //  The `variable_info` class.
    };

    /**
     * @brief Stores information about a symbol.
     *
     */
    struct name_info {
        /**
         * @brief The specific class of this instance.
         *
         */
        name_info_class name_info_type;

        /**
         * @brief Class constructor.
         */
        name_info(name_info_class name_info_type) : name_info_type(name_info_type) {}
    };

    /**
     * @brief Stores information about a function.
     *
     */
    struct function_info : public name_info {
        /**
         * @brief The types of the function call arguments.
         *
         */
        const std::vector<std::shared_ptr<resolved_type::resolved_type>> call_args;

        /**
         * @brief The resolved return type of the function.
         *
         */
        const std::shared_ptr<resolved_type::resolved_type> r_type;

        /**
         * @brief Class constructor.
         * @details Initializes `name_info_type` to `name_info_class::FUNCTION_INFO`.
         *
         * @param r_type The resolved return type of the function.
         */
        function_info(const std::vector<std::shared_ptr<resolved_type::resolved_type>>& call_args,
                      const std::shared_ptr<resolved_type::resolved_type>& r_type)
            : name_info(name_info_class::FUNCTION_INFO), call_args(call_args), r_type(r_type) {}
    };

    /**
     * @brief Stores information about a named type.
     *
     */
    struct type_info : public name_info {
        /**
         * @brief The resolved type of this named type.
         *
         */
        const std::shared_ptr<resolved_type::resolved_type> type;

        /**
         * @brief Class constructor.
         * @details Initializes `name_info_type` to `name_info_class::TYPE_INFO`.
         *
         * @param r_type The resolved type of this named type.
         */
        type_info(const std::shared_ptr<resolved_type::resolved_type>& r_type)
            : name_info(name_info_class::TYPE_INFO), type(r_type) {}
    };

    /**
     * @brief Stores information about a variable.
     *
     */
    struct variable_info : public name_info {
        /**
         * @brief The resolved type of the variable.
         *
         */
        const std::shared_ptr<resolved_type::resolved_type> r_type;

        /**
         * @brief Class constructor.
         * @details Initializes `name_info_type` to `name_info_class::VARIABLE_INFO`.
         *
         * @param r_type The resolved type of this variable.
         */
        variable_info(const std::shared_ptr<resolved_type::resolved_type>& r_type)
            : name_info(name_info_class::VARIABLE_INFO), r_type(r_type) {}
    };
}  //  namespace name_info

#endif
