/**
 * @file generator.cpp
 * @package Assignments 9-10
 * @author Cayden Lund (u1182408)
 * @brief Implements the assembly `generator` class, and associated subtypes.
 * @details See the header file for documentation.
 *
 */

#include <sstream>

#include "generator.hpp"

namespace generator {
    //  ==============
    //  ||  Types:  ||
    //  ==============

    generator::const_table::table_entry::table_entry(long int_value)
        : type(INT_ENTRY), int_value(int_value), float_value(), string_value() {}

    generator::const_table::table_entry::table_entry(double float_value)
        : type(FLOAT_ENTRY), int_value(), float_value(float_value), string_value() {}

    generator::const_table::table_entry::table_entry(std::string string_value)
        : type(STRING_ENTRY), int_value(), float_value(), string_value(std::move(string_value)) {}

    bool generator::const_table::table_entry::operator==(const const_table::table_entry& other) const {
        return this->type == other.type && this->int_value == other.int_value && this->float_value == other.float_value
            && this->string_value == other.string_value;
    }

    std::string generator::const_table::table_entry::assem() const {
        if (this->type == INT_ENTRY) { return "dq " + std::to_string(this->int_value); }

        if (this->type == FLOAT_ENTRY) {
            const std::string format = "dq %.10g";
            constexpr unsigned int buf_size = 100;
            const std::unique_ptr<char[]> buf(new char[buf_size]);
            const unsigned int result_size = std::snprintf(buf.get(), buf_size, format.c_str(), this->float_value);
            std::string result(buf.get(), buf.get() + result_size);

            if (result.find('.') == std::string::npos && result.find('e') == std::string::npos) result += ".0";

            return result;
        }

        return "db `" + this->string_value + "`, 0";
    }

    size_t generator::const_table::table_entry_hash::operator()(const table_entry& entry) const {
        return std::hash<long>()(entry.int_value) + std::hash<double>()(entry.float_value)
             + std::hash<std::string>()(entry.string_value);
    }

    generator::const_table::const_table() : next_jump_number(1) { this->assembly << "section .data\n"; }

    std::string generator::const_table::operator[](const const_table::table_entry& entry) {
        if (this->constants.count(entry) > 0) return this->constants[entry];

        const std::string name = "const" + std::to_string(this->constants.size());
        this->assembly << name << ": " << entry.assem() << "\n";

        this->constants[entry] = name;

        return name;
    }

    std::string generator::const_table::assem() const { return this->assembly.str(); }

    std::string generator::const_table::next_jump() { return ".jump" + std::to_string(this->next_jump_number++); }

    void generator::generate_linking_preface() {
        this->linking_preface_assembly << "global jpl_main\n"
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
    }

    void generator::generate_commands() {
        this->main_assembly << "jpl_main:\n"
                            << "_jpl_main:\n"
                            << "\tpush rbp\n"
                            << "\tmov rbp, rsp\n"
                            << "\tpush r12\n";
        this->stack.push();
        this->main_assembly << "\tmov r12, rbp\n";

        for (const std::shared_ptr<ast_node::ast_node>& node : this->nodes) {
            this->generate_cmd(std::reinterpret_pointer_cast<ast_node::cmd_node>(node));
        }

        const unsigned int vars_size = this->stack.pop_all_vars();
        if (vars_size > 0) {
            this->main_assembly << "\tadd rsp, " << vars_size;
            if (this->debug) this->main_assembly << " ; Remove local variables";
            this->main_assembly << "\n";
        }

        this->main_assembly << "\tpop r12\n";
        this->stack.pop();
        this->main_assembly << "\tpop rbp\n";
        this->stack.pop();
        this->main_assembly << "\tret\n";
    }

    //  Commands:
    //  ---------

