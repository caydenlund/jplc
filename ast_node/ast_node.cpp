/**
 * @file ast_node.cpp
 * @package Assignments 3-5
 * @author Cayden Lund (u1182408)
 * @brief Implements the AST node classes.
 *
 */

#include <sstream>

#include "ast_node.hpp"

namespace ast_node {
    /*
    =====================
    ||  Superclasses:  ||
    =====================
    */

    ast_node::ast_node(node_type type) : type(type) {}

    std::string ast_node::s_expression() const {
        //  Unused; overridden.
        return "";
    }

    argument_node::argument_node(node_type type) : ast_node(type) {}

    std::string argument_node::s_expression() const {
        //  Unused; overridden.
        return "";
    }

    binding_node::binding_node(node_type type) : ast_node(type) {}

    std::string binding_node::s_expression() const {
        //  Unused; overridden.
        return "";
    }

    cmd_node::cmd_node(node_type type) : ast_node(type) {}

    std::string cmd_node::s_expression() const {
        //  Unused; overridden.
        return "";
    }

    expr_node::expr_node(node_type type) : ast_node(type) {}

    std::string expr_node::s_expression() const {
        //  Unused; overridden.
        return "";
    }

    lvalue_node::lvalue_node(node_type type) : ast_node(type) {}

    std::string lvalue_node::s_expression() const {
        //  Unused; overridden.
        return "";
    }

    stmt_node::stmt_node(node_type type) : ast_node(type) {}

    std::string stmt_node::s_expression() const {
        //  Unused; overridden.
        return "";
    }

    type_node::type_node(node_type type) : ast_node(type) {}

    std::string type_node::s_expression() const {
        //  Unused; overridden.
        return "";
    }

    /*
    ===================
    ||  Subclasses:  ||
    ===================
    */

    //  Arguments:
    //  ----------
    array_argument_node::array_argument_node(const token::token& main_var,
                                             const std::vector<token::token>& dimension_vars)
        : argument_node(node_type::ARRAY_ARGUMENT), name(main_var.text), dimension_vars(dimension_vars) {
        if (main_var.type != token::token_type::VARIABLE)
            throw std::runtime_error("`array_argument_node`: not a variable: \"" + main_var.text + "\"");
        for (const token::token& dimension_var : dimension_vars) {
            if (dimension_var.type != token::token_type::VARIABLE)
                throw std::runtime_error("`array_argument_node`: not a variable: \"" + dimension_var.text + "\"");
        }
    }

    std::string array_argument_node::s_expression() const {
        std::stringstream result;
        result << "(ArrayArgument " << this->name;
        for (const token::token& dimension_var : dimension_vars) { result << " " << dimension_var.text; }
        result << ")";
        return result.str();
    }

    variable_argument_node::variable_argument_node(const token::token& var_tok)
        : argument_node(node_type::VARIABLE_ARGUMENT), name(var_tok.text) {
        if (var_tok.type != token::token_type::VARIABLE)
            throw std::runtime_error("`variable_argument_node`: not a variable: " + var_tok.text);
    }

    std::string variable_argument_node::s_expression() const { return "(VarArgument " + this->name + ")"; }

    //  Bindings:
    //  ---------
    tuple_binding_node::tuple_binding_node(const std::vector<std::shared_ptr<binding_node>>& bindings)
        : binding_node(node_type::TUPLE_BINDING), bindings(bindings) {}

    std::string tuple_binding_node::s_expression() const {
        std::stringstream result;
        result << "(TupleBinding";
        for (const std::shared_ptr<binding_node>& binding : bindings) { result << " " << binding->s_expression(); }
        result << ")";
        return result.str();
    }

    var_binding_node::var_binding_node(const std::shared_ptr<argument_node>& binding_arg,
                                       const std::shared_ptr<type_node>& binding_type)
        : binding_node(node_type::VAR_BINDING), binding_arg(binding_arg), binding_type(binding_type) {}

    std::string var_binding_node::s_expression() const {
        return "(VarBinding " + this->binding_arg->s_expression() + " " + this->binding_type->s_expression() + ")";
    }

