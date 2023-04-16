/**
 * @file generator.cpp
 * @package Assignments 9-10
 * @author Cayden Lund (u1182408)
 * @brief Implements the assembly `generator` class, and associated subtypes.
 * @details See the header file for documentation.
 *
 */

//  TODO (HW10): Remove the extraneous debugging output.
//               See output including "<<(START|END) #: #>>", "[[#]]".

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
            const unsigned long result_size = std::snprintf(buf.get(), buf_size, format.c_str(), this->float_value);
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

    std::tuple<std::string, long> generator::variable_table::get_variable_address(const std::string& variable,
                                                                                  bool from_child) const {
        const std::string reg = (this->parent == nullptr && from_child) ? "r12" : "rbp";
        if (this->variables.count(variable) > 0) { return {reg, this->variables.at(variable)}; }

        return this->parent->get_variable_address(variable, true);
    }

    void generator::variable_table::set_variable_address(const std::string& variable, long offset) {
        this->variables[variable] = offset;
    }

    //  =======================
    //  ||  Base generator:  ||
    //  =======================

    //  Expressions:
    //  ------------

    std::string generator::generate_assem_mul(const std::string& reg, long value) const {
        std::stringstream assembly;

        const long log = generator::log_2(value);
        const bool power_of_two = this->opt_level >= 1 && log >= 0;

        if (power_of_two) {
            assembly << "\tshl " << reg << ", " << log << "\n";
        } else {
            assembly << "\timul " << reg << ", " << value << "\n";
        }

        return assembly.str();
    }

    std::string generator::generate_expr(const std::shared_ptr<ast_node::expr_node>& expression) {
        switch (expression->type) {
            case ast_node::ARRAY_INDEX_EXPR:
                return this->generate_expr_array_index(
                        std::reinterpret_pointer_cast<ast_node::array_index_expr_node>(expression));
            case ast_node::ARRAY_LITERAL_EXPR:
                return this->generate_expr_array_literal(
                        std::reinterpret_pointer_cast<ast_node::array_literal_expr_node>(expression));
            case ast_node::ARRAY_LOOP_EXPR:
                return this->generate_expr_array_loop(
                        std::reinterpret_pointer_cast<ast_node::array_loop_expr_node>(expression));
            case ast_node::BINOP_EXPR:
                return this->generate_expr_binop(std::reinterpret_pointer_cast<ast_node::binop_expr_node>(expression));
            case ast_node::CALL_EXPR:
                return this->generate_expr_call(std::reinterpret_pointer_cast<ast_node::call_expr_node>(expression));
            case ast_node::FALSE_EXPR:
                return this->generate_expr_false(std::reinterpret_pointer_cast<ast_node::false_expr_node>(expression));
            case ast_node::FLOAT_EXPR:
                return this->generate_expr_float(std::reinterpret_pointer_cast<ast_node::float_expr_node>(expression));
            case ast_node::IF_EXPR:
                return this->generate_expr_if(std::reinterpret_pointer_cast<ast_node::if_expr_node>(expression));
            case ast_node::INTEGER_EXPR:
                return this->generate_expr_integer(
                        std::reinterpret_pointer_cast<ast_node::integer_expr_node>(expression));
            case ast_node::SUM_LOOP_EXPR:
                return this->generate_expr_sum_loop(
                        std::reinterpret_pointer_cast<ast_node::sum_loop_expr_node>(expression));
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
    generator::generate_expr_array_index(const std::shared_ptr<ast_node::array_index_expr_node>& expression) {
        std::stringstream assembly;

        if (this->debug) assembly << "\t;  START generate_expr_array_index\n";

        constexpr long reg_size = 8;
        const long rank = (long)expression->params.size();

        const long array_offset = reg_size * rank;

        const bool is_variable = this->opt_level >= 1 && expression->array->type == ast_node::node_type::VARIABLE_EXPR;
        bool is_local_variable = false;
        long gap = 0;
        if (is_variable) {
            const std::shared_ptr<ast_node::variable_expr_node> var_expr
                    = std::reinterpret_pointer_cast<ast_node::variable_expr_node>(expression->array);
            const std::tuple<std::string, long> variable_address = this->variables.get_variable_address(var_expr->name);
            is_local_variable = std::get<0>(variable_address) == "rbp";
            gap = this->stack.size() - std::get<1>(variable_address);
        }

        if (!is_local_variable) assembly << this->generate_expr(expression->array);

        //  Put each index expression onto the stack.
        for (long index = rank - 1; index >= 0; --index) { assembly << generate_expr(expression->params[index]); }

        for (long offset = 0; offset < array_offset; offset += reg_size) {
            const std::string jump_1 = this->constants->next_jump();
            const std::string jump_2 = this->constants->next_jump();

            assembly << "\tmov rax, [rsp + " << offset << "]\n"
                     << "\tcmp rax, 0\n"
                     << "\tjge " << jump_1 << "\n";

            const bool needs_alignment = this->stack.needs_alignment();
            if (needs_alignment) {
                assembly << "\tsub rsp, 8";
                if (this->debug) assembly << " ; Align stack";
                assembly << "\n";
                this->stack.push();
            }

            assembly << "\tlea rdi, [rel " << (*this->constants)[{"negative array index"}] << "]";
            if (this->debug) assembly << " ; negative array index";
            assembly << "\n";

            assembly << "\tcall _fail_assertion\n";

            if (needs_alignment) {
                assembly << "\tadd rsp, 8";
                if (this->debug) assembly << " ; Remove alignment";
                assembly << "\n";
                this->stack.pop();
            }

            assembly << jump_1 << ":\n"
                     << "\tcmp rax, [rsp + " << array_offset + offset + gap << "]\n"
                     << "\tjl " << jump_2 << "\n";

            if (needs_alignment) {
                assembly << "\tsub rsp, 8";
                if (this->debug) assembly << " ; Align stack";
                assembly << "\n";
                this->stack.push();
            }

            assembly << "\tlea rdi, [rel " << (*this->constants)[{"index too large"}] << "]";
            if (this->debug) assembly << " ; index too large";
            assembly << "\n";

            assembly << "\tcall _fail_assertion\n";

            if (needs_alignment) {
                assembly << "\tadd rsp, 8";
                if (this->debug) assembly << " ; Remove alignment";
                assembly << "\n";
                this->stack.pop();
            }

            assembly << jump_2 << ":\n";
        }

        if (this->opt_level >= 1) {
            assembly << "\tmov rax, [rsp + 0]\n";
        } else {
            assembly << "\tmov rax, 0\n"
                     << "\timul rax, [rsp + " << array_offset + gap << "]\n"
                     << "\tadd rax, [rsp + 0]\n";
        }

        for (long index = 1; index < rank; ++index) {
            const long offset = index * reg_size;
            const bool is_constant = this->opt_level >= 2 && (long)expression->array->cp_val.array_value.size() > index
                                  && expression->array->cp_val.array_value[index].type == ast_node::INT_VALUE;
            if (this->debug)
                assembly << "\t;  [[Opt level: " << this->opt_level << "]]\n"
                         << "\t;  [[Array CP value size: " << expression->array->cp_val.array_value.size() << "]]\n"
                         << "\t;  [[CP value type: " << expression->array->cp_val.type << "]]\n";
            if (is_constant) {
                assembly << this->generate_assem_mul("rax", expression->array->cp_val.array_value[index].int_value);
            } else {
                assembly << "\timul rax, [rsp + " << array_offset + offset + gap << "]\n";
            }
            assembly << "\tadd rax, [rsp + " << offset << "]\n";
        }

        assembly << this->generate_assem_mul("rax", expression->r_type->size());

        assembly << "\tadd rax, [rsp + " << array_offset * 2 + gap << "]\n";

        if (this->debug) assembly << "\t;  Remove index variables\n";
        long total = 0;
        for (long offset = 0; offset < rank; ++offset) {
            const long val = this->stack.pop();
            if (this->opt_level >= 1) {
                total += val;
            } else {
                assembly << "\tadd rsp, " << val << "\n";
            }
        }
        if (this->opt_level >= 1) { assembly << "\tadd rsp, " << total << "\n"; }

        if (!is_local_variable) {
            assembly << "\tadd rsp, " << array_offset + reg_size;
            if (this->debug) assembly << " ; Remove array";
            assembly << "\n";
            this->stack.pop();
        }

        const long return_size = (long)expression->r_type->size();
        assembly << "\tsub rsp, " << return_size << "\n";
        this->stack.push(return_size);

        if (this->debug) assembly << "\t;  Moving " << return_size << " bytes from [rax] to [rsp]\n";
        for (long offset = return_size - reg_size; offset >= 0; offset -= reg_size) {
            if (this->debug) assembly << "\t";  //  Extra indentation for debug mode.
            assembly << "\tmov r10, [rax + " << offset << "]\n";

            if (this->debug) assembly << "\t";  //  Extra indentation for debug mode.
            assembly << "\tmov [rsp + " << offset << "], r10\n";
        }

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

        for (long offset = reg_size; offset <= (long)total_size; offset += reg_size) {
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

    std::string generator::generate_expr_array_loop(const std::shared_ptr<ast_node::array_loop_expr_node>& expression) {
        constexpr long reg_size = 8;
        std::stringstream assembly;

        if (this->debug)
            assembly << "\t;  START generate_expr_array_loop\n"
                     << "\t;  Allocating 8 bytes for the array pointer\n";

        assembly << "\tsub rsp, 8\n";
        this->stack.push();

        const std::shared_ptr<resolved_type::array_resolved_type> array_r_type
                = std::reinterpret_pointer_cast<resolved_type::array_resolved_type>(expression->r_type);
        const long item_size = (long)array_r_type->element_type->size();
        const long rank = (long)expression->binding_pairs.size();

        for (long index = rank - 1; index >= 0; --index) {
            const std::tuple<token::token, std::shared_ptr<ast_node::expr_node>>& pair
                    = expression->binding_pairs[index];
            if (this->debug) assembly << "\t;  Computing bound for " << std::get<0>(pair).text << "\n";

            assembly << this->generate_expr(std::get<1>(pair));

            const std::string jump = this->constants->next_jump();

            assembly << "\tmov rax, [rsp]\n"
                     << "\tcmp rax, 0\n"
                     << "\tjg " << jump << "\n";

            const bool needs_alignment = this->stack.needs_alignment();
            if (needs_alignment) {
                assembly << "\tsub rsp, 8";
                if (this->debug) assembly << " ; Align stack";
                assembly << "\n";
                this->stack.push();
            }

            assembly << "\tlea rdi, [rel " << (*this->constants)[{"non-positive loop bound"}] << "]";
            if (this->debug) assembly << " ; non-positive loop bound";
            assembly << "\n"
                     << "\tcall _fail_assertion\n";

            if (needs_alignment) {
                assembly << "\tadd rsp, 8";
                if (this->debug) assembly << " ; Remove alignment";
                assembly << "\n";
                this->stack.pop();
            }

            assembly << jump << ":\n";
        }

        if (this->debug) assembly << "\t;  Computing total heap size to allocate\n";

        assembly << "\tmov rdi, " << item_size;
        if (this->debug) assembly << " ; Size of one element";
        assembly << "\n";

        for (long offset = 0; offset < reg_size * rank; offset += reg_size) {
            assembly << "\timul rdi, [rsp + " << offset << "]";
            if (this->debug) assembly << "; Multiply by one dimension";
            assembly << "\n";

            const std::string next_jump = this->constants->next_jump();
            assembly << "\tjno " << next_jump << "\n";

            const bool needs_alignment = this->stack.needs_alignment();
            if (needs_alignment) {
                assembly << "\tsub rsp, 8";
                if (this->debug) assembly << " ; Align stack";
                assembly << "\n";
                this->stack.push();
            }

            assembly << "\tlea rdi, [rel " << (*this->constants)[{"overflow computing array size"}] << "]";
            if (this->debug) assembly << " ; overflow computing array size";
            assembly << "\n"
                     << "\tcall _fail_assertion\n";

            if (needs_alignment) {
                assembly << "\tadd rsp, 8";
                if (this->debug) assembly << " ; Remove alignment";
                assembly << "\n";
                this->stack.pop();
            }

            assembly << next_jump << ":\n";
        }

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

        assembly << "\tmov [rsp + " << reg_size * rank << "], rax";
        if (this->debug) assembly << " ; Move array pointer to previously-allocated space";
        assembly << "\n";

        for (long index = rank - 1; index >= 0; --index) {
            const std::string& name = std::get<0>(expression->binding_pairs[index]).text;
            assembly << "\tmov rax, 0";
            if (this->debug) assembly << " ; Initialize " << name << " to 0";
            assembly << "\n"
                     << "\tpush rax\n";
            this->stack.push();
            this->variables.set_variable_address(name, (long)this->stack.size());
        }

        const std::string body_start = this->constants->next_jump();

        assembly << body_start << ":";
        if (this->debug) assembly << " ; Begin loop body";
        assembly << "\n";

        assembly << this->generate_expr(expression->item_expr);

        if (this->debug) assembly << "\t;  Calculate the index to store the result\n";

        if (this->opt_level >= 1) {
            assembly << "\tmov rax, [rsp + " << item_size << "]\n";
        } else {
            assembly << "\tmov rax, 0\n"
                     << "\timul rax, [rsp + " << rank * reg_size + item_size << "]\n"
                     << "\tadd rax, [rsp + " << item_size << "]\n";
        }

        for (long index = 1; index < rank; ++index) {
            const long offset = reg_size * index + item_size;
            const std::shared_ptr<ast_node::expr_node>& binding = std::get<1>(expression->binding_pairs[index]);
            const bool is_literal = this->opt_level >= 1 && binding->type == ast_node::node_type::INTEGER_EXPR;
            const bool is_const = this->opt_level >= 2 && binding->cp_val.type == ast_node::INT_VALUE;
            if (is_literal) {
                assembly << this->generate_assem_mul(
                        "rax", std::reinterpret_pointer_cast<ast_node::integer_expr_node>(binding)->value);
            } else if (is_const) {
                assembly << this->generate_assem_mul("rax", binding->cp_val.int_value);
            } else {
                assembly << "\timul rax, [rsp + " << rank * reg_size + offset << "]\n";
            }
            assembly << "\tadd rax, [rsp + " << offset << "]\n";
        }

        assembly << this->generate_assem_mul("rax", item_size) << "\tadd rax, [rsp + "
                 << 2 * rank * reg_size + item_size << "]\n";

        if (this->debug) assembly << "\t;  Moving " << item_size << "-byte body from [rsp] to [rax]\n";
        for (long offset = item_size - reg_size; offset >= 0; offset -= reg_size) {
            if (this->debug) assembly << "\t";
            assembly << "\tmov r10, [rsp + " << offset << "]\n";

            if (this->debug) assembly << "\t";
            assembly << "\tmov [rax + " << offset << "], r10\n";
        }

        assembly << "\tadd rsp, " << item_size << "\n";
        this->stack.pop();

        for (long index = rank - 1; index >= 0; --index) {
            const std::string& name = std::get<0>(expression->binding_pairs[index]).text;

            if (this->debug) assembly << "\t;  Increment " << name << "\n";
            assembly << "\tadd qword [rsp + " << reg_size * index << "], 1\n";

            if (this->debug) assembly << "\t;  Compare " << name << " to its bound\n";
            assembly << "\tmov rax, [rsp + " << reg_size * index << "]\n"
                     << "\tcmp rax, [rsp + " << reg_size * (index + rank) << "]\n"
                     << "\tjl " << body_start;
            if (this->debug) assembly << " ; If " << name << " < bound, run next iteration";
            assembly << "\n";

            if (index > 0) {
                assembly << "\tmov qword [rsp + " << index * reg_size << "], 0";
                if (this->debug) assembly << " ; Set " << name << " = 0";
                assembly << "\n";
            }
        }

        assembly << "\tadd rsp, " << reg_size * rank;
        if (this->debug) assembly << " ; Free all loop variables";
        assembly << "\n";
        for (long index = 0; index < 2 * rank + 1; ++index) { this->stack.pop(); }
        this->stack.push(reg_size * (rank + 1));

        if (this->debug) assembly << "\t;  END generate_expr_array_loop\n";

        return assembly.str();
    }

    std::string generator::generate_expr_binop(const std::shared_ptr<ast_node::binop_expr_node>& expression) {
        std::stringstream assembly;

        if (this->debug) assembly << "\t;  START generate_expr_binop\n";

        const resolved_type::resolved_type_type operand_type = expression->left_operand->r_type->type;

        if (expression->operator_type == ast_node::op_type::BINOP_AND
            || expression->operator_type == ast_node::op_type::BINOP_OR) {
            assembly << this->generate_expr(expression->left_operand);

            const std::string short_circuit_jump = this->constants->next_jump();
            assembly << "\tpop rax\n";
            this->stack.pop();
            assembly << "\tcmp rax, 0\n"
                     << "\t" << (expression->operator_type == ast_node::op_type::BINOP_AND ? "je" : "jne") << " "
                     << short_circuit_jump << "\n";

            assembly << this->generate_expr(expression->right_operand) << "\tpop rax\n"
                     << short_circuit_jump << ":\n"
                     << "\tpush rax\n";

            if (this->debug) assembly << "\t;  END generate_expr_binop\n";

            return assembly.str();
        }

        if (this->opt_level >= 1 && operand_type == resolved_type::resolved_type_type::INT_TYPE
            && expression->operator_type == ast_node::op_type::BINOP_TIMES) {
            const bool left_is_literal = expression->left_operand->type == ast_node::node_type::INTEGER_EXPR;
            const bool left_has_literal_val = expression->left_operand->cp_val.type == ast_node::INT_VALUE;
            long left_value = 0;
            if (left_is_literal) {
                left_value
                        = std::reinterpret_pointer_cast<ast_node::integer_expr_node>(expression->left_operand)->value;
            } else if (left_has_literal_val) {
                left_value = expression->left_operand->cp_val.int_value;
            }
            const long left_log = generator::log_2(left_value);

            const bool right_is_literal = expression->right_operand->type == ast_node::node_type::INTEGER_EXPR;
            const bool right_has_literal_val = expression->right_operand->cp_val.type == ast_node::INT_VALUE;
            long right_value = 0;
            if (right_is_literal) {
                right_value
                        = std::reinterpret_pointer_cast<ast_node::integer_expr_node>(expression->right_operand)->value;
            } else if (right_has_literal_val) {
                right_value = expression->right_operand->cp_val.int_value;
            }
            const long right_log = generator::log_2(right_value);

            if ((left_is_literal || left_has_literal_val) && left_value == 1) {
                assembly << this->generate_expr(expression->right_operand);

                if (this->debug) assembly << "\t;  END generate_expr_binop\n";

                return assembly.str();
            }

            if ((right_is_literal || right_has_literal_val) && right_value == 1) {
                assembly << this->generate_expr(expression->left_operand);

                if (this->debug) assembly << "\t;  END generate_expr_binop\n";

                return assembly.str();
            }

            if ((left_is_literal || left_has_literal_val) && left_log > 0) {
                assembly << this->generate_expr(expression->right_operand) << "\tpop rax\n"
                         << "\tshl rax, " << left_log << "\n"
                         << "\tpush rax\n";

                if (this->debug) assembly << "\t;  END generate_expr_binop\n";

                return assembly.str();
            }

            if ((right_is_literal || right_has_literal_val) && right_log > 0) {
                assembly << this->generate_expr(expression->left_operand) << "\tpop rax\n"
                         << "\tshl rax, " << right_log << "\n"
                         << "\tpush rax\n";

                if (this->debug) assembly << "\t;  END generate_expr_binop\n";

                return assembly.str();
            }
        }

        assembly << this->generate_expr(expression->right_operand) << this->generate_expr(expression->left_operand);

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
                default:
                    throw std::runtime_error("`generate_expr_binop`: invalid expression: \""
                                             + expression->s_expression() + "\"");
            }
        }

        if (this->debug) assembly << "\t;  END generate_expr_binop\n";

        return assembly.str();
    }

    std::string generator::generate_expr_call(const std::shared_ptr<ast_node::call_expr_node>& expression) {
        constexpr unsigned int reg_size = 8;
        std::stringstream assembly;

        if (this->debug) assembly << "\t;  START generate_expr_call\n";

        const call_signature::call_signature& function = (*this->function_signatures)[expression->name];

        if (this->debug) assembly << "\t;  <<START 1: " << this->stack.size() << ">>\n";

        //  1.  If there's a struct return value, allocate space for it on the stack.
        const bool has_struct_return = function.ret_type->type == resolved_type::ARRAY_TYPE
                                    || (function.ret_type->type == resolved_type::TUPLE_TYPE
                                        && !std::reinterpret_pointer_cast<resolved_type::tuple_resolved_type>(
                                                    function.ret_type)
                                                    ->element_types.empty());
        if (has_struct_return) {
            assembly << "\tsub rsp, " << function.ret_type->size();
            if (this->debug) assembly << " ; Return struct";
            assembly << "\n";
            this->stack.push(function.ret_type->size());
        }

        if (this->debug) assembly << "\t;  <<END 1: " << this->stack.size() << ">>\n";

        if (this->debug) assembly << "\t;  <<START 2: " << this->stack.size() << ">>\n";

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

        if (this->debug) assembly << "\t;  <<END 2: " << this->stack.size() << ">>\n";

        if (this->debug) assembly << "\t;  <<START 3/4: " << this->stack.size() << ">>\n";
        //  3.  Compute every stack argument in reverse order (so that they end up on the stack in the normal order).
        //  4.  Compute every register argument in reverse order.
        for (const unsigned int& index : function.push_order) {
            assembly << generate_expr(expression->call_args[index]);
        }

        if (this->debug) assembly << "\t;  <<END 3/4: " << this->stack.size() << ">>\n";

        if (this->debug) assembly << "\t;  <<START 5: " << this->stack.size() << ">>\n";

        //  5.  Pop all register arguments into their correct registers.
        for (const std::string& assem : function.pop_assem) {
            assembly << assem;
            this->stack.pop();
        }

        if (this->debug) assembly << "\t;  <<END 5: " << this->stack.size() << ">>\n";

        if (this->debug) assembly << "\t;  <<START 6: " << this->stack.size() << ">>\n";

        //  6.  If there's a struct return value, load its address into RDI.
        if (has_struct_return) {
            assembly << "\tlea rdi, [rsp + " << function.bytes_on_stack + (needs_alignment ? reg_size : 0) << "]\n";
        }

        if (this->debug) assembly << "\t;  <<END 6: " << this->stack.size() << ">>\n";

        if (this->debug) assembly << "\t;  <<START 7: " << this->stack.size() << ">>\n";

        //  7.  Execute the `call` instruction.
        assembly << "\tcall _" << expression->name << "\n";

        if (this->debug) assembly << "\t;  <<END 7: " << this->stack.size() << ">>\n";

        if (this->debug) assembly << "\t;  <<START 8: " << this->stack.size() << ">>\n";

        //  8.  Drop every stack argument.
        for (const long arg : function.stack_args) {
            assembly << "\tadd rsp, " << arg << "\n";
            this->stack.pop();
        }

        if (this->debug) assembly << "\t;  <<END 8: " << this->stack.size() << ">>\n";

        if (this->debug) assembly << "\t;  <<START 9: " << this->stack.size() << ">>\n";

        //  9.  Drop the padding, if any.
        if (needs_alignment) {
            assembly << "\tadd rsp, 8";
            if (this->debug) assembly << " ; Remove alignment";
            assembly << "\n";

            while (this->stack.pop() == 0) {}
        }

        if (this->debug) assembly << "\t;  <<END 9: " << this->stack.size() << ">>\n";

        if (this->debug) assembly << "\t;  <<START 10: " << this->stack.size() << ">>\n";

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
                break;
            default:
                if (!has_struct_return) this->stack.push(0);
                break;
        }


        if (this->debug) assembly << "\t;  <<END 10: " << this->stack.size() << ">>\n";

        if (this->debug) assembly << "\t;  END generate_expr_call\n";

        return assembly.str();
    }

    std::string generator::generate_expr_false(const std::shared_ptr<ast_node::false_expr_node>&) {
        std::stringstream assembly;

        if (this->debug) assembly << "\t;  START generate_expr_false\n";

        if (this->opt_level >= 1) {
            assembly << "\tpush qword 0";
            if (this->debug) assembly << " ; False";
            assembly << "\n";
            this->stack.push();
        } else {
            assembly << "\tmov rax, [rel " << (*this->constants)[(long)0] << "]";
            if (this->debug) assembly << " ; False";
            assembly << "\n"
                     << "\tpush rax\n";
            this->stack.push();
        }

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

    std::string generator::generate_expr_if(const std::shared_ptr<ast_node::if_expr_node>& expression) {
        std::stringstream assembly;

        if (this->debug) assembly << "\t;  START generate_expr_if\n";

        assembly << this->generate_expr(expression->conditional_expr);

        std::string jump_1;
        std::string jump_2;

        const bool is_bool_cast
                = (this->opt_level >= 1)
               && ((expression->affirmative_expr->cp_val.type == ast_node::INT_VALUE
                    && expression->negative_expr->cp_val.type == ast_node::INT_VALUE
                    && expression->affirmative_expr->cp_val.int_value == 1
                    && expression->negative_expr->cp_val.int_value == 0)
                   || (expression->affirmative_expr->type == ast_node::node_type::INTEGER_EXPR
                       && expression->negative_expr->type == ast_node::node_type::INTEGER_EXPR
                       && std::reinterpret_pointer_cast<ast_node::integer_expr_node>(expression->affirmative_expr)
                                          ->value
                                  == 1
                       && std::reinterpret_pointer_cast<ast_node::integer_expr_node>(expression->negative_expr)->value
                                  == 0));

        if (is_bool_cast) {
            if (this->debug) assembly << "\t;  O1: Boolean cast\n";
        } else {
            assembly << "\tpop rax\n";
            this->stack.pop();
            assembly << "\tcmp rax, 0\n";

            jump_1 = this->constants->next_jump();
            jump_2 = this->constants->next_jump();

            assembly << "\tje " << jump_1 << "\n";

            assembly << this->generate_expr(expression->affirmative_expr);
            this->stack.pop();

            assembly << "\tjmp " << jump_2 << "\n";

            assembly << jump_1 << ":\n";
            assembly << this->generate_expr(expression->negative_expr);

            assembly << jump_2 << ":\n";
        }

        if (this->debug) assembly << "\t;  END generate_expr_if\n";

        return assembly.str();
    }

    std::string generator::generate_expr_integer(const std::shared_ptr<ast_node::integer_expr_node>& expression) {
        std::stringstream assembly;

        if (this->debug) assembly << "\t;  START generate_expr_integer\n";

        const bool push_literal = (this->opt_level >= 1) && generator::fits_into_32(expression->value);

        if (push_literal) {
            assembly << "\tpush qword " << expression->value << "\n";
            this->stack.push();
        } else {
            assembly << "\tmov rax, [rel " << (*this->constants)[expression->value] << "]";
            if (this->debug) assembly << " ; " << expression->value;
            assembly << "\n"
                     << "\tpush rax\n";
            this->stack.push();
        }

        if (this->debug) assembly << "\t;  END generate_expr_integer\n";

        return assembly.str();
    }

    std::string generator::generate_expr_sum_loop(const std::shared_ptr<ast_node::sum_loop_expr_node>& expression) {
        constexpr long reg_size = 8;
        std::stringstream assembly;

        if (this->debug) assembly << "\t;  START generate_expr_sum_loop\n";

        const bool is_int = expression->r_type->type == resolved_type::INT_TYPE;

        if (this->debug) assembly << "\t;  Allocating 8 bytes for the sum\n";

        assembly << "\tsub rsp, 8\n";
        this->stack.push();

        const long rank = (long)expression->binding_pairs.size();

        for (long index = rank - 1; index >= 0; --index) {
            const std::tuple<token::token, std::shared_ptr<ast_node::expr_node>>& pair
                    = expression->binding_pairs[index];
            if (this->debug) assembly << "\t;  Computing bound for " << std::get<0>(pair).text << "\n";

            assembly << this->generate_expr(std::get<1>(pair));

            const std::string jump = this->constants->next_jump();

            assembly << "\tmov rax, [rsp]\n"
                     << "\tcmp rax, 0\n"
                     << "\tjg " << jump << "\n";

            const bool needs_alignment = this->stack.needs_alignment();
            if (needs_alignment) {
                assembly << "\tsub rsp, 8";
                if (this->debug) assembly << " ; Align stack";
                assembly << "\n";
                this->stack.push();
            }

            assembly << "\tlea rdi, [rel " << (*this->constants)[{"non-positive loop bound"}] << "]";
            if (this->debug) assembly << " ; non-positive loop bound";
            assembly << "\n"
                     << "\tcall _fail_assertion\n";

            if (needs_alignment) {
                assembly << "\tadd rsp, 8";
                if (this->debug) assembly << " ; Remove alignment";
                assembly << "\n";
                this->stack.pop();
            }

            assembly << jump << ":\n";
        }

        if (this->debug) assembly << "\t;  Initializing sum to 0\n";

        assembly << "\tmov rax, 0\n"
                 << "\tmov [rsp + " << reg_size * rank << "], rax";
        if (this->debug) assembly << " ; Move to pre-allocated space";
        assembly << "\n";

        for (long index = rank - 1; index >= 0; --index) {
            const std::string& name = std::get<0>(expression->binding_pairs[index]).text;
            assembly << "\tmov rax, 0";
            if (this->debug) assembly << " ; Initialize " << name << " to 0";
            assembly << "\n"
                     << "\tpush rax\n";
            this->stack.push();
            this->variables.set_variable_address(name, (long)this->stack.size());
        }

        const std::string body_start = this->constants->next_jump();

        assembly << body_start << ":";
        if (this->debug) assembly << " ; Begin loop body";
        assembly << "\n";

        assembly << this->generate_expr(expression->sum_expr);

        if (is_int) {
            assembly << "\tpop rax\n";
            this->stack.pop();

            assembly << "\tadd [rsp + " << 2 * reg_size * rank << "], rax";
            if (this->debug) assembly << " ; Add loop body to sum";
            assembly << "\n";
        } else {
            assembly << "\tmovsd xmm0, [rsp]\n"
                     << "\tadd rsp, 8\n";
            this->stack.pop();

            const long offset = 2 * reg_size * rank;
            assembly << "\taddsd xmm0, [rsp + " << offset << "]";
            if (this->debug) assembly << " ; Load sum";
            assembly << "\n";

            assembly << "\tmovsd [rsp + " << offset << "], xmm0";
            if (this->debug) assembly << " ; Store sum";
            assembly << "\n";
        }

        for (long index = rank - 1; index >= 0; --index) {
            const std::string& name = std::get<0>(expression->binding_pairs[index]).text;

            if (this->debug) assembly << "\t;  Increment " << name << "\n";
            assembly << "\tadd qword [rsp + " << reg_size * index << "], 1\n";

            if (this->debug) assembly << "\t;  Compare " << name << " to its bound\n";
            assembly << "\tmov rax, [rsp + " << reg_size * index << "]\n"
                     << "\tcmp rax, [rsp + " << reg_size * (index + rank) << "]\n"
                     << "\tjl " << body_start;
            if (this->debug) assembly << " ; If " << name << " < bound, run next iteration";
            assembly << "\n";

            if (index > 0) {
                assembly << "\tmov qword [rsp + " << index * reg_size << "], 0";
                if (this->debug) assembly << " ; Set " << name << " = 0";
                assembly << "\n";
            }
        }

        assembly << "\tadd rsp, " << reg_size * rank;
        if (this->debug) assembly << " ; Free all loop variables";
        assembly << "\n";
        for (long index = 0; index < rank; ++index) { this->stack.pop(); }
        assembly << "\tadd rsp, " << reg_size * rank;
        if (this->debug) assembly << " ; Free all loop bounds";
        assembly << "\n";
        for (long index = 0; index < rank; ++index) { this->stack.pop(); }

        if (this->debug) assembly << "\t;  END generate_expr_sum_loop\n";

        return assembly.str();
    }

    std::string generator::generate_expr_true(const std::shared_ptr<ast_node::true_expr_node>&) {
        std::stringstream assembly;

        if (this->debug) assembly << "\t;  START generate_expr_true\n";

        if (this->opt_level >= 1) {
            assembly << "\tpush qword 1";
            if (this->debug) assembly << " ; True";
            assembly << "\n";
            this->stack.push();
        } else {
            assembly << "\tmov rax, [rel " << (*this->constants)[(long)1] << "]";
            if (this->debug) assembly << " ; True";
            assembly << "\n"
                     << "\tpush rax\n";
            this->stack.push();
        }

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
        const long element_offset = (long)tuple_type->offset(element_index);
        const long destination = (long)tuple_type->size() - element_size;  //  NOLINT(*-narrowing-conversions)

        if (this->debug)
            assembly << "\t;  Moving " << element_size << " bytes from rsp + " << element_offset << " to rsp + "
                     << destination << "\n";

        if (element_size > 0) {
            for (long mov_offset = (long)(element_size - reg_size); mov_offset >= 0; mov_offset -= reg_size) {
                if (this->debug) assembly << "\t";
                assembly << "\tmov r10, [rsp + " << element_offset << " + " << mov_offset << "]\n";

                if (this->debug) assembly << "\t";
                assembly << "\tmov [rsp + " << destination << " + " << mov_offset << "], r10\n";
            }

            this->stack.pop();
            this->stack.push(element_size);
        }

        assembly << "\tadd rsp, " << tuple_type->size() - element_size << "\n";

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

        const ast_node::cp_value& cp_val = expression->cp_val;
        if (cp_val.type == ast_node::INT_VALUE && generator::fits_into_32(cp_val.int_value)) {
            if (this->debug) assembly << "\t;  O2: pushing integer constant\n";

            this->stack.push();

            assembly << "\tpush qword " << cp_val.int_value << "\n";
        } else {
            const unsigned int size = expression->r_type->size();
            assembly << "\tsub rsp, " << size << "\n";
            this->stack.push(size);

            const std::tuple<std::string, long> variable_address = this->variables.get_variable_address(
                    expression->name);
            const std::string reg = std::get<0>(variable_address);
            const long reg_offset = std::get<1>(variable_address);

            if (debug)
                assembly << "\t;  Moving " << size << " bytes from [" << reg << " - " << reg_offset << "] to [rsp]\n";

            for (long offset = reg_size; offset <= (long)size; offset += reg_size) {
                //  Extra indentation for debug mode.
                if (this->debug) assembly << "\t";
                assembly << "\tmov r10, [" << reg << " - " << reg_offset - size + offset << "]\n";

                if (this->debug) assembly << "\t";
                assembly << "\tmov [rsp + " << size - offset << "], r10\n";
            }
        }

        if (this->debug) assembly << "\t;  END generate_expr_variable\n";

        return assembly.str();
    }

    void generator::bind_argument(const std::shared_ptr<ast_node::argument_node>& argument,
                                  const std::shared_ptr<resolved_type::resolved_type>& r_type) {
        this->stack.push(r_type->size());
        if (argument->type == ast_node::ARRAY_ARGUMENT) {
            const std::shared_ptr<ast_node::array_argument_node> array_argument
                    = std::reinterpret_pointer_cast<ast_node::array_argument_node>(argument);

            this->variables.set_variable_address(array_argument->name, (long)this->stack.size());

            long offset = 0;
            constexpr int reg_size = 8;
            for (const token::token& size_argument : array_argument->dimension_vars) {
                this->variables.set_variable_address(size_argument.text, (long)this->stack.size() - offset);
                offset += reg_size;
            }
        } else {
            const std::shared_ptr<ast_node::variable_argument_node> variable_argument
                    = std::reinterpret_pointer_cast<ast_node::variable_argument_node>(argument);

            this->variables.set_variable_address(variable_argument->name, (long)this->stack.size());
        }
    }

    void generator::bind_lvalue(const std::shared_ptr<ast_node::lvalue_node>& lvalue,
                                const std::shared_ptr<resolved_type::resolved_type>& r_type) {
        if (lvalue->type == ast_node::ARGUMENT_LVALUE) {
            this->bind_argument(std::reinterpret_pointer_cast<ast_node::argument_lvalue_node>(lvalue)->argument,
                                r_type);
        } else {
            const std::shared_ptr<ast_node::tuple_lvalue_node> tuple_lvalue
                    = std::reinterpret_pointer_cast<ast_node::tuple_lvalue_node>(lvalue);
            const std::shared_ptr<resolved_type::tuple_resolved_type> tuple_r_type
                    = std::reinterpret_pointer_cast<resolved_type::tuple_resolved_type>(r_type);

            for (long index = (long)tuple_lvalue->lvalues.size() - 1; index >= 0; index--) {
                bind_lvalue(tuple_lvalue->lvalues[index], tuple_r_type->element_types[index]);
            }
        }
    }

    bool generator::fits_into_32(long value) { return (value & INT32_MAX) == value; }

    long generator::log_2(long value) {
        if (value < 0 || (value & (value - 1)) != 0) return -1;

        long log;
        for (log = 0; (1L << log) < value; ++log)
            ;

        return log;
    }

    generator::generator(
            const std::shared_ptr<symbol_table::symbol_table>& global_symbol_table,
            const std::shared_ptr<const_table>& constants,
            const std::shared_ptr<std::unordered_map<std::string, call_signature::call_signature>>& function_signatures,
            const std::shared_ptr<variable_table>& parent_variable_table, bool debug, unsigned int opt_level)
        : constants(constants), debug(debug), function_signatures(function_signatures),
          global_symbol_table(global_symbol_table), opt_level(opt_level), variables(parent_variable_table) {}

    //  ===========================
    //  ||  Function generator:  ||
    //  ===========================

    //  Statements:
    //  -----------

    void fn_generator::generate_stmt(const std::shared_ptr<ast_node::stmt_node>& statement) {
        switch (statement->type) {
            case ast_node::ASSERT_STMT:
                this->generate_stmt_assert(std::reinterpret_pointer_cast<ast_node::assert_stmt_node>(statement));
                break;
            case ast_node::LET_STMT:
                this->generate_stmt_let(std::reinterpret_pointer_cast<ast_node::let_stmt_node>(statement));
                break;
            case ast_node::RETURN_STMT:
                this->generate_stmt_return(std::reinterpret_pointer_cast<ast_node::return_stmt_node>(statement));
                break;
            default:
                throw std::runtime_error("`generate_stmt_let`: invalid statement: \"" + statement->s_expression()
                                         + "\"");
        }
    }

    void fn_generator::generate_stmt_assert(const std::shared_ptr<ast_node::assert_stmt_node>& statement) {
        if (this->debug) this->main_assembly << "\t;  START generate_stmt_assert\n";

        this->main_assembly << generate_expr(statement->expr) << "\tpop rax\n";
        this->stack.pop();

        const std::string next_jump = (*this->constants).next_jump();

        this->main_assembly << "\tcmp rax, 0\n"
                            << "\tjne " << next_jump << "\n";

        const bool needs_alignment = this->stack.needs_alignment();
        if (needs_alignment) {
            this->main_assembly << "\tsub rsp, 8";
            if (this->debug) this->main_assembly << " ; Align stack";
            this->main_assembly << "\n";
            this->stack.push();
        }

        this->main_assembly << "\tlea rdi, [rel " << (*this->constants)[statement->text] << "]";
        if (this->debug) this->main_assembly << " ; " << statement->text;
        this->main_assembly << "\n"
                            << "\tcall _fail_assertion\n";

        if (needs_alignment) {
            this->main_assembly << "\tadd rsp, 8";
            if (this->debug) this->main_assembly << " ; Remove alignment";
            this->main_assembly << "\n";
            this->stack.pop();
        }

        this->main_assembly << next_jump << ":\n";

        if (this->debug) this->main_assembly << "\t;  END generate_stmt_assert\n";
    }

    void fn_generator::generate_stmt_let(const std::shared_ptr<ast_node::let_stmt_node>& statement) {
        if (this->debug) this->main_assembly << "\t;  START generate_stmt_let\n";

        this->main_assembly << generate_expr(statement->expr);
        this->stack.pop();

        this->bind_lvalue(statement->lvalue, statement->expr->r_type);

        if (this->debug) this->main_assembly << "\t;  END generate_stmt_let\n";
    }

    void fn_generator::generate_stmt_return(const std::shared_ptr<ast_node::return_stmt_node>& statement) {
        if (this->debug) this->main_assembly << "\t;  START generate_stmt_return\n";

        this->main_assembly << generate_expr(statement->return_val);

        switch (statement->return_val->r_type->type) {
            case resolved_type::BOOL_TYPE:
            case resolved_type::INT_TYPE:
                this->main_assembly << "\tpop rax\n";
                this->stack.pop();
                break;
            case resolved_type::FLOAT_TYPE:
                this->main_assembly << "\tmovsd xmm0, [rsp]\n"
                                    << "\tadd rsp, 8\n";
                this->stack.pop();
                break;
            case resolved_type::ARRAY_TYPE:
            case resolved_type::TUPLE_TYPE:
                bool has_return_val = true;
                if (statement->return_val->r_type->type == resolved_type::TUPLE_TYPE) {
                    if (std::reinterpret_pointer_cast<resolved_type::tuple_resolved_type>(statement->return_val->r_type)
                                ->element_types.empty()) {
                        has_return_val = false;
                    }
                }
                if (has_return_val) {
                    const long size = (long)statement->return_val->r_type->size();
                    this->main_assembly << "\tmov rax, [rbp - 8]";
                    if (this->debug) this->main_assembly << " ; Address for the return value.";
                    this->main_assembly << "\n";

                    if (this->debug) this->main_assembly << "\t;  Moving " << size << " bytes from [rsp] to [rax]\n";

                    constexpr long reg_size = 8;
                    for (long offset = size - reg_size; offset >= 0; offset -= reg_size) {
                        if (this->debug) this->main_assembly << "\t";

                        this->main_assembly << "\tmov r10, [rsp + " << offset << "]\n";

                        if (this->debug) this->main_assembly << "\t";

                        this->main_assembly << "\tmov [rax + " << offset << "], r10\n";
                    }
                }
        }

        this->main_assembly << "\tadd rsp, " << this->stack.size() << "\n"
                            << "\tpop rbp\n"
                            << "\tret\n";

        if (this->debug) this->main_assembly << "\t;  END generate_stmt_return\n";
    }

    void fn_generator::handle_reg_binding(const std::shared_ptr<ast_node::binding_node>& binding,
                                          const std::shared_ptr<resolved_type::resolved_type>& r_type,
                                          const std::string& reg) {
        if (binding->type == ast_node::TUPLE_BINDING) {
            throw std::runtime_error("`handle_reg_binding`: not a register binding: \"" + binding->s_expression()
                                     + "\"");
        }

        const std::shared_ptr<ast_node::argument_node> argument
                = std::reinterpret_pointer_cast<ast_node::var_binding_node>(binding)->binding_arg;

        if (r_type->type == resolved_type::FLOAT_TYPE) {
            this->main_assembly << "\tsub rsp, 8\n"
                                << "\tmovsd [rsp], " << reg << "\n";
            this->bind_argument(argument, r_type);
        } else {
            this->main_assembly << "\tpush " << reg << "\n";
            this->bind_argument(argument, r_type);
        }
    }

    void fn_generator::handle_stack_binding(const std::shared_ptr<ast_node::binding_node>& binding,
                                            const std::shared_ptr<resolved_type::resolved_type>& r_type) {
        if (binding->type == ast_node::TUPLE_BINDING) {
            const std::shared_ptr<ast_node::tuple_binding_node> tuple_binding
                    = std::reinterpret_pointer_cast<ast_node::tuple_binding_node>(binding);
            const std::shared_ptr<resolved_type::tuple_resolved_type> tuple_type
                    = std::reinterpret_pointer_cast<resolved_type::tuple_resolved_type>(r_type);

            for (unsigned int index = 0; index < tuple_binding->bindings.size(); ++index) {
                this->handle_stack_binding(tuple_binding->bindings[index], tuple_type->element_types[index]);
            }
        } else {
            const std::shared_ptr<ast_node::argument_node> argument
                    = std::reinterpret_pointer_cast<ast_node::var_binding_node>(binding)->binding_arg;
            if (argument->type == ast_node::ARRAY_ARGUMENT) {
                const std::shared_ptr<ast_node::array_argument_node> array_argument
                        = std::reinterpret_pointer_cast<ast_node::array_argument_node>(argument);
                const std::shared_ptr<resolved_type::array_resolved_type> array_type
                        = std::reinterpret_pointer_cast<resolved_type::array_resolved_type>(r_type);

                constexpr int reg_size = 8;
                long rbp_offset_offset = rbp_offset;
                for (const token::token& dim_arg : array_argument->dimension_vars) {
                    this->variables.set_variable_address(dim_arg.text, rbp_offset_offset);
                    rbp_offset_offset -= reg_size;
                }
                this->variables.set_variable_address(array_argument->name, this->rbp_offset);
                this->rbp_offset -= (long)r_type->size();
            } else {
                const std::shared_ptr<ast_node::variable_argument_node> var_argument
                        = std::reinterpret_pointer_cast<ast_node::variable_argument_node>(argument);
                this->main_assembly << "\t;    [[" << this->rbp_offset << "]]\n";
                this->variables.set_variable_address(var_argument->name, this->rbp_offset);
                this->rbp_offset -= (long)r_type->size();
            }
        }
    }

    fn_generator::fn_generator(
            const std::shared_ptr<symbol_table::symbol_table>& global_symbol_table,
            const std::shared_ptr<ast_node::fn_cmd_node>& function, const std::shared_ptr<const_table>& constants,
            const std::shared_ptr<std::unordered_map<std::string, call_signature::call_signature>>& function_signatures,
            const std::shared_ptr<variable_table>& parent_variable_table, bool debug, unsigned int opt_level)
        : generator(global_symbol_table, constants, function_signatures, parent_variable_table, debug, opt_level),
          rbp_offset(initial_rbp_offset) {
        this->main_assembly << "\t;  [[" << initial_rbp_offset << "]]\n";
        this->main_assembly << function->name << ":\n_" << function->name << ":\n";

        const std::shared_ptr<name_info::function_info> func_info
                = std::reinterpret_pointer_cast<name_info::function_info>((*this->global_symbol_table)[function->name]);

        const call_signature::call_signature call_sig(func_info->call_args, func_info->r_type);
        (*this->function_signatures)[function->name] = call_sig;

        if (this->debug) this->main_assembly << "\t;  <<START 1: " << this->stack.size() << ">>\n";

        //  1. Start with the preamble:
        this->main_assembly << "\tpush rbp\n"
                            << "\tmov rbp, rsp\n";

        if (this->debug) this->main_assembly << "\t;  <<END 1: " << this->stack.size() << ">>\n";

        if (this->debug) this->main_assembly << "\t;  <<START 2: " << this->stack.size() << ">>\n";

        //  2. If the function returns a struct, push the address for the return value onto the stack.
        if ((call_sig.ret_type->type == resolved_type::ARRAY_TYPE
             || call_sig.ret_type->type == resolved_type::TUPLE_TYPE)
            && (call_sig.ret_type->size() > 0
                || !std::reinterpret_pointer_cast<resolved_type::tuple_resolved_type>(call_sig.ret_type)
                            ->element_types.empty())) {
            this->main_assembly << "\tpush rdi\n";
            this->stack.push();
        }

        if (this->debug) this->main_assembly << "\t;  <<END 2: " << this->stack.size() << ">>\n";

        if (this->debug) this->main_assembly << "\t;  <<START 3: " << this->stack.size() << ">>\n";

        //  3. Process each argument.
        for (unsigned int arg_index = 0; arg_index < function->bindings.size(); ++arg_index) {
            const std::tuple<std::shared_ptr<resolved_type::resolved_type>, bool, std::string>& arg_info
                    = call_sig.all_args[arg_index];
            if (std::get<1>(arg_info)) {
                this->handle_reg_binding(function->bindings[arg_index], std::get<0>(arg_info), std::get<2>(arg_info));
            } else {
                this->handle_stack_binding(function->bindings[arg_index], std::get<0>(arg_info));
            }
        }

        if (this->debug) this->main_assembly << "\t;  <<END 3: " << this->stack.size() << ">>\n";

        if (this->debug) this->main_assembly << "\t;  <<START 4: " << this->stack.size() << ">>\n";

        //  4. Generate each statement.
        bool has_return = false;
        for (const std::shared_ptr<ast_node::stmt_node>& statement : function->statements) {
            has_return = has_return || (statement->type == ast_node::RETURN_STMT);
            this->generate_stmt(statement);
        }

        if (this->debug) this->main_assembly << "\t;  <<END 4: " << this->stack.size() << ">>\n";

        if (this->debug) this->main_assembly << "\t;  <<START 5: " << this->stack.size() << ">>\n";

        //  5. Add the postamble, if no return statement was found.
        if (!has_return) {
            if (this->stack.size() > 0) this->main_assembly << "\tadd rsp, " << this->stack.size() << "\n";
            this->main_assembly << "\tpop rbp\n"
                                << "\tret\n";
        }

        if (this->debug) this->main_assembly << "\t;  <<END 5: " << this->stack.size() << ">>\n";
    }

    std::string fn_generator::assem() const { return this->main_assembly.str(); }

    //  =======================
    //  ||  Main generator:  ||
    //  =======================

    //  Commands:
    //  ---------

    void main_generator::generate_cmd(const std::shared_ptr<ast_node::cmd_node>& command) {
        switch (command->type) {
            case ast_node::ASSERT_CMD:
                return this->generate_cmd_assert(std::reinterpret_pointer_cast<ast_node::assert_cmd_node>(command));
            case ast_node::FN_CMD:
                return this->generate_cmd_fn(std::reinterpret_pointer_cast<ast_node::fn_cmd_node>(command));
            case ast_node::LET_CMD:
                return this->generate_cmd_let(std::reinterpret_pointer_cast<ast_node::let_cmd_node>(command));
            case ast_node::PRINT_CMD:
                return this->generate_cmd_print(std::reinterpret_pointer_cast<ast_node::print_cmd_node>(command));
            case ast_node::READ_CMD:
                return this->generate_cmd_read(std::reinterpret_pointer_cast<ast_node::read_cmd_node>(command));
            case ast_node::SHOW_CMD:
                return this->generate_cmd_show(std::reinterpret_pointer_cast<ast_node::show_cmd_node>(command));
            case ast_node::TIME_CMD:
                return this->generate_cmd_time(std::reinterpret_pointer_cast<ast_node::time_cmd_node>(command));
            case ast_node::TYPE_CMD:
                return this->generate_cmd_type(std::reinterpret_pointer_cast<ast_node::type_cmd_node>(command));
            case ast_node::WRITE_CMD:
                return this->generate_cmd_write(std::reinterpret_pointer_cast<ast_node::write_cmd_node>(command));
            default:
                throw std::runtime_error("`generate_cmd`: invalid command: \"" + command->s_expression() + "\"");
        }
    }

    void main_generator::generate_cmd_assert(const std::shared_ptr<ast_node::assert_cmd_node>& command) {
        if (this->debug) this->main_assembly << "\t;  START generate_cmd_assert\n";

        this->main_assembly << generate_expr(command->condition) << "\tpop rax\n";
        this->stack.pop();

        const std::string next_jump = (*this->constants).next_jump();

        this->main_assembly << "\tcmp rax, 0\n"
                            << "\tjne " << next_jump << "\n";

        const bool needs_alignment = this->stack.needs_alignment();
        if (needs_alignment) {
            this->main_assembly << "\tsub rsp, 8";
            if (this->debug) this->main_assembly << " ; Align stack";
            this->main_assembly << "\n";
            this->stack.push();
        }

        this->main_assembly << "\tlea rdi, [rel " << (*this->constants)[command->text] << "]";
        if (this->debug) this->main_assembly << " ; " << command->text;
        this->main_assembly << "\n"
                            << "\tcall _fail_assertion\n";

        if (needs_alignment) {
            this->main_assembly << "\tadd rsp, 8";
            if (this->debug) this->main_assembly << " ; Remove alignment";
            this->main_assembly << "\n";
            this->stack.pop();
        }

        this->main_assembly << next_jump << ":\n";

        if (this->debug) this->main_assembly << "\t;  END generate_cmd_assert\n";
    }

    void main_generator::generate_cmd_fn(const std::shared_ptr<ast_node::fn_cmd_node>& command) {
        const fn_generator function(this->global_symbol_table, command, this->constants, this->function_signatures,
                                    std::make_shared<variable_table>(this->variables), this->debug, this->opt_level);
        this->function_assemblies.emplace_back(function.assem());
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

                            this->variables.set_variable_address(array_argument->name, (long)this->stack.size());

                            long offset = 0;
                            constexpr int reg_size = 8;
                            for (const token::token& size_argument : array_argument->dimension_vars) {
                                this->variables.set_variable_address(size_argument.text,
                                                                     (long)this->stack.size() - offset);
                                offset += reg_size;
                            }
                        } else {
                            const std::shared_ptr<ast_node::variable_argument_node> variable_argument
                                    = std::reinterpret_pointer_cast<ast_node::variable_argument_node>(argument);

                            this->variables.set_variable_address(variable_argument->name, (long)this->stack.size());
                        }
                    } else {
                        const std::shared_ptr<ast_node::tuple_lvalue_node> tuple_lvalue
                                = std::reinterpret_pointer_cast<ast_node::tuple_lvalue_node>(lvalue);
                        const std::shared_ptr<resolved_type::tuple_resolved_type> tuple_r_type
                                = std::reinterpret_pointer_cast<resolved_type::tuple_resolved_type>(r_type);

                        for (long index = (long)tuple_lvalue->lvalues.size() - 1; index >= 0; index--) {
                            bind_lvalue(tuple_lvalue->lvalues[index], tuple_r_type->element_types[index]);
                        }
                    }
                };

        bind_lvalue(command->lvalue, command->expr->r_type);

        if (this->debug) this->main_assembly << "\t;  END generate_cmd_let\n";
    }

    void main_generator::generate_cmd_print(const std::shared_ptr<ast_node::print_cmd_node>& command) {
        if (this->debug) this->main_assembly << "\t;  START generate_cmd_print\n";

        this->main_assembly << "\tlea rdi, [rel " << (*this->constants)[command->text] << "]";
        if (this->debug) this->main_assembly << " ; " << command->text;
        this->main_assembly << "\n";

        const bool needs_alignment = this->stack.needs_alignment();
        if (needs_alignment) {
            this->main_assembly << "\tsub rsp, 8";
            if (this->debug) this->main_assembly << " ; Align stack";
            this->main_assembly << "\n";
            this->stack.push();
        }

        this->main_assembly << "\tcall _print\n";

        if (needs_alignment) {
            this->main_assembly << "\tadd rsp, 8";
            if (this->debug) this->main_assembly << " ; Remove alignment";
            this->main_assembly << "\n";
            this->stack.pop();
        }

        if (this->debug) this->main_assembly << "\t;  END generate_cmd_print\n";
    }

    void main_generator::generate_cmd_read(const std::shared_ptr<ast_node::read_cmd_node>& command) {
        constexpr unsigned int image_size = 24;

        if (this->debug) this->main_assembly << "\t;  START generate_cmd_read\n";

        this->main_assembly << "\t;  [[" << this->stack.size() << "]]\n";

        this->main_assembly << "\t;  [[" << this->stack.size() << "]]\n";

        this->main_assembly << "\tsub rsp, " << image_size << "\n";
        this->stack.push(image_size);

        this->main_assembly << "\tlea rdi, [rsp]\n";

        const bool needs_alignment = this->stack.needs_alignment();
        if (needs_alignment) {
            this->main_assembly << "\tsub rsp, 8";
            if (this->debug) this->main_assembly << " ; Align stack";
            this->main_assembly << "\n";
            this->stack.push();
        }

        this->main_assembly << "\t;  [[" << this->stack.size() << "]]\n";

        this->main_assembly << "\tlea rsi, [rel " << (*this->constants)[command->file_name] << "]";
        if (this->debug) this->main_assembly << " ; " << command->file_name;
        this->main_assembly << "\n"
                            << "\tcall _read_image\n";

        if (needs_alignment) {
            this->main_assembly << "\tadd rsp, 8";
            if (this->debug) this->main_assembly << " ; Remove alignment";
            this->main_assembly << "\n";
            this->stack.pop();
        }

        const std::shared_ptr<resolved_type::resolved_type> float_type = std::make_shared<resolved_type::resolved_type>(
                resolved_type::FLOAT_TYPE);
        const std::shared_ptr<resolved_type::tuple_resolved_type> pixel_type
                = std::make_shared<resolved_type::tuple_resolved_type>(
                        std::vector<std::shared_ptr<resolved_type::resolved_type>> {float_type, float_type, float_type,
                                                                                    float_type});
        const std::shared_ptr<resolved_type::array_resolved_type> array_r_type
                = std::make_shared<resolved_type::array_resolved_type>(pixel_type, 2);

        this->main_assembly << "\t;  [[" << this->stack.size() << "]]\n";

        this->stack.pop();
        this->bind_argument(command->read_dest, array_r_type);

        this->main_assembly << "\t;  [[" << this->stack.size() << "]]\n";

        if (this->debug) this->main_assembly << "\t;  END generate_cmd_read\n";
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
        this->main_assembly << "\n";

        this->main_assembly << "\tlea rsi, [rsp]\n"
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

    void main_generator::generate_cmd_time(const std::shared_ptr<ast_node::time_cmd_node>& command) {
        if (this->debug) this->main_assembly << "\t;  START generate_cmd_time\n";

        bool needs_alignment = this->stack.needs_alignment();
        if (needs_alignment) {
            this->main_assembly << "\tsub rsp, 8";
            if (this->debug) this->main_assembly << " ; Align stack";
            this->main_assembly << "\n";
            this->stack.push();
        }

        this->main_assembly << "\tcall _get_time\n";

        if (needs_alignment) {
            this->main_assembly << "\tadd rsp, 8";
            if (this->debug) this->main_assembly << " ; Remove alignment";
            this->main_assembly << "\n";
            this->stack.pop();
        }

        this->main_assembly << "\tsub rsp, 8\n"
                            << "\tmovsd [rsp], xmm0\n";
        this->stack.push();
        const long time_offset_start = this->stack.size();

        this->generate_cmd(command->command);

        needs_alignment = this->stack.needs_alignment();
        if (needs_alignment) {
            this->main_assembly << "\tsub rsp, 8";
            if (this->debug) this->main_assembly << " ; Align stack";
            this->main_assembly << "\n";
            this->stack.push();
        }

        this->main_assembly << "\tcall _get_time\n";

        if (needs_alignment) {
            this->main_assembly << "\tadd rsp, 8";
            if (this->debug) this->main_assembly << " ; Remove alignment";
            this->main_assembly << "\n";
            this->stack.pop();
        }

        this->main_assembly << "\tsub rsp, 8\n"
                            << "\tmovsd [rsp], xmm0\n";
        this->stack.push();

        this->main_assembly << "\tmovsd xmm0, [rsp]\n"
                            << "\tadd rsp, 8\n";
        this->stack.pop();

        this->main_assembly << "\tmovsd xmm1, [rsp + " << this->stack.size() - time_offset_start << "]\n"
                            << "\tsubsd xmm0, xmm1\n";

        if (needs_alignment) {
            this->main_assembly << "\tsub rsp, 8";
            if (this->debug) this->main_assembly << " ; Align stack";
            this->main_assembly << "\n";
            this->stack.push();
        }
        this->main_assembly << "\tcall _print_time\n";

        if (needs_alignment) {
            this->main_assembly << "\tadd rsp, 8";
            if (this->debug) this->main_assembly << " ; Remove alignment";
            this->main_assembly << "\n";
            this->stack.pop();
        }

        if (this->debug) this->main_assembly << "\t;  END generate_cmd_time\n";
    }

    void main_generator::generate_cmd_type(const std::shared_ptr<ast_node::type_cmd_node>&) {
        if (this->debug) this->main_assembly << "\t;  START generate_cmd_type\n";

        if (this->debug) this->main_assembly << "\t;  END generate_cmd_type\n";
    }

    void main_generator::generate_cmd_write(const std::shared_ptr<ast_node::write_cmd_node>& command) {
        if (this->debug) this->main_assembly << "\t;  START generate_cmd_write\n";

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
                            << (*this->constants)[command->file_name] << "]\n"
                            << "\tcall _write_image\n"
                            << "\tadd rsp, " << this->stack.pop() << "\n";

        if (needs_alignment) {
            this->main_assembly << "\tadd rsp, 8";
            if (this->debug) this->main_assembly << " ; Remove alignment";
            this->main_assembly << "\n";
            this->stack.pop();
        }

        if (this->debug) this->main_assembly << "\t;  END generate_cmd_write\n";
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

    main_generator::main_generator(const std::shared_ptr<symbol_table::symbol_table>& global_symbol_table,
                                   const std::vector<std::shared_ptr<ast_node::ast_node>>& nodes, bool debug,
                                   unsigned int opt_level)
        : generator(global_symbol_table, std::make_shared<const_table>(),
                    std::make_shared<std::unordered_map<std::string, call_signature::call_signature>>(), nullptr, debug,
                    opt_level),
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

        constexpr long arg_address = -16;
        this->variables.set_variable_address("args", arg_address);
        this->variables.set_variable_address("argnum", arg_address);

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

    std::string generate(const std::shared_ptr<symbol_table::symbol_table>& global_symbol_table,
                         const std::vector<std::shared_ptr<ast_node::ast_node>>& nodes, bool debug,
                         unsigned int opt_level) {
        return main_generator(global_symbol_table, nodes, debug, opt_level).assem();
    }
}  //  namespace generator
