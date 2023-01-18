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
    };

    /**
     * @brief Represents a `float` literal.
     *
     */
    struct float_token : token {
        /**
         * @brief The (floating-point) value of the number.
         * @details Initialized to 0.
         *
         */
        double value {};
    };

    /**
     * @brief Represents an `int` literal.
     *
     */
    struct int_token : token {
        /**
         * @brief The (integer) value of the number.
         * @details Initialized to 0.
         *
         */
        long value {};
    };

    /**
     * @brief Represents a string literal.
     *
     */
    struct string_token : token {
        /**
         * @brief The (string) value of the token.
         *
         */
        std::string value;
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
}  //  namespace token

#endif