    //  Commands:
    //  ---------
    assert_cmd_node::assert_cmd_node(const std::shared_ptr<expr_node>& condition, const token::token& string_tok)
        : cmd_node(node_type::ASSERT_CMD), condition(condition), text(token::string_value(string_tok)) {
        if (string_tok.type != token::token_type::STRING)
            throw std::runtime_error("Not a string token: \"" + string_tok.text + "\"");
    }

    std::string assert_cmd_node::s_expression() const {
        return "(AssertCmd " + this->condition->s_expression() + " \"" + this->text + "\")";
    }

    fn_cmd_node::fn_cmd_node(const token::token& var_tok, const std::vector<std::shared_ptr<binding_node>>& bindings,
                             const std::shared_ptr<type_node>& return_type,
                             const std::vector<std::shared_ptr<stmt_node>>& statements)
        : cmd_node(node_type::FN_CMD), name(var_tok.text), bindings(bindings), return_type(return_type),
          statements(statements) {
        if (var_tok.type != token::token_type::VARIABLE)
            throw std::runtime_error("`fn_cmd_node`: not a variable: \"" + var_tok.text + "\"");
    }

    std::string fn_cmd_node::s_expression() const {
        std::stringstream result;
        result << "(FnCmd " << this->name << " (";
        if (!this->bindings.empty()) {
            for (unsigned int index = 0; index < this->bindings.size() - 1; index++) {
                result << this->bindings[index]->s_expression() << " ";
            }
            result << this->bindings[this->bindings.size() - 1]->s_expression();
        }
        result << ") " << this->return_type->s_expression();
        for (const std::shared_ptr<stmt_node>& stmt : this->statements) { result << " " << stmt->s_expression(); }
        result << ")";
        return result.str();
    }

    let_cmd_node::let_cmd_node(const std::shared_ptr<lvalue_node>& lvalue, const std::shared_ptr<expr_node>& expr)
        : cmd_node(node_type::LET_CMD), lvalue(lvalue), expr(expr) {}

    std::string let_cmd_node::s_expression() const {
        return "(LetCmd " + this->lvalue->s_expression() + " " + this->expr->s_expression() + ")";
    }

    print_cmd_node::print_cmd_node(const token::token& string_tok)
        : cmd_node(node_type::PRINT_CMD), text(token::string_value(string_tok)) {
        if (string_tok.type != token::token_type::STRING)
            throw std::runtime_error("`print_cmd_node`: not a string token: \"" + string_tok.text + "\"");
    }

    std::string print_cmd_node::s_expression() const { return "(PrintCmd \"" + this->text + "\")"; }

    read_cmd_node::read_cmd_node(const token::token& string_tok, const std::shared_ptr<argument_node>& read_dest)
        : cmd_node(node_type::READ_CMD), file_name(token::string_value(string_tok)), read_dest(read_dest) {
        if (string_tok.type != token::token_type::STRING)
            throw std::runtime_error("`read_cmd_node`: not a string token: \"" + string_tok.text + "\"");
    }

    std::string read_cmd_node::s_expression() const {
        return "(ReadCmd \"" + this->file_name + "\" " + this->read_dest->s_expression() + ")";
    }

    show_cmd_node::show_cmd_node(const std::shared_ptr<expr_node>& expr) : cmd_node(node_type::SHOW_CMD), expr(expr) {}

    std::string show_cmd_node::s_expression() const { return "(ShowCmd " + this->expr->s_expression() + ")"; }

    time_cmd_node::time_cmd_node(const std::shared_ptr<cmd_node>& command)
        : cmd_node(node_type::TIME_CMD), command(command) {}

    std::string time_cmd_node::s_expression() const { return "(TimeCmd " + this->command->s_expression() + ")"; }

    type_cmd_node::type_cmd_node(const token::token& var_tok, const std::shared_ptr<type_node>& new_type)
        : cmd_node(node_type::TYPE_CMD), name(var_tok.text), new_type(new_type) {
        if (var_tok.type != token::token_type::VARIABLE)
            throw std::runtime_error("`type_cmd_node`: not a variable: \"" + var_tok.text + "\"");
    }

