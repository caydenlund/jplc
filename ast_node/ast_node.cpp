/**
 * @file ast_node.cpp
 * @package Assignment 3
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
        if (this->bindings.size() > 0) {
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

    std::string integer_expr_node::s_expression() const { return "(IntExpr " + std::to_string(this->value) + ")"; }

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