    void generator::generate_cmd(const std::shared_ptr<ast_node::cmd_node>& command) {
        switch (command->type) {
            case ast_node::ASSERT_CMD:
            case ast_node::FN_CMD:
                //  TODO (HW10): Implement.
                throw std::runtime_error("`generate_cmd`: unhandled command: \"" + command->s_expression() + "\"");
            case ast_node::LET_CMD:
                return this->generate_cmd_let(std::reinterpret_pointer_cast<ast_node::let_cmd_node>(command));
            case ast_node::PRINT_CMD:
            case ast_node::READ_CMD:
                throw std::runtime_error("`generate_cmd`: unhandled command: \"" + command->s_expression() + "\"");
            case ast_node::SHOW_CMD:
                return this->generate_cmd_show(std::reinterpret_pointer_cast<ast_node::show_cmd_node>(command));
            case ast_node::TIME_CMD:
            case ast_node::TYPE_CMD:
            case ast_node::WRITE_CMD:
                throw std::runtime_error("`generate_cmd`: unhandled command: \"" + command->s_expression() + "\"");
            default:
                throw std::runtime_error("`generate_cmd`: invalid command: \"" + command->s_expression() + "\"");
        }
    }

    void generator::generate_cmd_let(const std::shared_ptr<ast_node::let_cmd_node>& command) {
        if (this->debug) this->main_assembly << "\t;  START generate_cmd_let\n";

        this->main_assembly << generate_expr(command->expr);
        const unsigned int size = stack.pop();

        //  TODO (HW10): Implement.
        if (command->lvalue->type == ast_node::ARGUMENT_LVALUE) {
            const std::shared_ptr<ast_node::argument_node> argument
                    = std::reinterpret_pointer_cast<ast_node::argument_lvalue_node>(command->lvalue)->argument;

            if (argument->type == ast_node::ARRAY_ARGUMENT) {
                //  TODO (HW10): Implement.
                throw std::runtime_error("`generate_cmd_let`: unimplemented lvalue argument: \""
                                         + argument->s_expression() + "\"");
            } else {
                //  TODO (HW10): Implement.
                const std::shared_ptr<ast_node::variable_argument_node> variable_argument
                        = std::reinterpret_pointer_cast<ast_node::variable_argument_node>(argument);

                stack.push(variable_argument->name, size);
            }
        } else {
            //  TODO (HW10): Implement.
            throw std::runtime_error("`generate_cmd_let`: unimplemented lvalue: \"" + command->lvalue->s_expression()
                                     + "\"");
        }

        if (this->debug) this->main_assembly << "\t;  END generate_cmd_let\n";
    }

    void generator::generate_cmd_show(const std::shared_ptr<ast_node::show_cmd_node>& command) {
        if (this->debug) this->main_assembly << "\t;  START generate_cmd_show\n";

        //  We need to know the size of the expression on the stack in order to align the stack,
        //  so we push it onto the stack and then pop it again before generating the expression.
        this->stack.push(command->expr->r_type->size());
        const bool needs_alignment = this->stack.needs_alignment();
        this->stack.pop();
        if (needs_alignment) {
            this->main_assembly << "\tsub rsp, 8";
            if (this->debug) this->main_assembly << " ; Align stack\n";
            this->main_assembly << "\n";
            this->stack.push();
        }

        this->main_assembly << this->generate_expr(command->expr) << "\tlea rdi, [rel "
                            << constants[command->expr->r_type->s_expression()] << "]";
        if (this->debug) {
            this->main_assembly << " ; ";
            this->main_assembly << command->expr->r_type->s_expression();
        }
        this->main_assembly << "\n"
                            << "\tlea rsi, [rsp]\n"
                            << "\tcall _show\n"
                            << "\tadd rsp, " << this->stack.pop() << "\n";

        if (needs_alignment) {
            this->main_assembly << "\tadd rsp, 8";
            if (this->debug) this->main_assembly << " ; Remove alignment\n";
            this->main_assembly << "\n";
            this->stack.pop();
        }

        if (this->debug) this->main_assembly << "\t;  END generate_cmd_show\n";
    }

    //  Expressions:
    //  ------------