    std::string type_cmd_node::s_expression() const {
        return "(TypeCmd " + this->name + " " + this->new_type->s_expression() + ")";
    }

    write_cmd_node::write_cmd_node(const std::shared_ptr<expr_node>& expr, const token::token& string_tok)
        : cmd_node(node_type::WRITE_CMD), expr(expr), file_name(token::string_value(string_tok)) {
        if (string_tok.type != token::token_type::STRING)
            throw std::runtime_error("`write_cmd_node`: not a string token: \"" + string_tok.text + "\"");
    }

    std::string write_cmd_node::s_expression() const {
        return "(WriteCmd " + this->expr->s_expression() + " \"" + this->file_name + "\")";
    }

    //  Expressions:
    //  ------------
    array_index_expr_node::array_index_expr_node(const std::shared_ptr<expr_node>& array,
                                                 const std::vector<std::shared_ptr<expr_node>>& params)
        : expr_node(node_type::ARRAY_INDEX_EXPR), array(array), params(params) {}

    std::string array_index_expr_node::s_expression() const {
        std::stringstream result;
        result << "(ArrayIndexExpr";
        if (this->r_type != nullptr) result << " " << this->r_type->s_expression();
        result << " " << this->array->s_expression();
        for (const std::shared_ptr<expr_node>& param : this->params) { result << " " << param->s_expression(); }
        result << ")";
        return result.str();
    }

    array_literal_expr_node::array_literal_expr_node(const std::vector<std::shared_ptr<expr_node>>& expressions)
        : expr_node(node_type::ARRAY_LITERAL_EXPR), expressions(expressions) {}

    std::string array_literal_expr_node::s_expression() const {
        std::stringstream result;
        result << "(ArrayLiteralExpr";
        if (this->r_type != nullptr) result << " " << this->r_type->s_expression();
        for (const std::shared_ptr<expr_node>& expr : this->expressions) { result << " " << expr->s_expression(); }
        result << ")";
        return result.str();
    }

    array_loop_expr_node::array_loop_expr_node(const std::vector<binding_pair_t>& binding_pairs,
                                               const std::shared_ptr<expr_node>& item_expr)
        : expr_node(node_type::ARRAY_LOOP_EXPR), binding_pairs(binding_pairs), item_expr(item_expr) {
        for (const binding_pair_t& binding : binding_pairs) {
            if (std::get<0>(binding).type != token::token_type::VARIABLE)
                throw std::runtime_error("`array_loop_expr_node`: not a variable token: \"" + std::get<0>(binding).text
                                         + "\"");
        }
    }

    std::string array_loop_expr_node::s_expression() const {
        std::stringstream result;
        result << "(ArrayLoopExpr";
        if (this->r_type != nullptr) result << " " << this->r_type->s_expression();
        for (const array_loop_expr_node::binding_pair_t& binding : this->binding_pairs) {
            result << " " << std::get<0>(binding).text << " " << std::get<1>(binding)->s_expression();
        }
        if (this->item_expr != nullptr) result << " " << this->item_expr->s_expression();
        result << ")";
        return result.str();
    }

    binop_expr_node::binop_expr_node(const token::token& binop, const std::shared_ptr<expr_node>& left_operand,
                                     const std::shared_ptr<expr_node>& right_operand)
        : expr_node(node_type::BINOP_EXPR), left_operand(left_operand), right_operand(right_operand) {
        if (binop.type != token::token_type::OP)
            throw std::runtime_error("`binop_expr_node`: not an operator: \"" + binop.text + "\"");
        if (binop.text == "+") this->operator_type = op_type::BINOP_PLUS;
        else if (binop.text == "-")
            this->operator_type = op_type::BINOP_MINUS;
        else if (binop.text == "*")
            this->operator_type = op_type::BINOP_TIMES;
        else if (binop.text == "/")
            this->operator_type = op_type::BINOP_DIVIDE;
        else if (binop.text == "%")
            this->operator_type = op_type::BINOP_MOD;
        else if (binop.text == "<")
            this->operator_type = op_type::BINOP_LT;
        else if (binop.text == ">")
            this->operator_type = op_type::BINOP_GT;
        else if (binop.text == "==")
            this->operator_type = op_type::BINOP_EQ;
        else if (binop.text == "!=")
            this->operator_type = op_type::BINOP_NEQ;
        else if (binop.text == "<=")
            this->operator_type = op_type::BINOP_LEQ;
        else if (binop.text == ">=")
            this->operator_type = op_type::BINOP_GEQ;
        else if (binop.text == "&&")
            this->operator_type = op_type::BINOP_AND;
        else if (binop.text == "||")
            this->operator_type = op_type::BINOP_OR;
        else
            throw std::runtime_error("`binop_expr_node`: not a binary operator: \"" + binop.text + "\"");
    }

