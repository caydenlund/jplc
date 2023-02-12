/**
 * @file ast_node.cpp
 * @package Assignments 3-5
 * @author Cayden Lund (u1182408)
 * @brief Implements the AST node classes.
 *
 */

#include "ast_node.hpp"
#include <sstream>

namespace ast_node {
    /*
    =====================
    ||  Superclasses:  ||
    =====================
    */
    std::string ast_node::s_expression() const {
        //  Unused; overridden.
        return "";
    }

    std::string argument_node::s_expression() const {
        //  Unused; overridden.
        return "";
    }

    std::string binding_node::s_expression() const {
        //  Unused; overridden.
        return "";
    }

    std::string cmd_node::s_expression() const {
        //  Unused; overridden.
        return "";
    }

    std::string expr_node::s_expression() const {
        //  Unused; overridden.
        return "";
    }

    std::string lvalue_node::s_expression() const {
        //  Unused; overridden.
        return "";
    }

    std::string stmt_node::s_expression() const {
        //  Unused; overridden.
        return "";
    }

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
    std::string array_argument_node::s_expression() const {
        std::stringstream result;
        result << "(ArrayArgument " << this->main_var.text;
        for (const token::token& tok : vars) { result << " " << tok.text; }
        result << ")";
        return result.str();
    }

    std::string variable_argument_node::s_expression() const { return "(VarArgument " + this->arg_1.text + ")"; }

    //  Bindings:
    //  ---------
    std::string tuple_binding_node::s_expression() const {
        std::stringstream result;
        result << "(TupleBinding";
        for (const std::shared_ptr<binding_node>& binding : args) { result << " " << binding->s_expression(); }
        result << ")";
        return result.str();
    }

    std::string var_binding_node::s_expression() const {
        return "(VarBinding " + arg_arg->s_expression() + " " + type_arg->s_expression() + ")";
    }

    //  Commands:
    //  ---------
    std::string assert_cmd_node::s_expression() const {
        return "(AssertCmd " + this->arg_1->s_expression() + " " + this->arg_2.text + ")";
    }

