/**
 * @file symbol_table.hpp
 * @package Assignment 7
 * @author Cayden Lund (u1182408)
 * @brief Defines the `symbol_table` class.
 *
 */

#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include "name_info/name_info.hpp"

#include <unordered_map>
#include <vector>

namespace symbol_table {
    /**
     * @brief A symbol table.
     * @details Contains a mapping of string names to `name_info::name_info` pointers.
     *
     */
    class symbol_table {
        /**
         * @brief A pointer to the parent table.
         *
         */
        const symbol_table *parent;

        /**
         * @brief The table of mappings from string names to `name_info::name_info` pointers.
         *
         */
        std::unordered_map<std::string, std::shared_ptr<name_info::name_info>> table;

    public:
        /**
         * @brief Class constructor.
         *
         * @param parent A pointer to the parent symbol table. Defaults to `nullptr`.
         */
        symbol_table(const symbol_table *parent = nullptr) : parent(parent) {}

        /**
         * @brief Subscript operator.
         * @details Throws an exception if the symbol is not in the table (or in the parent tables).
         *
         * @param name The name of the symbol to access.
         * @return A pointer to the `name_info::name_info` object corresponding to the given name.
         */
        std::shared_ptr<name_info::name_info> operator[](const std::string &name) const;

        /**
         * @brief Adds the given symbol to the symbol table.
         *
         * @param name The name of the symbol.
         * @param info The `name_info::name_info` object corresponding to the symbol of the given name.
         */
        void add_symbol(const std::string &name, const std::shared_ptr<name_info::name_info> &info);

        /**
         * @brief Reports whether the given symbol name is in the symbol table.
         * @details Recursively checks parent tables.
         *
         * @param name The name of the symbol to check.
         * @return True when the given symbol name is in the symbol table.
         */
        bool has_symbol(const std::string &name) const;
    };
}  //  namespace symbol_table

#endif