    std::string binop_expr_node::s_expression() const {
        std::stringstream result;
        result << "(BinopExpr";
        if (this->r_type != nullptr) result << " " << this->r_type->s_expression();
        result << " " << this->left_operand->s_expression() << " ";
        switch (this->operator_type) {
            case op_type::BINOP_PLUS:
                result << "+";
                break;
            case op_type::BINOP_MINUS:
                result << "-";
                break;
            case op_type::BINOP_TIMES:
                result << "*";
                break;
            case op_type::BINOP_DIVIDE:
                result << "/";
                break;
            case op_type::BINOP_MOD:
                result << "%";
                break;
            case op_type::BINOP_LT:
                result << "<";
                break;
            case op_type::BINOP_GT:
                result << ">";
                break;
            case op_type::BINOP_EQ:
                result << "==";
                break;
            case op_type::BINOP_NEQ:
                result << "!=";
                break;
            case op_type::BINOP_LEQ:
                result << "<=";
                break;
            case op_type::BINOP_GEQ:
                result << ">=";
                break;
            case op_type::BINOP_AND:
                result << "&&";
                break;
            case op_type::BINOP_OR:
                result << "||";
                break;
            default:
                throw std::runtime_error("`binop_expr_node`: unhandled operator type");
        }
        result << " " << this->right_operand->s_expression() << ")";
        return result.str();
    }

    call_expr_node::call_expr_node(const token::token& name, const std::vector<std::shared_ptr<expr_node>>& call_args)
        : expr_node(node_type::CALL_EXPR), name(name.text), call_args(call_args) {
        if (name.type != token::token_type::VARIABLE)
            throw std::runtime_error("`call_expr_node`: not a variable token: \"" + name.text + "\"");
    }

    std::string call_expr_node::s_expression() const {
        std::stringstream result;
        result << "(CallExpr";
        if (this->r_type != nullptr) result << " " << this->r_type->s_expression();
        result << " " << this->name;
        for (const std::shared_ptr<expr_node>& arg : this->call_args) { result << " " << arg->s_expression(); }
        result << ")";
        return result.str();
    }

    else_tok_expr_node::else_tok_expr_node() : expr_node(node_type::ELSE_TOK_EXPR) {}

    std::string else_tok_expr_node::s_expression() const { return ""; }

    false_expr_node::false_expr_node() : expr_node(node_type::FALSE_EXPR) {}

    std::string false_expr_node::s_expression() const {
        return "(FalseExpr" + ((this->r_type != nullptr) ? (" " + this->r_type->s_expression()) : "") + ")";
    }

    float_expr_node::float_expr_node(const token::token& float_tok) : expr_node(node_type::FLOAT_EXPR) {
        if (float_tok.type != token::token_type::FLOATVAL)
            throw std::runtime_error("`float_expr_node`: not a floating-point literal: \"" + float_tok.text + "\"");
        try {
            value = std::stod(float_tok.text);
        } catch (const std::exception& err) { throw std::runtime_error("Floating-point literal overflow"); }
    }

    std::string float_expr_node::s_expression() const {
        std::stringstream result;
        result << "(FloatExpr";
        if (this->r_type != nullptr) result << " " << this->r_type->s_expression();
        result << " " << (long)this->value << ")";
        return result.str();
    }

    if_expr_node::if_expr_node(const std::shared_ptr<expr_node>& conditional_expr,
                               const std::shared_ptr<expr_node>& affirmative_expr,
                               const std::shared_ptr<expr_node>& negative_expr)
        : expr_node(node_type::IF_EXPR), conditional_expr(conditional_expr), affirmative_expr(affirmative_expr),
          negative_expr(negative_expr) {}

