/**
 * @file ast_node.cpp
 * @package Assignment 3
 * @author Cayden Lund (u1182408)
 * @brief Implements the AST node classes.
 *
 */

#include "ast_node.hpp"

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

    std::string type_node::s_expression() const {
        //  Unused; overridden.
        return "";
    }

    std::string variable_argument_node::s_expression() const { return "(VarArgument" + this->variable + ")"; }

    std::string read_cmd_node::s_expression() const {
        return "(ReadCmd " + this->arg_1.text + get_s_expression(this->arg_2) + ")";
    }

    std::string write_cmd_node::s_expression() const {
        return "(WriteCmd " + get_s_expression(this->arg_1) + " " + this->arg_2.text + ")";
    }

    std::string type_cmd_node::s_expression() const {
        return "(TypeCmd " + this->arg_1.text + " " + get_s_expression(this->arg_2) + ")";
    }

    std::string let_cmd_node::s_expression() const {
        return "(LetCmd " + get_s_expression(this->arg_1) + " " + get_s_expression(this->arg_2) + ")";
    }

    std::string assert_cmd_node::s_expression() const {
        return "(AssertCmd " + get_s_expression(this->arg_1) + " " + this->arg_2.text + ")";
    }

    std::string print_cmd_node::s_expression() const { return "(PrintCmd " + this->arg_1.text + ")"; }

    std::string show_cmd_node::s_expression() const { return "(ShowCmd " + get_s_expression(this->arg_1) + ")"; }

    std::string integer_expr_node::s_expression() const {
        return "(IntExpr " + std::to_string(this->arg_1.value) + ")";
    }

    std::string float_expr_node::s_expression() const {
        return "(FloatExpr " + std::to_string((long)this->arg_1.value) + ")";
    }

    std::string true_expr_node::s_expression() const { return "(TrueExpr)"; }

    std::string false_expr_node::s_expression() const { return "(FalseExpr)"; }

    std::string variable_expr_node::s_expression() const { return "(VarExpr " + this->arg_1.text + ")"; }

    std::string argument_lvalue_node::s_expression() const {
        return "(ArgLValue " + get_s_expression(this->arg_1) + ")";
    }

    std::string int_type_node::s_expression() const { return "(IntType)"; }

    std::string bool_type_node::s_expression() const { return "(BoolType)"; }

    std::string float_type_node::s_expression() const { return "(FloatType)"; }

    std::string variable_type_node::s_expression() const { return "(VarType " + this->arg_1.text + ")"; }

    std::string get_s_expression(const std::shared_ptr<ast_node>& node) {
        switch (node->type) {
            //  Arguments:
            //  ----------
            case ARGUMENT:  //  AST node `argument_node`.
                return (std::reinterpret_pointer_cast<argument_node>(node))->s_expression();
            case VARIABLE_ARGUMENT:  //  AST node `variable_argument_node`.
                return (std::reinterpret_pointer_cast<variable_argument_node>(node))->s_expression();

            //  Commands:
            //  ---------
            case CMD:  //  AST node `cmd_node`.
                return (std::reinterpret_pointer_cast<cmd_node>(node))->s_expression();
            case READ_CMD:  //  AST node `read_cmd_node`.
                return (std::reinterpret_pointer_cast<read_cmd_node>(node))->s_expression();
            case WRITE_CMD:  //  AST node `write_cmd_node`.
                return (std::reinterpret_pointer_cast<write_cmd_node>(node))->s_expression();
            case TYPE_CMD:  //  AST node `type_cmd_node`.
                return (std::reinterpret_pointer_cast<type_cmd_node>(node))->s_expression();
            case LET_CMD:  //  AST node `let_cmd_node`.
                return (std::reinterpret_pointer_cast<let_cmd_node>(node))->s_expression();
            case ASSERT_CMD:  //  AST node `assert_cmd_node`.
                return (std::reinterpret_pointer_cast<assert_cmd_node>(node))->s_expression();
            case PRINT_CMD:  //  AST node `print_cmd_node`.
                return (std::reinterpret_pointer_cast<print_cmd_node>(node))->s_expression();
            case SHOW_CMD:  //  AST node `show_cmd_node`.
                return (std::reinterpret_pointer_cast<show_cmd_node>(node))->s_expression();

            //  Expressions:
            //  ------------
            case EXPR:  //  AST node `expr_node`.
                return (std::reinterpret_pointer_cast<expr_node>(node))->s_expression();
            case INTEGER_EXPR:  //  AST node `integer_expr_node`.
                return (std::reinterpret_pointer_cast<integer_expr_node>(node))->s_expression();
            case FLOAT_EXPR:  //  AST node `float_expr_node`.
                return (std::reinterpret_pointer_cast<float_expr_node>(node))->s_expression();
            case TRUE_EXPR:  //  AST node `true_expr_node`.
                return (std::reinterpret_pointer_cast<true_expr_node>(node))->s_expression();
            case FALSE_EXPR:  //  AST node `false_expr_node`.
                return (std::reinterpret_pointer_cast<false_expr_node>(node))->s_expression();
            case VARIABLE_EXPR:  //  AST node `variable_expr_node`.
                return (std::reinterpret_pointer_cast<variable_expr_node>(node))->s_expression();

            //  Lvalues:
            //  --------
            case LVALUE:  //  AST node `lvalue_node`.
                return (std::reinterpret_pointer_cast<lvalue_node>(node))->s_expression();
            case ARGUMENT_LVALUE:  //  AST node `argument_lvalue_node`.
                return (std::reinterpret_pointer_cast<argument_lvalue_node>(node))->s_expression();

            //  Types:
            //  ------
            case TYPE:  //  AST node `type_node`.
                return (std::reinterpret_pointer_cast<type_node>(node))->s_expression();
            case INT_TYPE:  //  AST node `int_type_node`.
                return (std::reinterpret_pointer_cast<int_type_node>(node))->s_expression();
            case BOOL_TYPE:  //  AST node `bool_type_node`.
                return (std::reinterpret_pointer_cast<bool_type_node>(node))->s_expression();
            case FLOAT_TYPE:  //  AST node `float_type_node`.
                return (std::reinterpret_pointer_cast<float_type_node>(node))->s_expression();
            case VARIABLE_TYPE:  //  AST node `variable_type_node`.
                return (std::reinterpret_pointer_cast<variable_type_node>(node))->s_expression();
            default:
                throw std::runtime_error("Attempting to call `s_expression` on an unimplemented node type");
        }
    }
}  //  namespace ast_node
