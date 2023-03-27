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
#include "call_signature/call_signature.hpp"
#include "stack_info/stack_info.hpp"
#include "symbol_table/symbol_table.hpp"

namespace generator {
    /**
     * @brief The assembly generator.
     *
     */
    class generator {
    protected:
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
         * @brief Tracks variables as an offset from register RBP.
         *
         */
        class variable_table {
        private:
            /**
             * @brief A pointer to the parent variable table, if present.
             *
             */
            const std::shared_ptr<variable_table> parent;

            /**
             * @brief A mapping from a variable name to an offset from register RBP.
             *
             */
            std::unordered_map<std::string, long> variables;

        public:
            /**
             * @brief Class constructor.
             *
             * @param parent The parent instance.
             */
            variable_table(const std::shared_ptr<variable_table>& parent = nullptr);

            /**
             * @brief Determines the address to use to access a local or global variable.
             *
             * @param variable The variable to access.
             * @param from_child True when a child instance is calling this instance,
             *     for when determining whether to use register RBP or register R12. Defaults to false.
             * @return The address to use to access the given variable, given in a pair of {register, offset}.
             */
            [[nodiscard]] std::tuple<std::string, long> get_variable_address(const std::string& variable,
                                                                             bool from_child = false) const;

            /**
             * @brief Sets the address for the given variable.
             *
             * @param variable The variable to set.
             * @param offset The offset from the base pointer where the value is.
             */
            void set_variable_address(const std::string& variable, long offset);
        };

        //  ===========================
        //  ||  Instance Variables:  ||
        //  ===========================

        /**
         * @brief The constants defined in the `.data` section of the assembly.
         * @details Each instance will point to the same main constants table.
         *
         */
        const std::shared_ptr<const_table> constants;

        /**
         * @brief Whether to include extra output when debugging.
         *
         */
        bool debug;

        /**
         * @brief The call signatures of the functions defined in the program.
         *
         */
        const std::shared_ptr<std::unordered_map<std::string, call_signature::call_signature>> function_signatures;

        /**
         * @brief The main assembly sequence for this instance.
         *
         */
        std::stringstream main_assembly;

        /**
         * @brief Information about the stack.
         *
         */
        stack_info::stack_info stack;

        /**
         * @brief The global symbol table.
         *
         */
        const std::shared_ptr<symbol_table::symbol_table> global_symbol_table;

        /**
         * @brief The set of all variables.
         *
         */
        variable_table variables;

        //  ================
        //  ||  Methods:  ||
        //  ================

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
         * @brief Generates assembly for a single array index expression AST node.
         *
         * @param expression The array index expression AST node.
         * @return The string assembly for the given expression.
         */
        std::string generate_expr_array_index(const std::shared_ptr<ast_node::array_index_expr_node>& expression);

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
         * @param global_symbol_table A pointer to the global symbol table.
         * @param constants A pointer to the global constants table.
         * @param function_signatures A pointer to the set of function signatures.
         * @param parent_variable_table A pointer to the parent generator's variable table.
         *     Use `nullptr` if there is no parent.
         * @param debug Whether to generate extra debugging output.
         */
        generator(const std::shared_ptr<symbol_table::symbol_table>& global_symbol_table,
                  const std::shared_ptr<const_table>& constants,
                  const std::shared_ptr<std::unordered_map<std::string, call_signature::call_signature>>&
                          function_signatures,
                  const std::shared_ptr<variable_table>& parent_variable_table, bool debug = false);

        /**
         * @brief Class destructor.
         *
         */
        virtual ~generator() = default;

        /**
         * @brief Returns the assembly generated by this instance.
         *
         * @return The (string) assembly generated by this instance.
         */
        virtual std::string assem() const = 0;

        /**
         * @brief Binds the value on the top of the stack to the given argument.
         *
         * @param argument The argument to bind.
         * @param r_type The type of the value on the top of the stack.
         */
        void bind_argument(const std::shared_ptr<ast_node::argument_node>& argument,
                           const std::shared_ptr<resolved_type::resolved_type>& r_type);

        /**
         * @brief Binds the value on the top of the stack to the given lvalue.
         *
         * @param lvalue The lvalue to bind.
         * @param r_type The type of the value on the top of the stack.
         */
        void bind_lvalue(const std::shared_ptr<ast_node::lvalue_node>& lvalue,
                         const std::shared_ptr<resolved_type::resolved_type>& r_type);
    };

    /**
     * @brief A specialization of the `generator` class for generating assembly a function.
     *
     */
    class fn_generator : public generator {
    private:
        /**
         * @brief The initial offset from RBP for the next argument from the stack.
         *
         */
        static constexpr long initial_rbp_offset = -16;

        /**
         * @brief The offset from RBP for the next argument from the stack.
         *
         */
        long rbp_offset;

        //  Statements:
        //  -----------

        /**
         * @brief Generates assembly for a single statement AST node.
         *
         * @param statement The statement AST node.
         * @return The string assembly for the given statement.
         */
        void generate_stmt(const std::shared_ptr<ast_node::stmt_node>& statement);

        /**
         * @brief Generates assembly for a single `assert` statement AST node.
         *
         * @param statement The `assert` statement AST node.
         * @return The string assembly for the given statement.
         */
        void generate_stmt_assert(const std::shared_ptr<ast_node::assert_stmt_node>& statement);