    std::string generator::generate_expr(const std::shared_ptr<ast_node::expr_node>& expression) {
        switch (expression->type) {
            case ast_node::ARRAY_INDEX_EXPR:
                throw std::runtime_error("`generate_expr`: Unhandled expression: \"" + expression->s_expression()
                                         + "\"");
            case ast_node::ARRAY_LITERAL_EXPR:
                return this->generate_expr_array_literal(
                        std::reinterpret_pointer_cast<ast_node::array_literal_expr_node>(expression));
            case ast_node::ARRAY_LOOP_EXPR:
                throw std::runtime_error("`generate_expr`: unhandled expression: \"" + expression->s_expression()
                                         + "\"");
            case ast_node::BINOP_EXPR:
                return this->generate_expr_binop(std::reinterpret_pointer_cast<ast_node::binop_expr_node>(expression));
            case ast_node::CALL_EXPR:
                return this->generate_expr_call(std::reinterpret_pointer_cast<ast_node::call_expr_node>(expression));
            case ast_node::FALSE_EXPR:
                return this->generate_expr_false(std::reinterpret_pointer_cast<ast_node::false_expr_node>(expression));
            case ast_node::FLOAT_EXPR:
                return this->generate_expr_float(std::reinterpret_pointer_cast<ast_node::float_expr_node>(expression));
            case ast_node::IF_EXPR:
                throw std::runtime_error("`generate_expr`: unhandled expression: \"" + expression->s_expression()
                                         + "\"");
            case ast_node::INTEGER_EXPR:
                return this->generate_expr_integer(
                        std::reinterpret_pointer_cast<ast_node::integer_expr_node>(expression));
            case ast_node::SUM_LOOP_EXPR:
                throw std::runtime_error("`generate_expr`: unhandled expression: \"" + expression->s_expression()
                                         + "\"");
            case ast_node::TRUE_EXPR:
                return this->generate_expr_true(std::reinterpret_pointer_cast<ast_node::true_expr_node>(expression));
            case ast_node::TUPLE_INDEX_EXPR:
                return this->generate_expr_tuple_index(
                        std::reinterpret_pointer_cast<ast_node::tuple_index_expr_node>(expression));
            case ast_node::TUPLE_LITERAL_EXPR:
                return this->generate_expr_tuple_literal(
                        std::reinterpret_pointer_cast<ast_node::tuple_literal_expr_node>(expression));
            case ast_node::UNOP_EXPR:
                return this->generate_expr_unop(std::reinterpret_pointer_cast<ast_node::unop_expr_node>(expression));
            case ast_node::VARIABLE_EXPR:
                return this->generate_expr_variable(
                        std::reinterpret_pointer_cast<ast_node::variable_expr_node>(expression));
            default:
                throw std::runtime_error("`generate_expr`: invalid expression: \"" + expression->s_expression() + "\"");
        }
    }

    std::string
    generator::generate_expr_array_literal(const std::shared_ptr<ast_node::array_literal_expr_node>& expression) {
        std::stringstream assembly;
        constexpr unsigned int reg_size = 8;

        if (this->debug) assembly << "\t;  START generate_expr_array_literal\n";

        //  Generate assembly from right to left.
        for (long index = (long)(expression->expressions.size() - 1); index >= 0; index--) {
            assembly << this->generate_expr(expression->expressions[index]);
        }

        //  We don't pop from the stack as we go, because sub-expressions need to know the status of the stack.
        //  We need to pop it all now, though, and replace it with one number.
        unsigned long total_size = 0;
        for (long index = 0; index < (long)expression->expressions.size(); index++) { total_size += this->stack.pop(); }
        this->stack.push(total_size);

        assembly << "\tmov rdi, " << total_size << "\n";

        const bool needs_alignment = this->stack.needs_alignment();
        if (needs_alignment) {
            assembly << "\tsub rsp, 8";
            if (this->debug) assembly << " ; Align stack\n";
            assembly << "\n";
            this->stack.push();
        }

        assembly << "\tcall _jpl_alloc\n";

        if (needs_alignment) {
            assembly << "\tadd rsp, 8";
            if (this->debug) assembly << " ; Remove alignment\n";
            assembly << "\n";
            this->stack.pop();
        }

        if (this->debug) assembly << "\t;  Moving " << total_size << " bytes from rsp to rax\n";

        for (unsigned int offset = reg_size; offset <= total_size; offset += reg_size) {
            //  Extra indentation for debug mode.
            if (this->debug) assembly << "\t";
            assembly << "\tmov r10, [rsp + " << total_size - offset << "]\n";

            if (this->debug) assembly << "\t";
            assembly << "\tmov [rax + " << total_size - offset << "], r10\n";
        }

        assembly << "\tadd rsp, " << total_size << "\n";
        this->stack.pop();
        assembly << "\tpush rax\n";
        //  NOTE: This will probably need to be updated for a rank-2 array.
        assembly << "\tmov rax, " << expression->expressions.size() << "\n"
                 << "\tpush rax\n";
        this->stack.push(expression->r_type->size());

        if (this->debug) assembly << "\t;  END generate_expr_array_literal\n";

        return assembly.str();
    }