    std::string fn_cmd_node::s_expression() const {
        std::stringstream result;
        result << "(FnCmd " << name.text << " (";
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

    std::string let_cmd_node::s_expression() const {
        return "(LetCmd " + this->arg_1->s_expression() + " " + this->arg_2->s_expression() + ")";
    }

    std::string print_cmd_node::s_expression() const { return "(PrintCmd \"" + this->arg_1 + "\")"; }

    std::string read_cmd_node::s_expression() const {
        return "(ReadCmd " + this->arg_1.text + " " + this->arg_2->s_expression() + ")";
    }

    std::string show_cmd_node::s_expression() const { return "(ShowCmd " + this->arg_1->s_expression() + ")"; }

    std::string time_cmd_node::s_expression() const { return "(TimeCmd " + this->command->s_expression() + ")"; }

    std::string type_cmd_node::s_expression() const {
        return "(TypeCmd " + this->arg_1.text + " " + this->arg_2->s_expression() + ")";
    }

    std::string write_cmd_node::s_expression() const {
        return "(WriteCmd " + this->arg_1->s_expression() + " " + this->arg_2.text + ")";
    }

    //  Expressions:
    //  ------------
    std::string array_index_expr_node::s_expression() const {
        std::stringstream result;
        result << "(ArrayIndexExpr " << this->array->s_expression();
        for (const std::shared_ptr<expr_node>& param : this->params) { result << " " << param->s_expression(); }
        result << ")";
        return result.str();
    }

    std::string array_literal_expr_node::s_expression() const {
        std::stringstream result;
        result << "(ArrayLiteralExpr";
        for (const std::shared_ptr<expr_node>& expr : this->expressions) { result << " " << expr->s_expression(); }
        result << ")";
        return result.str();
    }

    std::string array_loop_expr_node::s_expression() const {
        std::stringstream result;
        result << "(ArrayLoopExpr";
        for (const array_loop_expr_node::binding_pair_t& binding : this->binding_pairs) {
            result << " " << std::get<0>(binding).text << " " << std::get<1>(binding)->s_expression();
        }
        if (this->item_expr != nullptr) result << " " << this->item_expr->s_expression();
        result << ")";
        return result.str();
    }

    std::string binop_expr_node::s_expression() const {
        std::stringstream result;
        result << "(BinopExpr " << this->left_operand->s_expression() << " ";
        switch (this->type) {
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
                throw std::runtime_error("Unhandled `binop_type` in s-expression");
        }
        result << " " << this->right_operand->s_expression() << ")";
        return result.str();
    }

    std::string call_expr_node::s_expression() const {
        std::stringstream result;
        result << "(CallExpr " << this->name.text;
        for (const std::shared_ptr<expr_node>& arg : this->call_args) { result << " " << arg->s_expression(); }
        result << ")";
        return result.str();
    }

    std::string false_expr_node::s_expression() const { return "(FalseExpr)"; }

    std::string float_expr_node::s_expression() const {
        return "(FloatExpr " + std::to_string((long)this->value) + ")";
    }

    std::string if_expr_node::s_expression() const {
        return "(IfExpr " + this->conditional_expr->s_expression() + " " + this->affirmative_expr->s_expression() + " "
             + this->negative_expr->s_expression() + ")";
    }

    std::string integer_expr_node::s_expression() const { return "(IntExpr " + std::to_string(this->value) + ")"; }

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

    std::string sum_loop_expr_node::s_expression() const {
        std::stringstream result;
        result << "(SumLoopExpr";
        for (const array_loop_expr_node::binding_pair_t& binding : this->binding_pairs) {
            result << " " << std::get<0>(binding).text << " " << std::get<1>(binding)->s_expression();
        }
        if (this->sum_expr != nullptr) result << " " << this->sum_expr->s_expression();
        result << ")";
        return result.str();
    }

    std::string true_expr_node::s_expression() const { return "(TrueExpr)"; }

    std::string tuple_index_expr_node::s_expression() const {
        return "(TupleIndexExpr " + this->expr->s_expression() + " " + std::to_string(index->value) + ")";
    }

    std::string tuple_literal_expr_node::s_expression() const {
        std::stringstream result;
        result << "(TupleLiteralExpr";
        for (const std::shared_ptr<expr_node>& expr : this->exprs) { result << " " << expr->s_expression(); }
        result << ")";
        return result.str();
    }

    std::string unop_expr_node::s_expression() const {
        std::stringstream result;
        result << "(UnopExpr ";
        switch (this->type) {
            case op_type::UNOP_INV:
                result << "!";
                break;
            case op_type::UNOP_NEG:
                result << "-";
                break;
            default:
                throw std::runtime_error("Unhandled `unop_type` in s-expression");
        }
        result << " " << this->operand->s_expression() << ")";
        return result.str();
    }

    std::string variable_expr_node::s_expression() const { return "(VarExpr " + this->arg_1.text + ")"; }

    //  LValues:
    //  --------
    std::string argument_lvalue_node::s_expression() const { return "(ArgLValue " + this->arg_1->s_expression() + ")"; }

    std::string tuple_lvalue_node::s_expression() const {
        std::stringstream result;
        result << "(TupleLValue";
        for (const std::shared_ptr<lvalue_node>& lvalue : this->lvalues) { result << " " << lvalue->s_expression(); }
        result << ")";
        return result.str();
    }

    //  Statements:
    //  -----------
    std::string assert_stmt_node::s_expression() const {
        return "(AssertStmt " + this->arg_1->s_expression() + " " + this->arg_2.text + ")";
    }

    std::string let_stmt_node::s_expression() const {
        return "(LetStmt " + this->arg_1->s_expression() + " " + this->arg_2->s_expression() + ")";
    }

    std::string return_stmt_node::s_expression() const {
        return "(ReturnStmt " + this->return_val->s_expression() + ")";
    }

    //  Types:
    //  ------
    std::string array_type_node::s_expression() const {
        return "(ArrayType " + main_type->s_expression() + " " + std::to_string(dimensions) + ")";
    }

    std::string bool_type_node::s_expression() const { return "(BoolType)"; }

    std::string float_type_node::s_expression() const { return "(FloatType)"; }

    std::string int_type_node::s_expression() const { return "(IntType)"; }

    std::string tuple_type_node::s_expression() const {
        std::stringstream result;
        result << "(TupleType";
        for (const std::shared_ptr<type_node>& type : this->types) { result << " " << type->s_expression(); }
        result << ")";
        return result.str();
    }

    std::string variable_type_node::s_expression() const { return "(VarType " + this->arg_1.text + ")"; }
}  //  namespace ast_node