        /**
         * @brief Generates assembly for a single `let` statement AST node.
         *
         * @param statement The `let` statement AST node.
         * @return The string assembly for the given statement.
         */
        void generate_stmt_let(const std::shared_ptr<ast_node::let_stmt_node>& statement);

        /**
         * @brief Generates assembly for a single `return` statement AST node.
         *
         * @param statement The `return` statement AST node.
         * @return The string assembly for the given statement.
         */
        void generate_stmt_return(const std::shared_ptr<ast_node::return_stmt_node>& statement);

        //  Misc. methods:
        //  --------------

        /**
         * @brief Handles a single register binding as a function argument.
         *
         * @param binding The binding to handle.
         * @param r_type The resolved type of the argument.
         * @param reg The name of the register.
         */
        void handle_reg_binding(const std::shared_ptr<ast_node::binding_node>& binding,
                                const std::shared_ptr<resolved_type::resolved_type>& r_type, const std::string& reg);

        /**
         * @brief Handles a single stack binding as a function argument.
         *
         * @param binding The binding to handle.
         * @param r_type The resolved type of the argument.
         */
        void handle_stack_binding(const std::shared_ptr<ast_node::binding_node>& binding,
                                  const std::shared_ptr<resolved_type::resolved_type>& r_type);

    public:
        /**
         * @brief Class constructor.
         *
         * @param global_symbol_table A pointer to the global symbol table.
         * @param function The `fn` AST node.
         * @param constants A pointer to the global constants table.
         * @param function_signatures A pointer to the set of function signatures.
         * @param parent_variable_table A pointer to the parent generator's variable table.
         * @param debug Whether to generate extra debugging output.
         */
        fn_generator(const std::shared_ptr<symbol_table::symbol_table>& global_symbol_table,
                     const std::shared_ptr<ast_node::fn_cmd_node>& function,
                     const std::shared_ptr<const_table>& constants,
                     const std::shared_ptr<std::unordered_map<std::string, call_signature::call_signature>>&
                             function_signatures,
                     const std::shared_ptr<variable_table>& parent_variable_table, bool debug);

        /**
         * @brief Returns the assembly generated by this instance.
         *
         * @return The (string) assembly generated by this instance.
         */
        [[nodiscard]] std::string assem() const override;
    };

    /**
     * @brief A specialization of the `generator` class for generating the overall assembly for the program.
     *
     */
    class main_generator : public generator {
    private:
        /**
         * @brief The header linking preface.
         *
         */
        std::stringstream linking_preface_assembly;

        /**
         * @brief A set of assembly instructions for the program's commands.
         *
         */
        std::stringstream main_assembly;

        /**
         * @brief The set of command nodes that make up the program.
         *
         */
        const std::vector<std::shared_ptr<ast_node::ast_node>> nodes;

        /**
         * @brief The functions defined in the program.
         *
         */
        std::vector<std::string> function_assemblies;

        //  Commands:
        //  ---------

        /**
         * @brief Generates assembly for a single command AST node.
         *
         * @param command The command AST node.
         */
        void generate_cmd(const std::shared_ptr<ast_node::cmd_node>& command);

        /**
         * @brief Generates assembly for a single `fn` command AST node.
         *
         * @param command The `fn` command AST node.
         */
        void generate_cmd_fn(const std::shared_ptr<ast_node::fn_cmd_node>& command);

        /**
         * @brief Generates assembly for a single `let` command AST node.
         *
         * @param command The `let` command AST node.
         */
        void generate_cmd_let(const std::shared_ptr<ast_node::let_cmd_node>& command);

        /**
         * @brief Generates assembly for a single `read` command AST node.
         *
         * @param command The `read` command AST node.
         */
        void generate_cmd_read(const std::shared_ptr<ast_node::read_cmd_node>& command);

        /**
         * @brief Generates assembly for a single `show` command AST node.
         *
         * @param command The `show` command AST node.
         */
        void generate_cmd_show(const std::shared_ptr<ast_node::show_cmd_node>& command);

        //  Misc. methods:
        //  --------------

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

    public:
        /**
         * @brief Class constructor.
         *
         * @param global_symbol_table A pointer to the global symbol table.
         * @param nodes The set of command nodes that compose the program.
         * @param debug Whether to generate extra debug output.
         */
        main_generator(const std::shared_ptr<symbol_table::symbol_table>& global_symbol_table,
                       const std::vector<std::shared_ptr<ast_node::ast_node>>& nodes, bool debug);

        /**
         * @brief Returns the assembly generated by this instance.
         *
         * @return The (string) assembly generated by this instance.
         */
        [[nodiscard]] std::string assem() const override;
    };

    /**
     * @brief Generates the assembly output for the given set of AST nodes.
     *
     * @param global_symbol_table A pointer to the global symbol table.
     * @param nodes The set of AST nodes for which to generate the assembly.
     * @param debug Whether to generate extra comments for debugging.
     * @return The string assembly output.
     */
    std::string generate(const std::shared_ptr<symbol_table::symbol_table>& global_symbol_table,
                         const std::vector<std::shared_ptr<ast_node::ast_node>>& nodes, bool debug);
}  //  namespace generator

#endif