    std::string generator::generate_expr_binop(const std::shared_ptr<ast_node::binop_expr_node>& expression) {
        std::stringstream assembly;

        if (this->debug) assembly << "\t;  START generate_expr_binop\n";

        assembly << this->generate_expr(expression->right_operand) << this->generate_expr(expression->left_operand);

        const resolved_type::resolved_type_type operand_type = expression->left_operand->r_type->type;

        //  Because we're popping twice and then pushing once before making a function call, we invert the stack needing
        //  alignment.
        const bool needs_alignment = this->stack.needs_alignment();

        std::string next_jump;
        if (operand_type == resolved_type::resolved_type_type::INT_TYPE) {
            assembly << "\tpop rax\n";
            this->stack.pop();
            assembly << "\tpop r10\n";
            this->stack.pop();
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
                        assembly << "\tsub rsp, 8";
                        if (this->debug) assembly << " ; Align stack\n";
                        assembly << "\n";
                        this->stack.push();
                    }

                    assembly << "\tlea rdi, [rel " << constants[{"divide by zero"}] << "]\n"
                             << "\tcall _fail_assertion\n";

                    if (needs_alignment) {
                        assembly << "\tadd rsp, 8";
                        if (this->debug) assembly << " ; Remove alignment\n";
                        assembly << "\n";
                        this->stack.pop();
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
                        assembly << "\tsub rsp, 8";
                        if (this->debug) assembly << " ; Align stack\n";
                        assembly << "\n";
                        this->stack.push();
                    }
                    assembly << "\tlea rdi, [rel " << constants[{"mod by zero"}] << "]\n"
                             << "\tcall _fail_assertion\n";
                    if (needs_alignment) {
                        assembly << "\tadd rsp, 8";
                        if (this->debug) assembly << " ; Remove alignment\n";
                        assembly << "\n";
                        this->stack.pop();
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
                    throw std::runtime_error("`generate_expr_binop`: Invalid expression: \""
                                             + expression->s_expression() + "\"");
            }
            assembly << "\tpush rax\n";
            this->stack.push();
        } else if (operand_type == resolved_type::resolved_type_type::FLOAT_TYPE) {
            assembly << "\tmovsd xmm0, [rsp]\n"
                     << "\tadd rsp, 8\n";
            this->stack.pop();
            assembly << "\tmovsd xmm1, [rsp]\n"
                     << "\tadd rsp, 8\n";
            this->stack.pop();
            switch (expression->operator_type) {
                case ast_node::BINOP_PLUS:
                    assembly << "\taddsd xmm0, xmm1\n"
                             << "\tsub rsp, 8\n";
                    this->stack.push();
                    assembly << "\tmovsd [rsp], xmm0\n";
                    break;
                case ast_node::BINOP_MINUS:
                    assembly << "\tsubsd xmm0, xmm1\n"
                             << "\tsub rsp, 8\n";
                    this->stack.push();
                    assembly << "\tmovsd [rsp], xmm0\n";
                    break;
                case ast_node::BINOP_TIMES:
                    assembly << "\tmulsd xmm0, xmm1\n"
                             << "\tsub rsp, 8\n";
                    this->stack.push();
                    assembly << "\tmovsd [rsp], xmm0\n";
                    break;
                case ast_node::BINOP_DIVIDE:
                    assembly << "\tdivsd xmm0, xmm1\n"
                             << "\tsub rsp, 8\n";
                    this->stack.push();
                    assembly << "\tmovsd [rsp], xmm0\n";
                    break;
                case ast_node::BINOP_MOD:
                    assembly << "\tcall _fmod\n"
                             << "\tsub rsp, 8\n";
                    this->stack.push();
                    assembly << "\tmovsd [rsp], xmm0\n";
                    break;
                case ast_node::BINOP_LT:
                    assembly << "\tcmpltsd xmm0, xmm1\n"
                             << "\tmovq rax, xmm0\n"
                             << "\tand rax, 1\n"
                             << "\tpush rax\n";
                    this->stack.push();
                    break;
                case ast_node::BINOP_GT:
                    assembly << "\tcmpltsd xmm1, xmm0\n"
                             << "\tmovq rax, xmm1\n"
                             << "\tand rax, 1\n"
                             << "\tpush rax\n";
                    this->stack.push();
                    break;
                case ast_node::BINOP_EQ:
                    assembly << "\tcmpeqsd xmm0, xmm1\n"
                             << "\tmovq rax, xmm0\n"
                             << "\tand rax, 1\n"
                             << "\tpush rax\n";
                    this->stack.push();
                    break;
                case ast_node::BINOP_NEQ:
                    assembly << "\tcmpneqsd xmm0, xmm1\n"
                             << "\tmovq rax, xmm0\n"
                             << "\tand rax, 1\n"
                             << "\tpush rax\n";
                    this->stack.push();
                    break;
                case ast_node::BINOP_LEQ:
                    assembly << "\tcmplesd xmm0, xmm1\n"
                             << "\tmovq rax, xmm0\n"
                             << "\tand rax, 1\n"
                             << "\tpush rax\n";
                    this->stack.push();
                    break;
                case ast_node::BINOP_GEQ:
                    assembly << "\tcmplesd xmm1, xmm0\n"
                             << "\tmovq rax, xmm1\n"
                             << "\tand rax, 1\n"
                             << "\tpush rax\n";
                    this->stack.push();
                    break;
                default:
                    throw std::runtime_error("`generate_expr_binop`: invalid expression: \""
                                             + expression->s_expression() + "\"");
            }
        } else if (operand_type == resolved_type::resolved_type_type::BOOL_TYPE) {
            switch (expression->operator_type) {
                case ast_node::BINOP_EQ:
                    assembly << "\tpop rax\n";
                    this->stack.pop();
                    assembly << "\tpop r10\n";
                    this->stack.pop();
                    assembly << "\tcmp rax, r10\n"
                             << "\tsete al\n"
                             << "\tand rax, 1\n"
                             << "\tpush rax\n";
                    this->stack.push();
                    break;
                case ast_node::BINOP_NEQ:
                    assembly << "\tpop rax\n";
                    this->stack.pop();
                    assembly << "\tpop r10\n";
                    this->stack.pop();
                    assembly << "\tcmp rax, r10\n"
                             << "\tsetne al\n"
                             << "\tand rax, 1\n"
                             << "\tpush rax\n";
                    this->stack.push();
                    break;
                case ast_node::BINOP_AND:
                    //  TODO (HW11): Implement.
                case ast_node::BINOP_OR:
                    //  TODO (HW11): Implement.
                    throw std::runtime_error("`generate_expr_binop`: unimplemented expression: \""
                                             + expression->s_expression() + "\"");
                default:
                    throw std::runtime_error("`generate_expr_binop`: invalid expression: \""
                                             + expression->s_expression() + "\"");
            }
        }