    std::string if_expr_node::s_expression() const {
        std::stringstream result;
        result << "(IfExpr";
        if (this->r_type != nullptr) result << " " << this->r_type->s_expression();
        if (this->conditional_expr != nullptr) { result << " " << this->conditional_expr->s_expression(); }
        if (this->affirmative_expr != nullptr) { result << " " << this->affirmative_expr->s_expression(); }
        if (this->negative_expr != nullptr) { result << " " << this->negative_expr->s_expression(); }
        result << ")";
        return result.str();
    }

    integer_expr_node::integer_expr_node(const token::token& int_tok) : expr_node(node_type::INTEGER_EXPR) {
        if (int_tok.type != token::token_type::INTVAL)
            throw std::runtime_error("`integer_expr_node`: not an integer literal: \"" + int_tok.text + "\"");
        try {
            value = std::stol(int_tok.text);
        } catch (const std::exception& err) { throw std::runtime_error("Integer literal overflow"); }
    }

    std::string integer_expr_node::s_expression() const {
        std::stringstream result;
        result << "(IntExpr";
        if (this->r_type != nullptr) result << " " << this->r_type->s_expression();
        result << " " << this->value << ")";
        return result.str();
    }

    op_expr_node::op_expr_node(const token::token& op_tok) : expr_node(node_type::OP_EXPR) {
        if (op_tok.type != token::token_type::OP)
            throw std::runtime_error("`op_expr_node`: not an operator: \"" + op_tok.text + "\"");
        if (op_tok.text == "+") this->operator_type = op_type::BINOP_PLUS;
        else if (op_tok.text == "-")
            this->operator_type = op_type::BINOP_MINUS;
        else if (op_tok.text == "*")
            this->operator_type = op_type::BINOP_TIMES;
        else if (op_tok.text == "/")
            this->operator_type = op_type::BINOP_DIVIDE;
        else if (op_tok.text == "%")
            this->operator_type = op_type::BINOP_MOD;
        else if (op_tok.text == "<")
            this->operator_type = op_type::BINOP_LT;
        else if (op_tok.text == ">")
            this->operator_type = op_type::BINOP_GT;
        else if (op_tok.text == "==")
            this->operator_type = op_type::BINOP_EQ;
        else if (op_tok.text == "!=")
            this->operator_type = op_type::BINOP_NEQ;
        else if (op_tok.text == "<=")
            this->operator_type = op_type::BINOP_LEQ;
        else if (op_tok.text == ">=")
            this->operator_type = op_type::BINOP_GEQ;
        else if (op_tok.text == "&&")
            this->operator_type = op_type::BINOP_AND;
        else if (op_tok.text == "||")
            this->operator_type = op_type::BINOP_OR;
        else if (op_tok.text == "!")
            this->operator_type = op_type::UNOP_INV;
        else
            throw std::runtime_error("`op_expr_node`: not an operator: \"" + op_tok.text + "\"");
    }

    std::string op_expr_node::s_expression() const {
        switch (this->operator_type) {
            case op_type::BINOP_PLUS:
                return "+";
            case op_type::BINOP_MINUS:
                return "-";
            case op_type::BINOP_TIMES:
                return "*";
            case op_type::BINOP_DIVIDE:
                return "/";
            case op_type::BINOP_MOD:
                return "%";
            case op_type::BINOP_LT:
                return "<";
            case op_type::BINOP_GT:
                return ">";
            case op_type::BINOP_EQ:
                return "==";
            case op_type::BINOP_NEQ:
                return "!=";
            case op_type::BINOP_LEQ:
                return "<=";
            case op_type::BINOP_GEQ:
                return ">=";
            case op_type::BINOP_AND:
                return "&&";
            case op_type::BINOP_OR:
                return "||";
            case op_type::UNOP_INV:
                return "!";
            case op_type::UNOP_NEG:
                return "-";
            default:
                return "";
        }
    }

