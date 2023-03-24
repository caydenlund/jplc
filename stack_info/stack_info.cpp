/**
 * @file stack_info.cpp
 * @package Assignments 9-10
 * @author Cayden Lund (u1182408)
 * @brief Defines the `stack_info` class and associated subtypes.
 * @details See the header file for documentation.
 *
 */

#include "stack_info.hpp"

stack_info::stack_info::int_stack::int_stack() : last(nullptr), total(0) {}

unsigned int stack_info::stack_info::int_stack::get_total() const { return this->total; }

unsigned int stack_info::stack_info::int_stack::pop() {
    if (this->last == nullptr) return 0;

    const std::unique_ptr<stack_info::int_stack::node> last_node = std::move(this->last);
    if (last_node->last != nullptr) this->last = std::move(last_node->last);

    this->total -= last_node->value;

    return last_node->value;
}

void stack_info::stack_info::int_stack::push(unsigned int value) {
    std::unique_ptr<stack_info::int_stack::node> new_node = std::make_unique<node>(node {nullptr, value});
    if (this->last != nullptr) new_node->last = std::move(this->last);
    this->last = std::move(new_node);
    this->total += value;
}

bool stack_info::stack_info::needs_alignment() const { return this->stack.get_total() % stack_alignment > 0; }

unsigned int stack_info::stack_info::pop() { return this->stack.pop(); }

void stack_info::stack_info::push(unsigned int num_bytes) { this->stack.push(num_bytes); }

unsigned int stack_info::stack_info::size() const { return this->stack.get_total(); }
