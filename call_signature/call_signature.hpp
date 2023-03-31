/**
 * @file call_signature.hpp
 * @package Assignment 10
 * @author Cayden Lund (u1182408)
 * @brief Defines the `call_signature` class.
 *
 */

#ifndef CALL_SIGNATURE_HPP
#define CALL_SIGNATURE_HPP

#include <memory>
#include <vector>

#include "ast_node/ast_node.hpp"
#include "resolved_type/resolved_type.hpp"

namespace call_signature {
    /**
     * @brief Describes the call signature of a function.
     *
     */
    struct call_signature {
        /**
         * @brief The list of all arguments, in the order to save.
         * @details Each argument is encoded as a tuple of {resolved_type, is_register, register}.
         *
         */
        std::vector<std::tuple<std::shared_ptr<resolved_type::resolved_type>, bool, std::string>> all_args;

        /**
         * @brief The number of bytes that will be pushed onto the stack to call this function.
         * @details Does not include arguments that lie in registers,
         *          or the return value if the return value is a struct.
         *
         */
        unsigned int bytes_on_stack;

        /**
         * @brief The size of each stack argument.
         *
         */
        std::vector<long> stack_args;

        /**
         * @brief The order that the arguments should be pushed onto the stack.
         *
         */
        std::vector<unsigned int> push_order;

        /**
         * @brief Assembly for popping all arguments from the stack into the proper registers.
         *
         */
        std::vector<std::string> pop_assem;

        /**
         * @brief The return type of the function.
         *
         */
        std::shared_ptr<resolved_type::resolved_type> ret_type;

        /**
         * @brief Default constructor.
         *
         */
        call_signature();

        /**
         * @brief Class constructor.
         *
         * @param arg_types The types of the arguments to be passed to the function.
         * @param ret_type The return type of the function.
         */
        call_signature(const std::vector<std::shared_ptr<resolved_type::resolved_type>>& arg_types,
                       const std::shared_ptr<resolved_type::resolved_type>& ret_type);
    };
}  //  namespace call_signature

#endif
