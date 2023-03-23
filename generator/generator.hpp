/**
 * @file generator.hpp
 * @package Assignments 9-10
 * @author Cayden Lund (u1182408)
 * @brief Defines the assembly `generator` class, and associated subtypes.
 *
 */

#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include "ast_node/ast_node.hpp"
#include "stack_info/stack_info.hpp"
#include "symbol_table/symbol_table.hpp"

namespace generator {
    /**
     * @brief The assembly generator.
     *
     */
    class generator {
    private:
        //  ==============
        //  ||  Types:  ||
        //  ==============

        /**
         * @brief Defines a table for constants.
         *
         */
        class const_table {
        private:
            /**
             * @brief The type of an entry in the constants table.
             *
             */
            struct table_entry {
                /**
                 * @brief The types of entries.
                 *
                 */
                enum entry_type { INT_ENTRY, FLOAT_ENTRY, STRING_ENTRY };

                /**
                 * @brief The type of this entry.
                 *
                 */
                const entry_type type;

                /**
                 * @brief The integer value of this entry, if this entry is an integer.
                 *
                 */
                const long int_value;

                /**
                 * @brief The floating-point value of this entry, if this entry is a floating-point value.
                 *
                 */
                const double float_value;

                /**
                 * @brief The string value of this entry, if this entry is a string.
                 *
                 */
                const std::string string_value;

                /**
                 * @brief Implicit constructor for integer entries.
                 *
                 * @param int_value The integer value of this entry.
                 */
                table_entry(long int_value);

                /**
                 * @brief Implicit constructor for floating-point entries.
                 *
                 * @param float_value The floating-point value of this entry.
                 */
                table_entry(double float_value);

                /**
                 * @brief Implicit constructor for string entries.
                 *
                 * @param string_value The string value of this entry.
                 */
                table_entry(std::string string_value);

                /**
                 * @brief Equality operator.
                 *
                 * @param other The other entry to which to compare.
                 * @return True when the two entries are equal.
                 */
                bool operator==(const table_entry& other) const;

                /**
                 * @brief Returns the assembly code for this constant.
                 *
                 * @return The string assembly code for this constant.
                 */
                [[nodiscard]] std::string assem() const;
            };

            /**
             * @brief Hash function for the `table_entry` class.
             *
             */
            struct table_entry_hash {
                /**
                 * @brief Call operator.
                 *
                 * @param entry The `table_entry` to hash.
                 * @return The hash value of the given `table_entry` instance.
                 */
                size_t operator()(const table_entry& entry) const;
            };

            /**
             * @brief The assembly code of this constants table.
             *
             */
            std::stringstream assembly;

            /**
             * @brief A mapping of constant values to the corresponding constant name.
             *
             */
            std::unordered_map<table_entry, std::string, table_entry_hash> constants;

            /**
             * @brief The number of the next jump label.
             *
             */
            unsigned int next_jump_number;

        public:
            /**
             * @brief Class constructor.
             *
             */
            const_table();

            /**
             * @brief Subscript operator.
             * @details Returns the string name of the desired constant.
             *     Inserts into the constants table if the constant does not yet exist.
             *
             * @param entry The `table_entry` to be located in the constants table.
             * @return The string name of the desired constant.
             */
            std::string operator[](const table_entry& entry);

            /**
             * @brief Returns the string assembly code of the constants table.
             *
             * @return The string assembly code of the constants table.
             */
            [[nodiscard]] std::string assem() const;

            /**
             * @brief Returns a unique label for a new jump instruction.
             *
             * @return A unique label for a new jump instruction.
             */
            std::string next_jump();
        };

        //  ===========================
        //  ||  Instance Variables:  ||
        //  ===========================

        /**
         * @brief A set of assembly instructions for the program's commands.
         *
         */
        std::stringstream main_assembly;

        /**
         * @brief The constants defined in the `.data` section of the assembly.
         *
         */
        const_table constants;

        /**
         * @brief Whether to include extra output when debugging.
         *
         */
        bool debug;

        /**
         * @brief The functions defined in the program.
         *
         */
        std::stringstream function_assemblies;

        /**
         * @brief The header linking preface.
         *
         */
        std::stringstream linking_preface_assembly;

        /**
         * @brief The set of command nodes that make up the program.
         *
         */
        const std::vector<std::shared_ptr<ast_node::ast_node>> nodes;

        /**
         * @brief Information about the stack.
         *
         */
        stack_info::stack_info stack;

        //  ================
        //  ||  Methods:  ||
        //  ================

        /**
         * @brief Generates the header linking preface for a JPL program.
         *
         */
        void generate_linking_preface();

        /**
         * @brief Iterates over the commands of the JPL program, updating the `.data` and `.text` sections accordingly.
         *
         */
        void generate_commands();

