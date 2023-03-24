/**
 * @file call_signature.cpp
 * @package Assignment 10
 * @author Cayden Lund (u1182408)
 * @brief Implements the `call_signature` class.
 * @details See the header file for documentation.
 *
 */

#include <sstream>
#include <tuple>

#include "call_signature.hpp"

namespace call_signature {
    call_signature::call_signature() : bytes_on_stack(0) {}

    call_signature::call_signature(const std::vector<std::shared_ptr<resolved_type::resolved_type>>& arg_types,
                                   const std::shared_ptr<resolved_type::resolved_type>& ret_type)
        : bytes_on_stack(0), ret_type(ret_type) {
        constexpr unsigned int reg_size = 8;

        unsigned int total_ints = 0;
        unsigned int total_floats = 0;

        enum reg_t { INT, FLOAT };

        std::vector<std::tuple<unsigned int, reg_t, std::string>> reg_args;
        std::vector<unsigned int> stack_args;

        if (this->ret_type->type == resolved_type::ARRAY_TYPE || this->ret_type->type == resolved_type::TUPLE_TYPE) {
            ++total_ints;
        }

        const std::vector<std::string> int_regs = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
        const std::vector<std::string> float_regs = {"xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"};

        for (unsigned int arg_index = 0; arg_index < arg_types.size(); ++arg_index) {
            const std::shared_ptr<resolved_type::resolved_type>& arg_type = arg_types[arg_index];

            switch (arg_type->type) {
                case resolved_type::BOOL_TYPE:
                case resolved_type::INT_TYPE:
                    if (++total_ints > int_regs.size()) {
                        stack_args.emplace_back(arg_index);
                        this->bytes_on_stack += reg_size;
                    } else {
                        reg_args.emplace_back(arg_index, reg_t::INT, int_regs[total_ints - 1]);
                    }
                    break;
                case resolved_type::FLOAT_TYPE:
                    if (++total_floats > float_regs.size()) {
                        stack_args.emplace_back(arg_index);
                        this->bytes_on_stack += reg_size;
                    } else {
                        reg_args.emplace_back(arg_index, reg_t::FLOAT, float_regs[total_floats - 1]);
                    }
                    break;
                case resolved_type::ARRAY_TYPE:
                case resolved_type::TUPLE_TYPE:
                    stack_args.emplace_back(arg_index);
                    this->bytes_on_stack += arg_type->size();
                    break;
            }
        }

        for (int index = (int)stack_args.size() - 1; index >= 0; --index) {
            this->push_order.emplace_back(stack_args[stack_args.size() - index - 1]);
        }

        for (int index = (int)reg_args.size() - 1; index >= 0; --index) {
            this->push_order.emplace_back(std::get<0>(reg_args[reg_args.size() - index - 1]));
        }

        for (const std::tuple<unsigned int, reg_t, std::string>& reg_arg : reg_args) {
            switch (std::get<1>(reg_arg)) {
                case INT:
                    this->pop_assem.emplace_back("\tpop " + std::get<2>(reg_arg) + "\n");
                    break;
                case FLOAT:
                    this->pop_assem.emplace_back("\tmovsd " + std::get<2>(reg_arg) + ", [rsp]\n" + "\tadd rsp, 8\n");
                    break;
            }
        }
    }
}  //  namespace call_signature
