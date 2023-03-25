/**
 * @file generator.cpp
 * @package Assignments 9-10
 * @author Cayden Lund (u1182408)
 * @brief Implements the assembly `generator` class, and associated subtypes.
 * @details See the header file for documentation.
 *
 */

#include <functional>
#include <sstream>

#include "generator.hpp"

namespace generator {
    //  ========================
    //  ||  Constants table:  ||
    //  ========================

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

    //  =======================
    //  ||  Variable table:  ||
    //  =======================

    generator::variable_table::variable_table(const std::shared_ptr<variable_table>& parent) : parent(parent) {}

    std::tuple<std::string, unsigned int> generator::variable_table::get_variable_address(const std::string& variable,
                                                                                          bool from_child) const {
        const std::string reg = (this->parent == nullptr && from_child) ? "r12" : "rbp";
        if (this->variables.count(variable) > 0) { return {reg, this->variables.at(variable)}; }

        return this->parent->get_variable_address(variable, true);
    }

    void generator::variable_table::set_variable_address(const std::string& variable, unsigned int offset) {
        this->variables[variable] = offset;
    }

    //  =======================
    //  ||  Base generator:  ||
    //  =======================

    //  Expressions:
    //  ------------

    std::string generator::generate_expr(const std::shared_ptr<ast_node::expr_node>& expression) {
        switch (expression->type) {
            case ast_node::ARRAY_INDEX_EXPR:
                return this->generate_expr_array_index(
                        std::reinterpret_pointer_cast<ast_node::array_index_expr_node>(expression));
            case ast_node::ARRAY_LITERAL_EXPR:
                return this->generate_expr_array_literal(
                        std::reinterpret_pointer_cast<ast_node::array_literal_expr_node>(expression));
            case ast_node::ARRAY_LOOP_EXPR:
                //  TODO (HW11): Implement.
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
                //  TODO (HW11): Implement.
                throw std::runtime_error("`generate_expr`: unhandled expression: \"" + expression->s_expression()
                                         + "\"");
            case ast_node::INTEGER_EXPR:
                return this->generate_expr_integer(
                        std::reinterpret_pointer_cast<ast_node::integer_expr_node>(expression));
            case ast_node::SUM_LOOP_EXPR:
                //  TODO (HW11): Implement.
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

    std::string generator::generate_expr_array_index(  //  NOLINT(readability-make-member-function-const)
            const std::shared_ptr<ast_node::array_index_expr_node>&) {
        std::stringstream assembly;

        if (this->debug) assembly << "\t;  START generate_expr_array_index\n";

        //  TODO (HW11): Implement.

        if (this->debug) assembly << "\t;  END generate_expr_array_index\n";

        return assembly.str();
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
            if (this->debug) assembly << " ; Align stack";
            assembly << "\n";
            this->stack.push();
        }

        assembly << "\tcall _jpl_alloc\n";

        if (needs_alignment) {
            assembly << "\tadd rsp, 8";
            if (this->debug) assembly << " ; Remove alignment";
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
                    next_jump = constants->next_jump();
                    assembly << "\tcmp r10, 0\n"
                             << "\tjne " << next_jump << "\n";

                    if (needs_alignment) {
                        assembly << "\tsub rsp, 8";
                        if (this->debug) assembly << " ; Align stack";
                        assembly << "\n";
                        this->stack.push();
                    }

                    assembly << "\tlea rdi, [rel " << (*this->constants)[{"divide by zero"}] << "]\n"
                             << "\tcall _fail_assertion\n";

                    if (needs_alignment) {
                        assembly << "\tadd rsp, 8";
                        if (this->debug) assembly << " ; Remove alignment";
                        assembly << "\n";
                        this->stack.pop();
                    }

                    assembly << next_jump << ":\n"
                             << "\tcqo\n"
                             << "\tidiv r10\n";
                    break;
                case ast_node::BINOP_MOD:
                    next_jump = constants->next_jump();
                    assembly << "\tcmp r10, 0\n"
                             << "\tjne " << next_jump << "\n";

                    if (needs_alignment) {
                        assembly << "\tsub rsp, 8";
                        if (this->debug) assembly << " ; Align stack";
                        assembly << "\n";
                        this->stack.push();
                    }
                    assembly << "\tlea rdi, [rel " << (*this->constants)[{"mod by zero"}] << "]\n"
                             << "\tcall _fail_assertion\n";
                    if (needs_alignment) {
                        assembly << "\tadd rsp, 8";
                        if (this->debug) assembly << " ; Remove alignment";
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

        const call_signature::call_signature& function = (*this->function_signatures)[expression->name];

        //  1.  If there's a struct return value, allocate space for it on the stack.
        if (function.ret_type->type == resolved_type::ARRAY_TYPE
            || function.ret_type->type == resolved_type::TUPLE_TYPE) {
            assembly << "\tsub rsp, " << function.ret_type->size();
            if (this->debug) assembly << " ; Return struct";
            assembly << "\n";
            this->stack.push(function.ret_type->size());
        }

        //  2.  Add padding so the final stack size, before the call, is a multiple of 16 bytes.
        this->stack.push(function.bytes_on_stack);
        const bool needs_alignment = this->stack.needs_alignment();
        this->stack.pop();
        if (needs_alignment) {
            assembly << "\tsub rsp, 8";
            if (this->debug) assembly << " ; Align stack";
            assembly << "\n";
            this->stack.push();
        }

        //  3.  Compute every stack argument in reverse order (so that they end up on the stack in the normal order).
        //  4.  Compute every register argument in reverse order.
        for (const unsigned int& index : function.push_order) {
            assembly << generate_expr(expression->call_args[index]);
        }

        //  5.  Pop all register arguments into their correct registers.
        for (const std::string& assem : function.pop_assem) {
            assembly << assem;
            this->stack.pop();
        }

        //  6.  If there's a struct return value, load its address into RDI.
        if (function.ret_type->type == resolved_type::ARRAY_TYPE
            || function.ret_type->type == resolved_type::TUPLE_TYPE) {
            //  TODO (HW10): Implement.
            throw std::runtime_error("`generate_expr_call`: unhandled return type: \""
                                     + function.ret_type->s_expression() + "\"");
        }

        //  7.  Execute the `call` instruction.
        assembly << "\tcall _" << expression->name << "\n";

        //  8.  Drop every stack argument.
        //      TODO (HW10): This.

        //  9.  Drop the padding, if any.
        if (needs_alignment) {
            assembly << "\tadd rsp, 8";
            if (this->debug) assembly << " ; Remove alignment";
            assembly << "\n";
            this->stack.pop();
        }

        //  10. If the return value is in a register, push it onto the stack.
        switch (function.ret_type->type) {
            case resolved_type::FLOAT_TYPE:
                assembly << "\tsub rsp, 8\n"
                         << "\tmovsd [rsp], xmm0\n";
                this->stack.push();
                break;
            case resolved_type::BOOL_TYPE:
            case resolved_type::INT_TYPE:
                assembly << "\tpush rax\n";
                this->stack.push();
            default:
                break;
        }

        if (this->debug) assembly << "\t;  END generate_expr_call\n";

        return assembly.str();
    }

    std::string generator::generate_expr_false(const std::shared_ptr<ast_node::false_expr_node>&) {
        std::stringstream assembly;

        if (this->debug) assembly << "\t;  START generate_expr_false\n";

        assembly << "\tmov rax, [rel " << (*this->constants)[(long)0] << "]";
        if (this->debug) assembly << " ; False";
        assembly << "\n\tpush rax\n";
        this->stack.push();

        if (this->debug) assembly << "\t;  END generate_expr_false\n";

        return assembly.str();
    }

    std::string generator::generate_expr_float(const std::shared_ptr<ast_node::float_expr_node>& expression) {
        std::stringstream assembly;

        if (this->debug) assembly << "\t;  START generate_expr_float\n";

        assembly << "\tmov rax, [rel " << (*this->constants)[expression->value] << "]";
        if (this->debug) assembly << " ; " << expression->value;
        assembly << "\n\tpush rax\n";
        this->stack.push();

        if (this->debug) assembly << "\t;  END generate_expr_float\n";

        return assembly.str();
    }

    std::string generator::generate_expr_integer(const std::shared_ptr<ast_node::integer_expr_node>& expression) {
        std::stringstream assembly;

        if (this->debug) assembly << "\t;  START generate_expr_integer\n";

        assembly << "\tmov rax, [rel " << (*this->constants)[expression->value] << "]";
        if (this->debug) assembly << " ; " << expression->value;
        assembly << "\n\tpush rax\n";
        this->stack.push();

        if (this->debug) assembly << "\t;  END generate_expr_integer\n";

        return assembly.str();
    }

    std::string generator::generate_expr_true(const std::shared_ptr<ast_node::true_expr_node>&) {
        std::stringstream assembly;

        if (this->debug) assembly << "\t;  START generate_expr_true\n";

        assembly << "\tmov rax, [rel " << (*this->constants)[(long)1] << "]";
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

        const std::tuple<std::string, unsigned int> variable_address = this->variables.get_variable_address(
                expression->name);
        const std::string reg = std::get<0>(variable_address);
        const unsigned int reg_offset = std::get<1>(variable_address);

        if (debug)
            assembly << "\t;  Moving " << size << " bytes from [" << reg << " - " << reg_offset << "] to [rsp]\n";

        for (unsigned int offset = reg_size; offset <= size; offset += reg_size) {
            //  Extra indentation for debug mode.
            if (this->debug) assembly << "\t";
            assembly << "\tmov r10, [" << reg << " - " << reg_offset - size + offset << "]\n";

            if (this->debug) assembly << "\t";
            assembly << "\tmov [rsp + " << size - offset << "], r10\n";
        }

        if (this->debug) assembly << "\t;  END generate_expr_variable\n";

        return assembly.str();
    }

    generator::generator(
            const std::shared_ptr<const_table>& constants,
            const std::shared_ptr<std::unordered_map<std::string, call_signature::call_signature>>& function_signatures,
            const std::shared_ptr<variable_table>& parent_variable_table, bool debug)
        : constants(constants), debug(debug), function_signatures(function_signatures),
          variables(parent_variable_table) {
        //  TODO (HW10): Implement.
    }

    //  ===========================
    //  ||  Function generator:  ||
    //  ===========================

    //  Statements:
    //  -----------

    std::string fn_generator::generate_stmt(const std::shared_ptr<ast_node::stmt_node>& statement) {
        switch (statement->type) {
            case ast_node::ASSERT_STMT:
                return this->generate_stmt_assert(std::reinterpret_pointer_cast<ast_node::assert_stmt_node>(statement));
            case ast_node::LET_STMT:
                return this->generate_stmt_let(std::reinterpret_pointer_cast<ast_node::let_stmt_node>(statement));
            case ast_node::RETURN_STMT:
                return this->generate_stmt_return(std::reinterpret_pointer_cast<ast_node::return_stmt_node>(statement));
            default:
                throw std::runtime_error("`generate_stmt_let`: invalid statement: \"" + statement->s_expression()
                                         + "\"");
        }
    }

    std::string fn_generator::generate_stmt_assert(const std::shared_ptr<ast_node::assert_stmt_node>& statement) {
        std::stringstream assembly;

        const std::string next_jump = (*this->constants).next_jump();

        assembly << generate_expr(statement->expr) << "\tpop rax\n";
        this->stack.pop();
        assembly << "\tcmp rax, 0\n"
                 << "\tjne " << next_jump << "\n"
                 << "\tlea rdi, [rel const" << (*this->constants)[statement->text] << "]";
        if (this->debug) assembly << " ; " << statement->text;
        assembly << "\n"
                 << "\tcall _fail_assertion\n"
                 << next_jump << ":\n";

        return assembly.str();
    }

    std::string fn_generator::generate_stmt_let(const std::shared_ptr<ast_node::let_stmt_node>&) {
        //  TODO (HW10): Implement.

        return {};
    }

    std::string fn_generator::generate_stmt_return(const std::shared_ptr<ast_node::return_stmt_node>&) {
        //  TODO (HW10): Implement.

        return {};
    }

    fn_generator::fn_generator(
            const std::shared_ptr<const_table>& constants,
            const std::shared_ptr<std::unordered_map<std::string, call_signature::call_signature>>& function_signatures,
            const std::shared_ptr<variable_table>& parent_variable_table, bool debug)
        : generator(constants, function_signatures, parent_variable_table, debug) {
        //  TODO (HW10): Implement.
    }

    std::string fn_generator::assem() const {
        //  TODO (HW10): Implement.

        return {};
    }

    //  =======================
    //  ||  Main generator:  ||
    //  =======================

    //  Commands:
    //  ---------

    void main_generator::generate_cmd(const std::shared_ptr<ast_node::cmd_node>& command) {
        switch (command->type) {
            case ast_node::ASSERT_CMD:
                //  TODO (HW10): Implement.
                throw std::runtime_error("`generate_cmd`: unhandled command: \"" + command->s_expression() + "\"");
            case ast_node::FN_CMD:
                return this->generate_cmd_fn(std::reinterpret_pointer_cast<ast_node::fn_cmd_node>(command));
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

    void main_generator::generate_cmd_fn(const std::shared_ptr<ast_node::fn_cmd_node>&) {
        /*
        //  TODO (HW10): Implement.
        std::stringstream func_assembly;

        func_assembly << command->name << ":\n_" << command->name << ":\n";

        const std::vector<std::shared_ptr<resolved_type::resolved_type>> arg_types;
        for (const std::shared_ptr<ast_node::binding_node>& binding : command->bindings) {
            //  TODO (HW10): Get the correct signature arguments.
        }
        //  TODO (HW10): Get the correct return type.
        const std::shared_ptr<resolved_type::resolved_type> return_type;

        (*this->function_signatures)[command->name] = call_signature::call_signature(arg_types, return_type);

        //  1. Start with the preamble:
        func_assembly << "\tpush rbp\n"
                      << "\tmov rbp, rsp\n";

        //  Save the size of the stack so that we can restore it at the end of the function.
        const unsigned int start_stack_size = this->stack.size();

        //  2. If the function returns a struct, push the address for the return value onto the stack.
        //  TODO (HW10): Implement.

        //  3. Process each argument.
        //  TODO (HW10): Implement.

        //  4. Generate each statement.
        for (const std::shared_ptr<ast_node::stmt_node>& statement : command->statements) {
            func_assembly << this->generate_stmt(statement);
        }

        //  5. Free the local variables from the stack.
        if (this->stack.size() > start_stack_size) {
            func_assembly << "\tadd rsp, " << this->stack.size() - start_stack_size;
            if (this->debug) func_assembly << " ; Free local variables";
            func_assembly << "\n";

            while (this->stack.size() > start_stack_size) { this->stack.pop(); }
        }

        //  6. Add the postamble.
        func_assembly << "\tpop rbp\n"
                      << "\tret\n";

        this->function_assemblies << func_assembly.str() << "\n";
         */
    }

    void main_generator::generate_cmd_let(const std::shared_ptr<ast_node::let_cmd_node>& command) {
        if (this->debug) this->main_assembly << "\t;  START generate_cmd_let\n";

        this->main_assembly << generate_expr(command->expr);
        this->stack.pop();

        const std::function<void(const std::shared_ptr<ast_node::lvalue_node>&,
                                 const std::shared_ptr<resolved_type::resolved_type>&)>
                bind_lvalue = [this, &bind_lvalue](const std::shared_ptr<ast_node::lvalue_node>& lvalue,
                                                   const std::shared_ptr<resolved_type::resolved_type>& r_type) {
                    if (lvalue->type == ast_node::ARGUMENT_LVALUE) {
                        const std::shared_ptr<ast_node::argument_node> argument
                                = std::reinterpret_pointer_cast<ast_node::argument_lvalue_node>(lvalue)->argument;

                        this->stack.push(r_type->size());
                        if (argument->type == ast_node::ARRAY_ARGUMENT) {
                            const std::shared_ptr<ast_node::array_argument_node> array_argument
                                    = std::reinterpret_pointer_cast<ast_node::array_argument_node>(argument);

                            this->variables.set_variable_address(array_argument->name, this->stack.size());

                            //  TODO (HW10): Fix this so that it works with multi-dimensional arrays.
                            for (const token::token& size_argument : array_argument->dimension_vars) {
                                this->variables.set_variable_address(size_argument.text, this->stack.size());
                            }
                        } else {
                            const std::shared_ptr<ast_node::variable_argument_node> variable_argument
                                    = std::reinterpret_pointer_cast<ast_node::variable_argument_node>(argument);

                            this->variables.set_variable_address(variable_argument->name, this->stack.size());
                        }
                    } else {
                        const std::shared_ptr<ast_node::tuple_lvalue_node> tuple_lvalue
                                = std::reinterpret_pointer_cast<ast_node::tuple_lvalue_node>(lvalue);
                        const std::shared_ptr<resolved_type::tuple_resolved_type> tuple_r_type
                                = std::reinterpret_pointer_cast<resolved_type::tuple_resolved_type>(r_type);

                        for (int index = (int)tuple_lvalue->lvalues.size() - 1; index >= 0; index--) {
                            bind_lvalue(tuple_lvalue->lvalues[index], tuple_r_type->element_types[index]);
                        }
                    }
                };

        bind_lvalue(command->lvalue, command->expr->r_type);

        if (this->debug) this->main_assembly << "\t;  END generate_cmd_let\n";
    }

    void main_generator::generate_cmd_show(const std::shared_ptr<ast_node::show_cmd_node>& command) {
        if (this->debug) this->main_assembly << "\t;  START generate_cmd_show\n";

        //  We need to know the size of the expression on the stack in order to align the stack,
        //  so we push it onto the stack and then pop it again before generating the expression.
        this->stack.push(command->expr->r_type->size());
        const bool needs_alignment = this->stack.needs_alignment();
        this->stack.pop();
        if (needs_alignment) {
            this->main_assembly << "\tsub rsp, 8";
            if (this->debug) this->main_assembly << " ; Align stack";
            this->main_assembly << "\n";
            this->stack.push();
        }

        this->main_assembly << this->generate_expr(command->expr) << "\tlea rdi, [rel "
                            << (*this->constants)[command->expr->r_type->s_expression()] << "]";
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
            if (this->debug) this->main_assembly << " ; Remove alignment";
            this->main_assembly << "\n";
            this->stack.pop();
        }

        if (this->debug) this->main_assembly << "\t;  END generate_cmd_show\n";
    }

    void main_generator::generate_linking_preface() {
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

    void main_generator::generate_commands() {
        constexpr unsigned int reg_size = 8;

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

        if (this->stack.size() > reg_size) {
            this->main_assembly << "\tadd rsp, " << this->stack.size() - reg_size;
            if (this->debug) this->main_assembly << " ; Remove local variables";
            this->main_assembly << "\n";
        }

        this->main_assembly << "\tpop r12\n";
        this->stack.pop();
        this->main_assembly << "\tpop rbp\n"
                            << "\tret\n";
    }

    main_generator::main_generator(const std::vector<std::shared_ptr<ast_node::ast_node>>& nodes, bool debug)
        : generator(std::make_shared<const_table>(),
                    std::make_shared<std::unordered_map<std::string, call_signature::call_signature>>(), nullptr,
                    debug),
          nodes(nodes) {
        const std::shared_ptr<resolved_type::resolved_type> int_type = std::make_shared<resolved_type::resolved_type>(
                resolved_type::INT_TYPE);
        const std::shared_ptr<resolved_type::resolved_type> float_type = std::make_shared<resolved_type::resolved_type>(
                resolved_type::FLOAT_TYPE);

        for (const std::string function_name : {"sqrt", "exp", "sin", "cos", "tan", "asin", "acos", "atan", "log"}) {
            (*this->function_signatures)[function_name] = call_signature::call_signature({float_type}, float_type);
        }

        for (const std::string function_name : {"pow", "atan2"}) {
            (*this->function_signatures)[function_name] = call_signature::call_signature({float_type, float_type},
                                                                                         float_type);
        }

        (*this->function_signatures)["to_float"] = call_signature::call_signature({int_type}, float_type);
        (*this->function_signatures)["to_int"] = call_signature::call_signature({float_type}, int_type);

        this->generate_linking_preface();
        this->generate_commands();
    }

    std::string main_generator::assem() const {
        std::stringstream assembly;

        assembly << this->linking_preface_assembly.str() << "\n"
                 << this->constants->assem() << "\n"
                 << "section .text\n";

        for (const std::string& function_assem : this->function_assemblies) { assembly << function_assem << "\n"; }

        assembly << this->main_assembly.str() << "\n";

        return assembly.str();
    }

    //  ================
    //  ||  Methods:  ||
    //  ================

    std::string generate(const std::vector<std::shared_ptr<ast_node::ast_node>>& nodes, bool debug) {
        return main_generator(nodes, debug).assem();
    }
}  //  namespace generator
