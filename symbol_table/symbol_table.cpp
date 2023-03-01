/**
 * @file symbol_table.cpp
 * @package Assignment 6
 * @author Cayden Lund (u1182408)
 * @brief Implements the `symbol_table` class.
 * @details See the header file for documentation.
 *
 */

#include <stdexcept>

#include "symbol_table.hpp"

namespace symbol_table {
    std::shared_ptr<name_info::name_info> symbol_table::operator[](const std::string &name) const {
        if (this->table.count(name) > 0) {
            return this->table.at(name);
        }

        if (this->parent == nullptr)
            throw std::runtime_error("Symbol \"" + name + "\" is not in symbol table");

        return (*this->parent)[name];
    }

    void symbol_table::add_symbol(const std::string &name, const std::shared_ptr<name_info::name_info> &info) {
        if (this->has_symbol(name))
            throw std::runtime_error(
                    "Symbol \"" + name + "\" is already in symbol table");

        this->table[name] = info;
    }

    bool symbol_table::has_symbol(const std::string &name) const {
        if (this->table.count(name) > 0) return true;

        if (this->parent == nullptr) return false;

        return this->parent->has_symbol(name);
    }
}  // namespace symbol_table