    sum_loop_expr_node::sum_loop_expr_node(std::vector<binding_pair_t>& binding_pairs,
                                           const std::shared_ptr<expr_node>& sum_expr)
        : expr_node(node_type::SUM_LOOP_EXPR), binding_pairs(binding_pairs), sum_expr(sum_expr) {
        for (const binding_pair_t& binding : binding_pairs) {
            if (std::get<0>(binding).type != token::token_type::VARIABLE)
                throw std::runtime_error("`sum_loop_expr_node`: not a variable: \"" + std::get<0>(binding).text + "\"");
        }
    }

    std::string sum_loop_expr_node::s_expression() const {
        std::stringstream result;
        result << "(SumLoopExpr";
        if (this->r_type != nullptr) result << " " << this->r_type->s_expression();
        for (const array_loop_expr_node::binding_pair_t& binding : this->binding_pairs) {
            result << " " << std::get<0>(binding).text << " " << std::get<1>(binding)->s_expression();
        }
        if (this->sum_expr != nullptr) result << " " << this->sum_expr->s_expression();
        result << ")";
        return result.str();
    }

    then_tok_expr_node::then_tok_expr_node() : expr_node(node_type::THEN_TOK_EXPR) {}

    std::string then_tok_expr_node::s_expression() const { return ""; }

    true_expr_node::true_expr_node() : expr_node(node_type::TRUE_EXPR) {}

    std::string true_expr_node::s_expression() const {
        return "(TrueExpr" + ((this->r_type != nullptr) ? (" " + this->r_type->s_expression()) : "") + ")";
    }

    tuple_index_expr_node::tuple_index_expr_node(const std::shared_ptr<expr_node>& expr,
                                                 const std::shared_ptr<integer_expr_node>& index)
        : expr_node(node_type::TUPLE_INDEX_EXPR), expr(expr), index(index) {}

    std::string tuple_index_expr_node::s_expression() const {
        std::stringstream result;
        result << "(TupleIndexExpr";
        if (this->r_type != nullptr) result << " " << this->r_type->s_expression();
        result << " " << this->expr->s_expression() << " " << index->value << ")";
        return result.str();
    }

    tuple_literal_expr_node::tuple_literal_expr_node(const std::vector<std::shared_ptr<expr_node>>& exprs)
        : expr_node(node_type::TUPLE_LITERAL_EXPR), exprs(exprs) {}

    std::string tuple_literal_expr_node::s_expression() const {
        std::stringstream result;
        result << "(TupleLiteralExpr";
        if (this->r_type != nullptr) result << " " << this->r_type->s_expression();
        for (const std::shared_ptr<expr_node>& expr : this->exprs) { result << " " << expr->s_expression(); }
        result << ")";
        return result.str();
    }

    unop_expr_node::unop_expr_node(const token::token& unop, const std::shared_ptr<expr_node>& operand)
        : expr_node(node_type::UNOP_EXPR), operand(operand) {
        if (unop.type != token::token_type::OP)
            throw std::runtime_error("`unop_expr_node`: not an operator: \"" + unop.text + "\"");
        if (unop.text == "!") this->operator_type = op_type::UNOP_INV;
        else if (unop.text == "-")
            this->operator_type = op_type::UNOP_NEG;
        else
            throw std::runtime_error("`unop_expr_node`: not a unary operator: \"" + unop.text + "\"");
    }

    std::string unop_expr_node::s_expression() const {
        std::stringstream result;
        result << "(UnopExpr ";
        if (this->r_type != nullptr) result << this->r_type->s_expression() << " ";
        switch (this->operator_type) {
            case op_type::UNOP_INV:
                result << "!";
                break;
            case op_type::UNOP_NEG:
                result << "-";
                break;
            default:
                throw std::runtime_error("`unop_expr_node`: unhandled operator");
        }
        result << " " << this->operand->s_expression() << ")";
        return result.str();
    }

    variable_expr_node::variable_expr_node(const token::token& var_tok)
        : expr_node(node_type::VARIABLE_EXPR), name(var_tok.text) {
        if (var_tok.type != token::token_type::VARIABLE)
            throw std::runtime_error("`variable_expr_node`: not a variable token: \"" + var_tok.text + "\"");
    }

