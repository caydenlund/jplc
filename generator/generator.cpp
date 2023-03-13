/**
 * @file generator.cpp
 * @package Assignment 9
 * @author Cayden Lund (u1182408)
 * @brief Implements the assembly generation functions.
 * @details See the header file for documentation.
 *
 */

#include <sstream>

#include "generator.hpp"

namespace generator {
    //  ==============
    //  ||  Types:  ||
    //  ==============

    const_table::table_entry::table_entry(long int_value)
        : type(INT_ENTRY), int_value(int_value), float_value(), string_value() {}

    const_table::table_entry::table_entry(double float_value)
        : type(FLOAT_ENTRY), int_value(), float_value(float_value), string_value() {}

    const_table::table_entry::table_entry(std::string string_value)
        : type(STRING_ENTRY), int_value(), float_value(), string_value(std::move(string_value)) {}

    bool const_table::table_entry::operator==(const const_table::table_entry& other) const {
        return this->type == other.type && this->int_value == other.int_value && this->float_value == other.float_value
            && this->string_value == other.string_value;
    }

    std::string const_table::table_entry::assem() const {
        switch (this->type) {
            case INT_ENTRY:
                return "dq " + std::to_string(this->int_value);
            case FLOAT_ENTRY:
                return "dq " + std::to_string(this->float_value);
            case STRING_ENTRY:
                return "db `" + this->string_value + "`, 0";
            default:
                throw std::runtime_error("Invalid table entry type");
        }
    }

    size_t const_table::table_entry_hash::operator()(const table_entry& entry) const {
        return std::hash<long>()(entry.int_value) + std::hash<double>()(entry.float_value)
             + std::hash<std::string>()(entry.string_value);
    }

    const_table::const_table() { this->assembly << ".data\n"; }

    std::string const_table::operator[](const const_table::table_entry& entry) {
        if (this->constants.count(entry) > 0) return this->constants[entry];

        const std::string name = "const" + std::to_string(this->constants.size());
        this->assembly << name << ": " << entry.assem() << "\n";

        this->constants[entry] = name;

        return name;
    }

    std::string const_table::assem() const { return this->assembly.str(); }

    //  ================
    //  ||  Methods:  ||
    //  ================

    std::string generate(const std::vector<std::shared_ptr<ast_node::ast_node>>& nodes, bool debug) {
        std::stringstream result;

        result << generate_linking_preface() << generate_data_text_sections(nodes, debug);

        return result.str();
    }

    std::string generate_linking_preface() {
        std::stringstream result;

        result << "global jpl_main\n"
               << "global _jpl_main\n"
               << "extern _fail_assertion\n"
               << "extern _jpl_alloc\n"
               << "extern _get_time\n"
               << "extern _show\n"
               << "extern _print\n"
               << "extern _print_time\n"
               << "extern _read_image\n"
               << "extern _write_image\n"
               << "extern _fmod\n"
               << "extern _sqrt\n"
               << "extern _exp\n"
               << "extern _sin\n"
               << "extern _cos\n"
               << "extern _tan\n"
               << "extern _asin\n"
               << "extern _acos\n"
               << "extern _atan\n"
               << "extern _log\n"
               << "extern _pow\n"
               << "extern _atan2\n"
               << "extern _to_int\n"
               << "extern _to_float\n"
               << "\n";

        return result.str();
    }

    std::string generate_data_text_sections(const std::vector<std::shared_ptr<ast_node::ast_node>>& nodes, bool debug) {
        const_table constants;
        std::stringstream text_section;
        std::stringstream main_section;

        text_section << ".text\n";
        main_section << "jpl_main:\n"
                     << "_jpl_main:\n"
                     << "\tpush rbp\n"
                     << "\tmov rbp, rsp\n"
                     << "\tpush r12\n"
                     << "\tmov r12, rbp\n";

        for (const std::shared_ptr<ast_node::ast_node>& node : nodes) {
            main_section << generate_cmd(std::reinterpret_pointer_cast<ast_node::cmd_node>(node), constants, debug);
        }

        main_section << "\tadd rsp, 0\n"
                     << "\tpop r12\n"
                     << "\tpop rbp\n"
                     << "\tret\n";

        return constants.assem() + "\n" + text_section.str() + main_section.str();
    }