        if (this->debug) assembly << "\t;  END generate_expr_binop\n";

        return assembly.str();
    }

    std::string generator::generate_expr_call(const std::shared_ptr<ast_node::call_expr_node>& expression) {
        std::stringstream assembly;

        if (this->debug) assembly << "\t;  START generate_expr_call\n";

        //  TODO (HW10): Implement.

        //  Push call arguments onto the stack from right to left.
        for (int index = expression->call_args.size() - 1; index >= 0; index--) {
            assembly << generate_expr(expression->call_args[index]);
        }

        //  TODO (HW10): Add more.
        const std::vector<std::string> float_regs = {"xmm0"};
        unsigned int float_reg_index = 0;

        //  Next, we move arguments on the stack into their proper places.
        for (const std::shared_ptr<ast_node::expr_node>& arg : expression->call_args) {
            switch (arg->r_type->type) {
                case resolved_type::BOOL_TYPE:
                    throw std::runtime_error("`generate_expr_call`: unhandled call argument type: \""
                                             + arg->s_expression() + "\"");
                    break;
                case resolved_type::FLOAT_TYPE:
                    assembly << "\tmovsd " << float_regs[float_reg_index++] << "\n"
                             << "\tadd rsp, " << this->stack.pop() << "\n";
                    break;
                case resolved_type::INT_TYPE:
                case resolved_type::ARRAY_TYPE:
                case resolved_type::TUPLE_TYPE:
                    throw std::runtime_error("`generate_expr_call`: unhandled call argument type: \""
                                             + arg->s_expression() + "\"");
            }
        }

        //  TODO (HW10): Stack alignment.
        assembly << "\tcall _" << expression->name << "\n";

        if (this->debug) assembly << "\t;  END generate_expr_call\n";

        return assembly.str();
    }

    std::string generator::generate_expr_false(const std::shared_ptr<ast_node::false_expr_node>&) {
        std::stringstream assembly;

        if (this->debug) assembly << "\t;  START generate_expr_false\n";

        assembly << "\tmov rax, [rel " << constants[(long)0] << "]";
        if (this->debug) assembly << " ; False";
        assembly << "\n\tpush rax\n";
        this->stack.push();

        if (this->debug) assembly << "\t;  END generate_expr_false\n";

        return assembly.str();
    }

    std::string generator::generate_expr_float(const std::shared_ptr<ast_node::float_expr_node>& expression) {
        std::stringstream assembly;

        if (this->debug) assembly << "\t;  START generate_expr_float\n";

        assembly << "\tmov rax, [rel " << constants[expression->value] << "]";
        if (this->debug) assembly << " ; " << expression->value;
        assembly << "\n\tpush rax\n";
        this->stack.push();

        if (this->debug) assembly << "\t;  END generate_expr_float\n";

        return assembly.str();
    }

    std::string generator::generate_expr_integer(const std::shared_ptr<ast_node::integer_expr_node>& expression) {
        std::stringstream assembly;

        if (this->debug) assembly << "\t;  START generate_expr_integer\n";

        assembly << "\tmov rax, [rel " << constants[expression->value] << "]";
        if (this->debug) assembly << " ; " << expression->value;
        assembly << "\n\tpush rax\n";
        this->stack.push();

        if (this->debug) assembly << "\t;  END generate_expr_integer\n";

        return assembly.str();
    }

    std::string generator::generate_expr_true(const std::shared_ptr<ast_node::true_expr_node>&) {
        std::stringstream assembly;

        if (this->debug) assembly << "\t;  START generate_expr_true\n";

        assembly << "\tmov rax, [rel " << constants[(long)1] << "]";
        if (this->debug) assembly << " ; True";
        assembly << "\n\tpush rax\n";
        this->stack.push();

        if (this->debug) assembly << "\t;  END generate_expr_true\n";

        return assembly.str();
    }

    std::string
    generator::generate_expr_tuple_index(const std::shared_ptr<ast_node::tuple_index_expr_node>& expression) {
        std::stringstream assembly;

        constexpr unsigned int reg_size = 8;

        if (this->debug) assembly << "\t;  START generate_expr_tuple_index\n";

        assembly << this->generate_expr(expression->expr);

        const unsigned int element_index = expression->index->value;
        const std::shared_ptr<resolved_type::tuple_resolved_type> tuple_type
                = std::reinterpret_pointer_cast<resolved_type::tuple_resolved_type>(expression->expr->r_type);
        const unsigned int element_size = tuple_type->element_types[element_index]->size();
        const unsigned int element_offset = tuple_type->offset(element_index);
        const unsigned int destination = tuple_type->size() - element_size;

        if (this->debug)
            assembly << "\t;  Moving " << element_size << " bytes from rsp + " << element_offset << " to rsp + "
                     << destination << "\n";

        for (int mov_offset = (int)(element_size - reg_size); mov_offset >= 0; mov_offset -= reg_size) {
            if (this->debug) assembly << "\t";
            assembly << "\tmov r10, [rsp + " << element_offset << " + " << mov_offset << "]\n";

            if (this->debug) assembly << "\t";
            assembly << "\tmov [rsp + " << destination << " + " << mov_offset << "], r10\n";
        }

        assembly << "\tadd rsp, " << tuple_type->size() - element_size << "\n";
        this->stack.pop();
        this->stack.push(element_size);

        if (this->debug) assembly << "\t;  END generate_expr_tuple_index\n";

        return assembly.str();
    }

    std::string
    generator::generate_expr_tuple_literal(const std::shared_ptr<ast_node::tuple_literal_expr_node>& expression) {
        std::stringstream assembly;

        if (this->debug) assembly << "\t;  START generate_expr_tuple_literal\n";

        //  Generate assembly from right to left.
        for (long index = (long)(expression->exprs.size() - 1); index >= 0; index--) {
            assembly << this->generate_expr(expression->exprs[index]);
        }

        //  We don't pop from the stack as we go, because sub-expressions need to know the status of the stack.
        //  We need to pop it all now, though, and replace it with one number.
        for (long index = 0; index < (long)(expression->exprs.size()); index++) { this->stack.pop(); }
        this->stack.push(expression->r_type->size());

        if (this->debug) assembly << "\t;  END generate_expr_tuple_literal\n";

        return assembly.str();
    }

    std::string generator::generate_expr_unop(const std::shared_ptr<ast_node::unop_expr_node>& expression) {
        std::stringstream assembly;

        if (this->debug) assembly << "\t;  START generate_expr_unop\n";

        assembly << this->generate_expr(expression->operand);

        if (expression->operator_type == ast_node::UNOP_INV) {
            assembly << "\tpop rax\n";
            this->stack.pop();
            assembly << "\txor rax, 1\n"
                     << "\tpush rax\n";
            this->stack.push();
        } else {
            if (expression->r_type->type == resolved_type::resolved_type_type::INT_TYPE) {
                assembly << "\tpop rax\n";
                this->stack.pop();
                assembly << "\tneg rax\n"
                         << "\tpush rax\n";
                this->stack.push();
            } else {
                assembly << "\tmovsd xmm1, [rsp]\n"
                         << "\tadd rsp, 8\n";
                this->stack.pop();
                assembly << "\tpxor xmm0, xmm0\n"
                         << "\tsubsd xmm0, xmm1\n"
                         << "\tsub rsp, 8\n";
                this->stack.push();
                assembly << "\tmovsd [rsp], xmm0\n";
            }
        }

        if (this->debug) assembly << "\t;  END generate_expr_unop\n";

        return assembly.str();
    }

    std::string generator::generate_expr_variable(const std::shared_ptr<ast_node::variable_expr_node>& expression) {
        std::stringstream assembly;
        constexpr unsigned int reg_size = 8;

        if (this->debug) assembly << "\t;  START generate_expr_variable\n";

        const unsigned int size = expression->r_type->size();
        assembly << "\tsub rsp, " << size << "\n";
        this->stack.push(size);

        const unsigned int rbp_offset = this->stack[expression->name];

        if (debug) assembly << "\t;  Moving " << size << " bytes from [rbp - " << rbp_offset << "] to [rsp]\n";

        for (unsigned int offset = reg_size; offset <= size; offset += reg_size) {
            //  Extra indentation for debug mode.
            if (this->debug) assembly << "\t";
            assembly << "\tmov r10, [rbp - " << rbp_offset + size - offset << "]\n";

            if (this->debug) assembly << "\t";
            assembly << "\tmov [rsp + " << size - offset << "], r10\n";
        }

        if (this->debug) assembly << "\t;  END generate_expr_variable\n";

        return assembly.str();
    }

    generator::generator(const std::vector<std::shared_ptr<ast_node::ast_node>>& nodes, bool debug)
        : debug(debug), nodes(nodes) {
        this->generate_linking_preface();
        this->generate_commands();
    }

    std::string generator::assem() const {
        std::stringstream assembly;

        assembly << this->linking_preface_assembly.str() << "\n"
                 << this->constants.assem() << "\n"
                 << "section .text\n"
                 << this->function_assemblies.str() << "\n"
                 << this->main_assembly.str() << "\n";

        return assembly.str();
    }

    //  ================
    //  ||  Methods:  ||
    //  ================

    std::string generate(const std::vector<std::shared_ptr<ast_node::ast_node>>& nodes, bool debug) {
        return generator(nodes, debug).assem();
    }
}  //  namespace generator
