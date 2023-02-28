/**
 * @file token.hpp
 * @package Homework 2
 * @author Cayden Lund (u1182408)
 * @brief Defines the `token` class and derivatives, with associated helpers.
 *
 */

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

namespace token {
    /**
     * @brief Defines all the possible token types.
     *
     */
    enum token_type {
        //  =================
        //  ||  Keywords:  ||
        //  =================
        ARRAY,   //  The keyword "array".
        ASSERT,  //  The keyword "assert".
        BOOL,    //  The keyword "bool".
        ELSE,    //  The keyword "else".
        FALSE,   //  The keyword "false".
        FLOAT,   //  The keyword "float".
        FN,      //  The keyword "fn".
        IF,      //  The keyword "if".
        IMAGE,   //  The keyword "image".
        INT,     //  The keyword "int".
        LET,     //  The keyword "let".
        PRINT,   //  The keyword "print".
        READ,    //  The keyword "read".
        RETURN,  //  The keyword "return".
        SHOW,    //  The keyword "show".
        SUM,     //  The keyword "sum".
        THEN,    //  The keyword "then".
        TIME,    //  The keyword "time".
        TO,      //  The keyword "to".
        TRUE,    //  The keyword "true".
        TYPE,    //  The keyword "type".
        WRITE,   //  The keyword "write".

        //  ====================
        //  ||  Punctuation:  ||
        //  ====================
        COLON,    //  ':'
        LCURLY,   //  '{'
        RCURLY,   //  '}'
        LPAREN,   //  '('
        RPAREN,   //  ')'
        COMMA,    //  ','
        LSQUARE,  //  '['
        RSQUARE,  //  ']'
        EQUALS,   //  '='

        //  ==============
        //  ||  Types:  ||
        //  ==============
        STRING,    //  A string literal.
        INTVAL,    //  An integer literal.
        FLOATVAL,  //  A float literal.
        VARIABLE,  //  A variable.

        //  ======================
        //  ||  Miscellaneous:  ||
        //  ======================
        OP,          //  All arithmetic and boolean operators ('+', '-', etc.).
        NEWLINE,     //  '\n'
        END_OF_FILE  //  The end of the file.
    };

    /**
     * @brief Represents a single token in the JPL program.
     *
     */
    struct token {
        /**
         * @brief The byte position of the start of the token.
         * @details Can be used to get the line and column from the file.
         *
         */
        unsigned int start;

        /**
         * @brief The string text of the token.
         *
         */
        std::string text;

        /**
         * @brief The type of the token.
         *
         */
        token_type type;

        /**
         * @brief `==` operator.
         *
         * @param other The other token to compare.
         * @return True if the two tokens are equal.
         */
        bool operator==(const token& other) const {
            return (this->start == other.start) && (this->text == other.text) && (this->type == other.type);
        }

        /**
         * @brief `!=` operator.
         *
         * @param other The other token to compare.
         * @return True if the two tokens are not equal.
         */
        bool operator!=(const token& other) const { return !(*this == other); }
    };

    /**
     * @brief Given a `token_type`, returns the string representation of that
     *     type.
     *
     * @param type The type to translate to string.
     * @return The string representation of the given type.
     */
    inline std::string token_type_to_string(token_type type) {
        switch (type) {
            case ARRAY:
                return "ARRAY";
            case ASSERT:
                return "ASSERT";
            case BOOL:
                return "BOOL";
            case ELSE:
                return "ELSE";
            case FALSE:
                return "FALSE";
            case FLOAT:
                return "FLOAT";
            case FN:
                return "FN";
            case IF:
                return "IF";
            case IMAGE:
                return "IMAGE";
            case INT:
                return "INT";
            case LET:
                return "LET";
            case PRINT:
                return "PRINT";
            case READ:
                return "READ";
            case RETURN:
                return "RETURN";
            case SHOW:
                return "SHOW";
            case SUM:
                return "SUM";
            case THEN:
                return "THEN";
            case TIME:
                return "TIME";
            case TO:
                return "TO";
            case TRUE:
                return "TRUE";
            case TYPE:
                return "TYPE";
            case WRITE:
                return "WRITE";
            case COLON:
                return "COLON";
            case LCURLY:
                return "LCURLY";
            case RCURLY:
                return "RCURLY";
            case LPAREN:
                return "LPAREN";
            case RPAREN:
                return "RPAREN";
            case COMMA:
                return "COMMA";
            case LSQUARE:
                return "LSQUARE";
            case RSQUARE:
                return "RSQUARE";
            case EQUALS:
                return "EQUALS";
            case STRING:
                return "STRING";
            case INTVAL:
                return "INTVAL";
            case FLOATVAL:
                return "FLOATVAL";
            case VARIABLE:
                return "VARIABLE";
            case OP:
                return "OP";
            case NEWLINE:
                return "NEWLINE";
            case END_OF_FILE:
                return "END_OF_FILE";
            default:
                return "MISSING_TOKEN";
        }
    }

    /**
     * @brief Returns the contents between the quotation marks in a string token.
     *
     * @param string_tok The string token.
     * @return The contents between the quotation marks in a string token.
     */
    inline std::string string_value(const token& tok) {
        if (tok.type != token_type::STRING || tok.text.empty() || tok.text[0] != '"'
            || tok.text[tok.text.size() - 1] != '"')
            throw std::runtime_error("Not a string token: \"" + tok.text + "\"");

        return tok.text.substr(1, tok.text.size() - 2);
    }
}  //  namespace token

#endif
