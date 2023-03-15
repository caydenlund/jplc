/**
 * @file generator.hpp
 * @package Assignment 9
 * @author Cayden Lund (u1182408)
 * @brief Defines the assembly generation functions.
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
#include "symbol_table/symbol_table.hpp"

namespace generator {
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

    /**
     * @brief Records information about the stack.
     *
     */
    class stack_info {
    private:
        class int_stack {
        private:
            struct node {
                std::unique_ptr<node> last;
                const unsigned int value;
            };

            /**
             * @brief A pointer to the top of the stack.
             *
             */
            std::unique_ptr<node> last;

            /**
             * @brief The total of the elements in the stack.
             *
             */
            unsigned int total;

        public:
            /**
             * @brief Class constructor.
             *
             */
            int_stack();

            /**
             * @brief Reports the total of the elements in the stack.
             *
             * @return The total of the elements in the stack.
             */
            [[nodiscard]] unsigned int get_total() const;

            /**
             * @brief Pops the top element from the stack.
             *
             * @return The value of the top element on the stack.
             */
            unsigned int pop();

            /**
             * @brief Pushes a value onto the stack.
             *
             * @param value The value to push onto the stack.
             */
            void push(unsigned int value);
        };

        /**
         * @brief The default number of bytes to push onto the stack.
         *
         */
        static constexpr unsigned int default_num_bytes = 8;

        /**
         * @brief The stack must be aligned to this many bytes to call a function.
         *
         */
        static constexpr unsigned int stack_alignment = 16;

        /**
         * @brief The stack of integers.
         *
         */
        int_stack stack;

    public:
        /**
         * @brief Reports whether the stack needs to be 16-byte aligned.
         *
         * @return True if the stack needs to be 16-byte aligned.
         */
        [[nodiscard]] bool needs_alignment() const;

        /**
         * @brief Pops the top value from the stack.
         *
         * @return The top value on the stack.
         */
        unsigned int pop();

        /**
         * @brief Pushes the given number of bytes onto the stack.
         *
         * @param num_bytes The number of bytes to push onto the stack. Defaults to 8.
         */
        void push(unsigned int num_bytes = default_num_bytes);
    };

    //  ================
    //  ||  Methods:  ||
    //  ================

    /**
     * @brief Generates the assembly output for the given set of AST nodes.
     *
     * @param nodes The set of AST nodes for which to generate the assembly.
     * @param debug Whether to generate extra comments for debugging.
     * @return The string assembly output.
     */
    std::string generate(const std::vector<std::shared_ptr<ast_node::ast_node>>& nodes, bool debug);

    /**
     * @brief Generates the header linking preface for a JPL program.
     *
     * @return The string assembly header linking preface.
     */
    std::string generate_linking_preface();

    /**
     * @brief Generates the `.data` and `.text` section for a JPL program.
     *
     * @param nodes The set of AST nodes for which to generate the assembly.
     * @param debug Whether to generate extra comments for debugging.
     * @return The string assembly `.data` and `.text` section.
     */
    std::string generate_data_text_sections(const std::vector<std::shared_ptr<ast_node::ast_node>>& nodes, bool debug);

    //  Commands:
    //  ---------

    /**
     * @brief Generates assembly for a single command AST node.
     *
     * @param command The command AST node.
     * @param constants The set of global constants.
     * @param stack Information about the stack.
     * @param debug Whether to generate extra comments for debugging.
     * @return The string assembly for the given command.
     */
    std::string generate_cmd(const std::shared_ptr<ast_node::cmd_node>& command, const_table& constants,
                             stack_info& stack, bool debug);

    /**
     * @brief Generates assembly for a single `show` command AST node.
     *
     * @param command The show command AST node.
     * @param constants The set of global constants.
     * @param stack Information about the stack.
     * @param debug Whether to generate extra comments for debugging.
     * @return The string assembly for the given show command.
     */
    std::string generate_cmd_show(const std::shared_ptr<ast_node::show_cmd_node>& command, const_table& constants,
                                  stack_info& stack, bool debug);

    //  Expressions:
    //  ------------

    /**
     * @brief Generates assembly for a single expression AST node.
     *
     * @param expression The expression AST node.
     * @param constants The set of global constants.
     * @param stack Information about the stack.
     * @param debug Whether to generate extra comments for debugging.
     * @return The string assembly for the given expression.
     */
    std::string generate_expr(const std::shared_ptr<ast_node::expr_node>& expression, const_table& constants,
                              stack_info& stack, bool debug);

    /**
     * @brief Generates assembly for a single binary operation expression AST node.
     *
     * @param expression The binary operation expression AST node.
     * @param constants The set of global constants.
     * @param stack Information about the stack.
     * @param debug Whether to generate extra comments for debugging.
     * @return The string assembly for the given expression.
     */
    std::string generate_expr_binop(const std::shared_ptr<ast_node::binop_expr_node>& expression,
                                    const_table& constants, stack_info& stack, bool debug);

    /**
     * @brief Generates assembly for a single `false` expression AST node.
     *
     * @param expression The false expression AST node.
     * @param constants The set of global constants.
     * @param stack Information about the stack.
     * @param debug Whether to generate extra comments for debugging.
     * @return The string assembly for the given expression.
     */
    std::string generate_expr_false(const std::shared_ptr<ast_node::false_expr_node>& expression,
                                    const_table& constants, stack_info& stack, bool debug);

    /**
     * @brief Generates assembly for a single floating-point expression AST node.
     *
     * @param expression The float expression AST node.
     * @param constants The set of global constants.
     * @param stack Information about the stack.
     * @param debug Whether to generate extra comments for debugging.
     * @return The string assembly for the given expression.
     */
    std::string generate_expr_float(const std::shared_ptr<ast_node::float_expr_node>& expression,
                                    const_table& constants, stack_info& stack, bool debug);

    /**
     * @brief Generates assembly for a single integer expression AST node.
     *
     * @param expression The integer expression AST node.
     * @param constants The set of global constants.
     * @param stack Information about the stack.
     * @param debug Whether to generate extra comments for debugging.
     * @return The string assembly for the given expression.
     */
    std::string generate_expr_integer(const std::shared_ptr<ast_node::integer_expr_node>& expression,
                                      const_table& constants, stack_info& stack, bool debug);

    /**
     * @brief Generates assembly for a single `true` expression AST node.
     *
     * @param expression The true expression AST node.
     * @param constants The set of global constants.
     * @param stack Information about the stack.
     * @param debug Whether to generate extra comments for debugging.
     * @return The string assembly for the given expression.
     */
    std::string generate_expr_true(const std::shared_ptr<ast_node::true_expr_node>& expression, const_table& constants,
                                   stack_info& stack, bool debug);

    /**
     * @brief Generates assembly for a single tuple literal expression AST node.
     *
     * @param expression The tuple literal expression AST node.
     * @param constants The set of global constants.
     * @param stack Information about the stack.
     * @param debug Whether to generate extra comments for debugging.
     * @return The string assembly for the given expression.
     */
    std::string generate_expr_tuple_literal(const std::shared_ptr<ast_node::tuple_literal_expr_node>& expression,
                                            const_table& constants, stack_info& stack, bool debug);

    /**
     * @brief Generates assembly for a single unary operation expression AST node.
     *
     * @param expression The unary operation expression AST node.
     * @param constants The set of global constants.
     * @param stack Information about the stack.
     * @param debug Whether to generate extra comments for debugging.
     * @return The string assembly for the given expression.
     */
    std::string generate_expr_unop(const std::shared_ptr<ast_node::unop_expr_node>& expression, const_table& constants,
                                   stack_info& stack, bool debug);
}  //  namespace generator

#endif
