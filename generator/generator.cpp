/**
 * @file generator.cpp
 * @package Assignment 9
 * @author Cayden Lund (u1182408)
 * @brief Implements the assembly generation functions and classes.
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
        if (this->type == INT_ENTRY) { return "dq " + std::to_string(this->int_value); }

        if (this->type == FLOAT_ENTRY) {
            const std::string format = "dq %g";
            constexpr unsigned int buf_size = 100;
            const std::unique_ptr<char[]> buf(new char[buf_size]);
            const unsigned int result_size = std::snprintf(buf.get(), buf_size, format.c_str(), this->float_value);
            std::string result(buf.get(), buf.get() + result_size);

            if (result.find('.') == std::string::npos && result.find('e') == std::string::npos) result += ".0";

            return result;
        }

        return "db `" + this->string_value + "`, 0";
    }

    size_t const_table::table_entry_hash::operator()(const table_entry& entry) const {
        return std::hash<long>()(entry.int_value) + std::hash<double>()(entry.float_value)
             + std::hash<std::string>()(entry.string_value);
    }

    const_table::const_table() : next_jump_number(1) { this->assembly << "section .data\n"; }

    std::string const_table::operator[](const const_table::table_entry& entry) {
        if (this->constants.count(entry) > 0) return this->constants[entry];

        const std::string name = "const" + std::to_string(this->constants.size());
        this->assembly << name << ": " << entry.assem() << "\n";

        this->constants[entry] = name;

        return name;
    }

    std::string const_table::assem() const { return this->assembly.str(); }

    std::string const_table::next_jump() { return ".jump" + std::to_string(this->next_jump_number++); }

    stack_info::int_stack::int_stack() : last(nullptr), total(0) {}

    unsigned int stack_info::int_stack::get_total() const { return this->total; }

    unsigned int stack_info::int_stack::pop() {
        if (this->last == nullptr) return 0;

        const std::unique_ptr<stack_info::int_stack::node> last_node = std::move(this->last);
        if (last_node->last != nullptr) this->last = std::move(last_node->last);

        this->total -= last_node->value;

        return last_node->value;
    }

    void stack_info::int_stack::push(unsigned int value) {
        std::unique_ptr<stack_info::int_stack::node> new_node = std::make_unique<node>(node {nullptr, value});
        if (this->last != nullptr) new_node->last = std::move(this->last);
        this->last = std::move(new_node);
        this->total += value;
    }

    bool stack_info::needs_alignment() const { return this->stack.get_total() % stack_alignment > 0; }

    unsigned int stack_info::pop() { return this->stack.pop(); }

    void stack_info::push(unsigned int num_bytes) { this->stack.push(num_bytes); }

    //  ================
    //  ||  Methods:  ||
    //  ================

    std::string generate(const std::vector<std::shared_ptr<ast_node::ast_node>>& nodes, bool debug) {
        std::stringstream assembly;

        assembly << generate_linking_preface() << generate_data_text_sections(nodes, debug);

        return assembly.str();
    }

    std::string generate_linking_preface() {
        std::stringstream assembly;

        assembly << "global jpl_main\n"
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

        return assembly.str();
    }

    std::string generate_data_text_sections(const std::vector<std::shared_ptr<ast_node::ast_node>>& nodes, bool debug) {
        const_table constants;
        stack_info stack;
        std::stringstream text_section;
        std::stringstream main_section;

        text_section << "section .text\n";
        main_section << "jpl_main:\n"
                     << "_jpl_main:\n"
                     << "\tpush rbp\n"
                     << "\tmov rbp, rsp\n"
                     << "\tpush r12\n";
        stack.push();
        main_section << "\tmov r12, rbp\n";

        for (const std::shared_ptr<ast_node::ast_node>& node : nodes) {
            main_section << generate_cmd(std::reinterpret_pointer_cast<ast_node::cmd_node>(node), constants, stack,
                                         debug);
        }

        main_section << "\tpop r12\n";
        stack.pop();
        main_section << "\tpop rbp\n";
        stack.pop();
        main_section << "\tret\n";

        return constants.assem() + "\n" + text_section.str() + main_section.str();
    }

    //  Commands:
    //  ---------

    std::string generate_cmd(const std::shared_ptr<ast_node::cmd_node>& command, const_table& constants,
                             stack_info& stack, bool debug) {
        switch (command->type) {
            case ast_node::ASSERT_CMD:
            case ast_node::FN_CMD:
            case ast_node::LET_CMD:
            case ast_node::PRINT_CMD:
            case ast_node::READ_CMD:
                throw std::runtime_error("Unhandled command: \"" + command->s_expression() + "\"");
            case ast_node::SHOW_CMD:
                return generate_cmd_show(std::reinterpret_pointer_cast<ast_node::show_cmd_node>(command), constants,
                                         stack, debug);
            case ast_node::TIME_CMD:
            case ast_node::TYPE_CMD:
            case ast_node::WRITE_CMD:
                throw std::runtime_error("Unhandled command: \"" + command->s_expression() + "\"");
            default:
                throw std::runtime_error("Invalid command: \"" + command->s_expression() + "\"");
        }
    }

    std::string generate_cmd_show(const std::shared_ptr<ast_node::show_cmd_node>& command, const_table& constants,
                                  stack_info& stack, bool debug) {
        std::stringstream assembly;

        if (debug) assembly << "\t;  START generate_cmd_show\n";

        //  We need to know the size of the expression on the stack in order to align the stack,
        //  so we push it onto the stack and then pop it again before generating the expression.
        stack.push(command->expr->r_type->size());
        const bool needs_alignment = stack.needs_alignment();
        stack.pop();
        if (needs_alignment) {
            assembly << "\tsub rsp, 8 ; Align stack\n";
            stack.push();
        }

        assembly << generate_expr(command->expr, constants, stack, debug) << "\tlea rdi, [rel "
                 << constants[command->expr->r_type->s_expression()] << "] ; " << command->expr->r_type->s_expression()
                 << "\n"
                 << "\tlea rsi, [rsp]\n"
                 << "\tcall _show\n"
                 << "\tadd rsp, " << stack.pop() << "\n";

        if (needs_alignment) {
            assembly << "\tadd rsp, 8 ; Remove alignment\n";
            stack.pop();
        }

        if (debug) assembly << "\t;  END generate_cmd_show\n";

        return assembly.str();
    }

    //  Expressions:
    //  ------------

    std::string generate_expr(const std::shared_ptr<ast_node::expr_node>& expression, const_table& constants,
                              stack_info& stack, bool debug) {
        switch (expression->type) {
            case ast_node::ARRAY_INDEX_EXPR:
                throw std::runtime_error("Unhandled expression: \"" + expression->s_expression() + "\"");
            case ast_node::ARRAY_LITERAL_EXPR:
                return generate_expr_array_literal(
                        std::reinterpret_pointer_cast<ast_node::array_literal_expr_node>(expression), constants, stack,
                        debug);
            case ast_node::ARRAY_LOOP_EXPR:
                throw std::runtime_error("Unhandled expression: \"" + expression->s_expression() + "\"");
            case ast_node::BINOP_EXPR:
                return generate_expr_binop(std::reinterpret_pointer_cast<ast_node::binop_expr_node>(expression),
                                           constants, stack, debug);
            case ast_node::CALL_EXPR:
                throw std::runtime_error("Unhandled expression: \"" + expression->s_expression() + "\"");
            case ast_node::FALSE_EXPR:
                return generate_expr_false(std::reinterpret_pointer_cast<ast_node::false_expr_node>(expression),
                                           constants, stack, debug);
            case ast_node::FLOAT_EXPR:
                return generate_expr_float(std::reinterpret_pointer_cast<ast_node::float_expr_node>(expression),
                                           constants, stack, debug);
            case ast_node::IF_EXPR:
                throw std::runtime_error("Unhandled expression: \"" + expression->s_expression() + "\"");
            case ast_node::INTEGER_EXPR:
                return generate_expr_integer(std::reinterpret_pointer_cast<ast_node::integer_expr_node>(expression),
                                             constants, stack, debug);
            case ast_node::OP_EXPR:
            case ast_node::SUM_LOOP_EXPR:
                throw std::runtime_error("Unhandled expression: \"" + expression->s_expression() + "\"");
            case ast_node::TRUE_EXPR:
                return generate_expr_true(std::reinterpret_pointer_cast<ast_node::true_expr_node>(expression),
                                          constants, stack, debug);
            case ast_node::TUPLE_INDEX_EXPR:
                //  TODO (HW9): Implement.
                throw std::runtime_error("Unhandled expression: \"" + expression->s_expression() + "\"");
            case ast_node::TUPLE_LITERAL_EXPR:
                return generate_expr_tuple_literal(
                        std::reinterpret_pointer_cast<ast_node::tuple_literal_expr_node>(expression), constants, stack,
                        debug);
            case ast_node::UNOP_EXPR:
                return generate_expr_unop(std::reinterpret_pointer_cast<ast_node::unop_expr_node>(expression),
                                          constants, stack, debug);
            case ast_node::VARIABLE_EXPR:
                throw std::runtime_error("Unhandled expression: \"" + expression->s_expression() + "\"");
            default:
                throw std::runtime_error("Invalid expression: \"" + expression->s_expression() + "\"");
        }
    }

    std::string generate_expr_array_literal(const std::shared_ptr<ast_node::array_literal_expr_node>& expression,
                                            const_table& constants, stack_info& stack, bool debug) {
        std::stringstream assembly;
        constexpr unsigned int reg_size = 8;

        if (debug) assembly << "\t;  START generate_expr_array_literal\n";

        //  Generate assembly from right to left.
        unsigned int total = 0;
        for (int index = (int)(expression->expressions.size() - 1); index >= 0; index--) {
            assembly << generate_expr(expression->expressions[index], constants, stack, debug);
            total += stack.pop();
        }
        stack.push(total);

        assembly << "\tmov rdi, " << total << "\n";

        const bool needs_alignment = stack.needs_alignment();
        if (needs_alignment) assembly << "\tsub rsp, 8 ; Align stack\n";

        assembly << "\tcall _jpl_alloc\n";

        if (needs_alignment) assembly << "\tadd rsp, 8 ; Remove alignment\n";

        if (debug) assembly << "\t;  Moving " << total << " bytes from rsp to rax\n";

        for (unsigned int offset = reg_size; offset <= total; offset += reg_size) {
            //  Extra indentation for debug mode.
            if (debug) assembly << "\t";
            assembly << "\tmov r10, [rsp + " << total - offset << "]\n";

            if (debug) assembly << "\t";
            assembly << "\tmov [rax + " << total - offset << "], r10\n";
        }

        assembly << "\tadd rsp, " << total << "\n";
        stack.pop();
        assembly << "\tpush rax\n";
        //  NOTE: This will probably need to be updated for a rank-2 array.
        assembly << "\tmov rax, " << expression->expressions.size() << "\n"
                 << "\tpush rax\n";
        stack.push(expression->r_type->size());

        if (debug) assembly << "\t;  END generate_expr_array_literal\n";

        return assembly.str();
    }

    std::string generate_expr_binop(const std::shared_ptr<ast_node::binop_expr_node>& expression,
                                    const_table& constants, stack_info& stack, bool debug) {
        std::stringstream assembly;

        if (debug) assembly << "\t;  START generate_expr_binop\n";

        assembly << generate_expr(expression->right_operand, constants, stack, debug)
                 << generate_expr(expression->left_operand, constants, stack, debug);

        const resolved_type::resolved_type_type operand_type = expression->left_operand->r_type->type;

        //  Because we're popping twice and then pushing once before making a function call, we invert the stack needing
        //  alignment.
        const bool needs_alignment = stack.needs_alignment();

        std::string next_jump;
        if (operand_type == resolved_type::resolved_type_type::INT_TYPE) {
            assembly << "\tpop rax\n";
            stack.pop();
            assembly << "\tpop r10\n";
            stack.pop();
            switch (expression->operator_type) {
                case ast_node::BINOP_PLUS:
                    assembly << "\tadd rax, r10\n";
                    break;
                case ast_node::BINOP_MINUS:
                    assembly << "\tsub rax, r10\n";
                    break;
                case ast_node::BINOP_TIMES:
                    assembly << "\timul rax, r10\n";
                    break;
                case ast_node::BINOP_DIVIDE:
                    next_jump = constants.next_jump();
                    assembly << "\tcmp r10, 0\n"
                             << "\tjne " << next_jump << "\n";

                    if (needs_alignment) {
                        assembly << "\tsub rsp, 8 ; Align stack\n";
                        stack.push();
                    }

                    assembly << "\tlea rdi, [rel " << constants[{"divide by zero"}] << "]\n"
                             << "\tcall _fail_assertion\n";

                    if (needs_alignment) {
                        assembly << "\tadd rsp, 8 ; Remove alignment\n";
                        stack.pop();
                    }

                    assembly << next_jump << ":\n"
                             << "\tcqo\n"
                             << "\tidiv r10\n";
                    break;
                case ast_node::BINOP_MOD:
                    next_jump = constants.next_jump();
                    assembly << "\tcmp r10, 0\n"
                             << "\tjne " << next_jump << "\n";

                    if (needs_alignment) {
                        assembly << "\tsub rsp, 8 ; Align stack\n";
                        stack.push();
                    }
                    assembly << "\tlea rdi, [rel " << constants[{"mod by zero"}] << "]\n"
                             << "\tcall _fail_assertion\n";
                    if (needs_alignment) {
                        assembly << "\tadd rsp, 8 ; Remove alignment\n";
                        stack.pop();
                    }

                    assembly << next_jump << ":\n"
                             << "\tcqo\n"
                             << "\tidiv r10\n"
                             << "\tmov rax, rdx\n";
                    break;
                case ast_node::BINOP_LT:
                    assembly << "\tcmp rax, r10\n"
                             << "\tsetl al\n"
                             << "\tand rax, 1\n";
                    break;
                case ast_node::BINOP_GT:
                    assembly << "\tcmp rax, r10\n"
                             << "\tsetg al\n"
                             << "\tand rax, 1\n";
                    break;
                case ast_node::BINOP_EQ:
                    assembly << "\tcmp rax, r10\n"
                             << "\tsete al\n"
                             << "\tand rax, 1\n";
                    break;
                case ast_node::BINOP_NEQ:
                    assembly << "\tcmp rax, r10\n"
                             << "\tsetne al\n"
                             << "\tand rax, 1\n";
                    break;
                case ast_node::BINOP_LEQ:
                    assembly << "\tcmp rax, r10\n"
                             << "\tsetle al\n"
                             << "\tand rax, 1\n";
                    break;
                case ast_node::BINOP_GEQ:
                    assembly << "\tcmp rax, r10\n"
                             << "\tsetge al\n"
                             << "\tand rax, 1\n";
                    break;
                default:
                    throw std::runtime_error("Invalid expression: \"" + expression->s_expression() + "\"");
            }
            assembly << "\tpush rax\n";
            stack.push();
        } else if (operand_type == resolved_type::resolved_type_type::FLOAT_TYPE) {
            assembly << "\tmovsd xmm0, [rsp]\n"
                     << "\tadd rsp, 8\n";
            stack.pop();
            assembly << "\tmovsd xmm1, [rsp]\n"
                     << "\tadd rsp, 8\n";
            stack.pop();
            switch (expression->operator_type) {
                case ast_node::BINOP_PLUS:
                    assembly << "\taddsd xmm0, xmm1\n"
                             << "\tsub rsp, 8\n";
                    stack.push();
                    assembly << "\tmovsd [rsp], xmm0\n";
                    break;
                case ast_node::BINOP_MINUS:
                    assembly << "\tsubsd xmm0, xmm1\n"
                             << "\tsub rsp, 8\n";
                    stack.push();
                    assembly << "\tmovsd [rsp], xmm0\n";
                    break;
                case ast_node::BINOP_TIMES:
                    assembly << "\tmulsd xmm0, xmm1\n"
                             << "\tsub rsp, 8\n";
                    stack.push();
                    assembly << "\tmovsd [rsp], xmm0\n";
                    break;
                case ast_node::BINOP_DIVIDE:
                    assembly << "\tdivsd xmm0, xmm1\n"
                             << "\tsub rsp, 8\n";
                    stack.push();
                    assembly << "\tmovsd [rsp], xmm0\n";
                    break;
                case ast_node::BINOP_MOD:
                    assembly << "\tcall _fmod\n"
                             << "\tsub rsp, 8\n";
                    stack.push();
                    assembly << "\tmovsd [rsp], xmm0\n";
                    break;
                case ast_node::BINOP_LT:
                    assembly << "\tcmpltsd xmm0, xmm1\n"
                             << "\tmovq rax, xmm0\n"
                             << "\tand rax, 1\n"
                             << "\tpush rax\n";
                    stack.push();
                    break;
                case ast_node::BINOP_GT:
                    assembly << "\tcmpltsd xmm1, xmm0\n"
                             << "\tmovq rax, xmm1\n"
                             << "\tand rax, 1\n"
                             << "\tpush rax\n";
                    stack.push();
                    break;
                case ast_node::BINOP_EQ:
                    assembly << "\tcmpeqsd xmm0, xmm1\n"
                             << "\tmovq rax, xmm0\n"
                             << "\tand rax, 1\n"
                             << "\tpush rax\n";
                    stack.push();
                    break;
                case ast_node::BINOP_NEQ:
                    assembly << "\tcmpneqsd xmm0, xmm1\n"
                             << "\tmovq rax, xmm0\n"
                             << "\tand rax, 1\n"
                             << "\tpush rax\n";
                    stack.push();
                    break;
                case ast_node::BINOP_LEQ:
                    assembly << "\tcmplesd xmm0, xmm1\n"
                             << "\tmovq rax, xmm0\n"
                             << "\tand rax, 1\n"
                             << "\tpush rax\n";
                    stack.push();
                    break;
                case ast_node::BINOP_GEQ:
                    assembly << "\tcmplesd xmm1, xmm0\n"
                             << "\tmovq rax, xmm1\n"
                             << "\tand rax, 1\n"
                             << "\tpush rax\n";
                    stack.push();
                    break;
                default:
                    throw std::runtime_error("Invalid expression: \"" + expression->s_expression() + "\"");
            }
        } else if (operand_type == resolved_type::resolved_type_type::BOOL_TYPE) {
            switch (expression->operator_type) {
                case ast_node::BINOP_EQ:
                    assembly << "\tpop rax\n";
                    stack.pop();
                    assembly << "\tpop r10\n";
                    stack.pop();
                    assembly << "\tcmp rax, r10\n"
                             << "\tsete al\n"
                             << "\tand rax, 1\n"
                             << "\tpush rax\n";
                    stack.push();
                    break;
                case ast_node::BINOP_NEQ:
                    assembly << "\tpop rax\n";
                    stack.pop();
                    assembly << "\tpop r10\n";
                    stack.pop();
                    assembly << "\tcmp rax, r10\n"
                             << "\tsetne al\n"
                             << "\tand rax, 1\n"
                             << "\tpush rax\n";
                    stack.push();
                    break;
                case ast_node::BINOP_AND:
                    //  TODO (HW10): Implement.
                case ast_node::BINOP_OR:
                    //  TODO (HW10): Implement.
                    throw std::runtime_error("Unimplemented expression: \"" + expression->s_expression() + "\"");
                default:
                    throw std::runtime_error("Invalid expression: \"" + expression->s_expression() + "\"");
            }
        }

        if (debug) assembly << "\t;  END generate_expr_binop\n";

        return assembly.str();
    }

    std::string generate_expr_false(const std::shared_ptr<ast_node::false_expr_node>&, const_table& constants,
                                    stack_info& stack, bool debug) {
        std::stringstream assembly;

        if (debug) assembly << "\t;  START generate_expr_false\n";

        assembly << "\tmov rax, [rel " << constants[(long)0] << "] ; False\n"
                 << "\tpush rax\n";
        stack.push();

        if (debug) assembly << "\t;  END generate_expr_false\n";

        return assembly.str();
    }

    std::string generate_expr_float(const std::shared_ptr<ast_node::float_expr_node>& expression,
                                    const_table& constants, stack_info& stack, bool debug) {
        std::stringstream assembly;

        if (debug) assembly << "\t;  START generate_expr_float\n";

        assembly << "\tmov rax, [rel " << constants[expression->value] << "] ; " << expression->value << "\n"
                 << "\tpush rax\n";
        stack.push();

        if (debug) assembly << "\t;  END generate_expr_float\n";

        return assembly.str();
    }

    std::string generate_expr_integer(const std::shared_ptr<ast_node::integer_expr_node>& expression,
                                      const_table& constants, stack_info& stack, bool debug) {
        std::stringstream assembly;

        if (debug) assembly << "\t;  START generate_expr_integer\n";

        assembly << "\tmov rax, [rel " << constants[expression->value] << "] ; " << expression->value << "\n"
                 << "\tpush rax\n";
        stack.push();

        if (debug) assembly << "\t;  END generate_expr_integer\n";

        return assembly.str();
    }

    std::string generate_expr_true(const std::shared_ptr<ast_node::true_expr_node>&, const_table& constants,
                                   stack_info& stack, bool debug) {
        std::stringstream assembly;

        if (debug) assembly << "\t;  START generate_expr_true\n";

        assembly << "\tmov rax, [rel " << constants[(long)1] << "] ; True\n"
                 << "\tpush rax\n";
        stack.push();

        if (debug) assembly << "\t;  END generate_expr_true\n";

        return assembly.str();
    }

    std::string generate_expr_tuple_literal(const std::shared_ptr<ast_node::tuple_literal_expr_node>& expression,
                                            const_table& constants, stack_info& stack, bool debug) {
        std::stringstream assembly;

        if (debug) assembly << "\t;  START generate_expr_tuple_literal\n";

        //  Generate assembly from right to left.
        for (int index = (int)(expression->exprs.size() - 1); index >= 0; index--) {
            assembly << generate_expr(expression->exprs[index], constants, stack, debug);
            stack.pop();
        }

        stack.push(expression->r_type->size());

        if (debug) assembly << "\t;  END generate_expr_tuple_literal\n";

        return assembly.str();
    }

    std::string generate_expr_unop(const std::shared_ptr<ast_node::unop_expr_node>& expression, const_table& constants,
                                   stack_info& stack, bool debug) {
        std::stringstream assembly;

        if (debug) assembly << "\t;  START generate_expr_unop\n";

        assembly << generate_expr(expression->operand, constants, stack, debug);

        if (expression->operator_type == ast_node::UNOP_INV) {
            assembly << "\tpop rax\n";
            stack.pop();
            assembly << "\txor rax, 1\n"
                     << "\tpush rax\n";
            stack.push();
        } else {
            if (expression->r_type->type == resolved_type::resolved_type_type::INT_TYPE) {
                assembly << "\tpop rax\n";
                stack.pop();
                assembly << "\tneg rax\n"
                         << "\tpush rax\n";
                stack.push();
            } else {
                assembly << "\tmovsd xmm1, [rsp]\n"
                         << "\tadd rsp, 8\n";
                stack.pop();
                assembly << "\tpxor xmm0, xmm0\n"
                         << "\tsubsd xmm0, xmm1\n"
                         << "\tsub rsp, 8\n";
                stack.push();
                assembly << "\tmovsd [rsp], xmm0\n";
            }
        }

        if (debug) assembly << "\t;  END generate_expr_unop\n";

        return assembly.str();
    }
}  //  namespace generator