    std::string generate_cmd(const std::shared_ptr<ast_node::cmd_node>& command, const_table& constants, bool debug) {
        switch (command->type) {
            case ast_node::ASSERT_CMD:
            case ast_node::FN_CMD:
            case ast_node::LET_CMD:
            case ast_node::PRINT_CMD:
            case ast_node::READ_CMD:
                throw std::runtime_error("Unhandled command: \"" + command->s_expression() + "\"");
            case ast_node::SHOW_CMD:
                return generate_cmd_show(std::reinterpret_pointer_cast<ast_node::show_cmd_node>(command), constants,
                                         debug);
            case ast_node::TIME_CMD:
            case ast_node::TYPE_CMD:
            case ast_node::WRITE_CMD:
                throw std::runtime_error("Unhandled command: \"" + command->s_expression() + "\"");
            default:
                throw std::runtime_error("Invalid command: \"" + command->s_expression() + "\"");
        }
    }

    std::string generate_cmd_show(const std::shared_ptr<ast_node::show_cmd_node>& command, const_table& constants,
                                  bool debug) {
        std::stringstream result;

        if (debug) result << "\t;  START generate_cmd_show\n";

        const expr_ret_t expr_result = generate_expr(command->expr, constants, debug);
        result << std::get<0>(expr_result);

        result << "\tlea rdi, [rel " << constants[command->expr->r_type->s_expression()] << "] ; "
               << command->expr->r_type->s_expression() << "\n"
               << "\tlea rsi, [rsp]\n"
               << "\tcall _show\n"
               << "\tadd rsp, " << std::get<1>(expr_result) << "\n";

        if (debug) result << "\t;  END generate_cmd_show\n";

        return result.str();
    }

    expr_ret_t generate_expr(const std::shared_ptr<ast_node::expr_node>& expression, const_table& constants,
                             bool debug) {
        switch (expression->type) {
            case ast_node::ARRAY_INDEX_EXPR:
            case ast_node::ARRAY_LITERAL_EXPR:
            case ast_node::ARRAY_LOOP_EXPR:
            case ast_node::BINOP_EXPR:
            case ast_node::CALL_EXPR:
            case ast_node::ELSE_TOK_EXPR:
            case ast_node::FALSE_EXPR:
                throw std::runtime_error("Unhandled expression: \"" + expression->s_expression() + "\"");
            case ast_node::FLOAT_EXPR:
                return generate_expr_float(std::reinterpret_pointer_cast<ast_node::float_expr_node>(expression),
                                             constants, debug);
            case ast_node::IF_EXPR:
                throw std::runtime_error("Unhandled expression: \"" + expression->s_expression() + "\"");
            case ast_node::INTEGER_EXPR:
                return generate_expr_integer(std::reinterpret_pointer_cast<ast_node::integer_expr_node>(expression),
                                             constants, debug);
            case ast_node::OP_EXPR:
            case ast_node::SUM_LOOP_EXPR:
            case ast_node::THEN_TOK_EXPR:
            case ast_node::TRUE_EXPR:
            case ast_node::TUPLE_INDEX_EXPR:
            case ast_node::TUPLE_LITERAL_EXPR:
            case ast_node::UNOP_EXPR:
            case ast_node::VARIABLE_EXPR:
                throw std::runtime_error("Unhandled expression: \"" + expression->s_expression() + "\"");
            default:
                throw std::runtime_error("Invalid expression: \"" + expression->s_expression() + "\"");
        }
    }

    expr_ret_t generate_expr_float(const std::shared_ptr<ast_node::float_expr_node>& expression, const_table& constants,
                                   bool debug) {
        std::stringstream result;
        constexpr unsigned int float_size = 8;

        if (debug) result << "\t;  START generate_expr_float\n";

        result << "\tmov rax, [rel " << constants[expression->value] << "] ; " << expression->value << "\n"
               << "\tpush rax\n";

        if (debug) result << "\t;  END generate_expr_float (stack size: " << float_size << ")\n";

        return {result.str(), float_size};
    }

    expr_ret_t generate_expr_integer(const std::shared_ptr<ast_node::integer_expr_node>& expression,
                                     const_table& constants, bool debug) {
        std::stringstream result;
        constexpr unsigned int int_size = 8;

        if (debug) result << "\t;  START generate_expr_integer\n";

        result << "\tmov rax, [rel " << constants[expression->value] << "] ; " << expression->value << "\n"
               << "\tpush rax\n";

        if (debug) result << "\t;  END generate_expr_integer (stack size: " << int_size << ")\n";

        return {result.str(), int_size};
    }
}  //  namespace generator