    std::string variable_expr_node::s_expression() const {
        std::stringstream result;
        result << "(VarExpr";
        if (this->r_type != nullptr) result << " " << this->r_type->s_expression();
        result << " " << this->name << ")";
        return result.str();
    }

    //  LValues:
    //  --------
    argument_lvalue_node::argument_lvalue_node(const std::shared_ptr<argument_node>& argument)
        : lvalue_node(node_type::ARGUMENT_LVALUE), argument(argument) {}

    std::string argument_lvalue_node::s_expression() const {
        return "(ArgLValue " + this->argument->s_expression() + ")";
    }

    tuple_lvalue_node::tuple_lvalue_node(const std::vector<std::shared_ptr<lvalue_node>>& lvalues)
        : lvalue_node(node_type::TUPLE_LVALUE), lvalues(lvalues) {}

    std::string tuple_lvalue_node::s_expression() const {
        std::stringstream result;
        result << "(TupleLValue";
        for (const std::shared_ptr<lvalue_node>& lvalue : this->lvalues) { result << " " << lvalue->s_expression(); }
        result << ")";
        return result.str();
    }

    //  Statements:
    //  -----------
    assert_stmt_node::assert_stmt_node(const std::shared_ptr<expr_node>& expr, const token::token& string_tok)
        : stmt_node(node_type::ASSERT_STMT), expr(expr), text(token::string_value(string_tok)) {
        if (string_tok.type != token::token_type::STRING)
            throw std::runtime_error("`assert_stmt_node`: not a string token: \"" + string_tok.text + "\"");
    }

    std::string assert_stmt_node::s_expression() const {
        return "(AssertStmt " + this->expr->s_expression() + " \"" + this->text + "\")";
    }

    let_stmt_node::let_stmt_node(const std::shared_ptr<lvalue_node>& lvalue, const std::shared_ptr<expr_node>& expr)
        : stmt_node(node_type::LET_STMT), lvalue(lvalue), expr(expr) {}

    std::string let_stmt_node::s_expression() const {
        return "(LetStmt " + this->lvalue->s_expression() + " " + this->expr->s_expression() + ")";
    }

    return_stmt_node::return_stmt_node(const std::shared_ptr<expr_node>& return_val)
        : stmt_node(node_type::RETURN_STMT), return_val(return_val) {}

    std::string return_stmt_node::s_expression() const {
        return "(ReturnStmt " + this->return_val->s_expression() + ")";
    }

    //  Types:
    //  ------
    array_type_node::array_type_node(const std::shared_ptr<type_node>& main_type, const unsigned long dimensions)
        : type_node(node_type::ARRAY_TYPE), main_type(main_type), dimensions(dimensions) {}

    std::string array_type_node::s_expression() const {
        return "(ArrayType " + main_type->s_expression() + " " + std::to_string(dimensions) + ")";
    }

    bool_type_node::bool_type_node() : type_node(node_type::BOOL_TYPE) {}

    std::string bool_type_node::s_expression() const { return "(BoolType)"; }

    float_type_node::float_type_node() : type_node(node_type::FLOAT_TYPE) {}

    std::string float_type_node::s_expression() const { return "(FloatType)"; }

    int_type_node::int_type_node() : type_node(node_type::INT_TYPE) {}

    std::string int_type_node::s_expression() const { return "(IntType)"; }

    tuple_type_node::tuple_type_node(const std::vector<std::shared_ptr<type_node>>& types)
        : type_node(node_type::TUPLE_TYPE), types(types) {}

    std::string tuple_type_node::s_expression() const {
        std::stringstream result;
        result << "(TupleType";
        for (const std::shared_ptr<type_node>& type : this->types) { result << " " << type->s_expression(); }
        result << ")";
        return result.str();
    }

    variable_type_node::variable_type_node(const token::token& var_tok)
        : type_node(node_type::VARIABLE_TYPE), name(var_tok.text) {
        if (var_tok.type != token::token_type::VARIABLE)
            throw std::runtime_error("`variable_type_node`: not a variable: \"" + var_tok.text + "\"");
    }

    std::string variable_type_node::s_expression() const { return "(VarType " + this->name + ")"; }
}  //  namespace ast_node