        //  Commands:
        //  ---------

        /**
         * @brief Generates assembly for a single command AST node.
         *
         * @param command The command AST node.
         */
        void generate_cmd(const std::shared_ptr<ast_node::cmd_node>& command);

        /**
         * @brief Generates assembly for a single `let` command AST node.
         *
         * @param command The `let` command AST node.
         */
        void generate_cmd_let(const std::shared_ptr<ast_node::let_cmd_node>& command);

        /**
         * @brief Generates assembly for a single `show` command AST node.
         *
         * @param command The `show` command AST node.
         */
        void generate_cmd_show(const std::shared_ptr<ast_node::show_cmd_node>& command);

        //  Expressions:
        //  ------------

        /**
         * @brief Generates assembly for a single expression AST node.
         *
         * @param expression The expression AST node.
         * @return The string assembly result for the given expression.
         */
        std::string generate_expr(const std::shared_ptr<ast_node::expr_node>& expression);

        /**
         * @brief Generates assembly for a single array literal expression AST node.
         *
         * @param expression The array literal expression AST node.
         * @return The string assembly for the given expression.
         */
        std::string generate_expr_array_literal(const std::shared_ptr<ast_node::array_literal_expr_node>& expression);

        /**
         * @brief Generates assembly for a single binary operation expression AST node.
         *
         * @param expression The binary operation expression AST node.
         * @return The string assembly for the given expression.
         */
        std::string generate_expr_binop(const std::shared_ptr<ast_node::binop_expr_node>& expression);

        /**
         * @brief Generates assembly for a single call expression AST node.
         *
         * @param expression The call expression AST node.
         * @return The string assembly for the given expression.
         */
        std::string generate_expr_call(const std::shared_ptr<ast_node::call_expr_node>& expression);

        /**
         * @brief Generates assembly for a single `false` expression AST node.
         *
         * @param expression The false expression AST node.
         * @return The string assembly for the given expression.
         */
        std::string generate_expr_false(const std::shared_ptr<ast_node::false_expr_node>& expression);

        /**
         * @brief Generates assembly for a single floating-point expression AST node.
         *
         * @param expression The float expression AST node.
         * @return The string assembly for the given expression.
         */
        std::string generate_expr_float(const std::shared_ptr<ast_node::float_expr_node>& expression);

        /**
         * @brief Generates assembly for a single integer expression AST node.
         *
         * @param expression The integer expression AST node.
         * @return The string assembly for the given expression.
         */
        std::string generate_expr_integer(const std::shared_ptr<ast_node::integer_expr_node>& expression);

        /**
         * @brief Generates assembly for a single `true` expression AST node.
         *
         * @param expression The true expression AST node.
         * @return The string assembly for the given expression.
         */
        std::string generate_expr_true(const std::shared_ptr<ast_node::true_expr_node>& expression);

        /**
         * @brief Generates assembly for a single tuple index expression AST node.
         *
         * @param expression The tuple literal expression AST node.
         * @return The string assembly for the given expression.
         */
        std::string generate_expr_tuple_index(const std::shared_ptr<ast_node::tuple_index_expr_node>& expression);

        /**
         * @brief Generates assembly for a single tuple literal expression AST node.
         *
         * @param expression The tuple literal expression AST node.
         * @return The string assembly for the given expression.
         */
        std::string generate_expr_tuple_literal(const std::shared_ptr<ast_node::tuple_literal_expr_node>& expression);

        /**
         * @brief Generates assembly for a single unary operation expression AST node.
         *
         * @param expression The unary operation expression AST node.
         * @return The string assembly for the given expression.
         */
        std::string generate_expr_unop(const std::shared_ptr<ast_node::unop_expr_node>& expression);

        /**
         * @brief Generates assembly for a single variable expression AST node.
         *
         * @param expression The variable expression AST node.
         * @return The string assembly for the given expression.
         */
        std::string generate_expr_variable(const std::shared_ptr<ast_node::variable_expr_node>& expression);

    public:
        /**
         * @brief Class constructor.
         *
         * @param nodes The set of AST nodes for which to generate the assembly.
         * @param debug Whether to generate extra comments for debugging.
         */
        generator(const std::vector<std::shared_ptr<ast_node::ast_node>>& nodes, bool debug);

        /**
         * @brief Returns the assembly generated by this instance.
         *
         * @return The (string) assembly generated by this instance.
         */
        std::string assem() const;
    };

    /**
     * @brief Generates the assembly output for the given set of AST nodes.
     *
     * @param nodes The set of AST nodes for which to generate the assembly.
     * @param debug Whether to generate extra comments for debugging.
     * @return The string assembly output.
     */
    std::string generate(const std::vector<std::shared_ptr<ast_node::ast_node>>& nodes, bool debug);
}  //  namespace generator

#endif
