# Cayden Lund's JPL Compiler

This program implements a compiler of
the [JPL language specification](https://github.com/utah-cs4470-sp23/class).
It's broken up into modules, which are detailed below.

## AST nodes (`ast_node/`):

Each `ast_node::ast_node` class defines a constructor and the function
`std::string s_expression() const`, which returns the s-expression of the
instance as a string.

The s-expression is structured as follows:

    (ASTNodeName Arg1 Arg2)

Each AST node has a name
(derived from its class name---e.g., `AssertCmd`, `LetCmd`)
and up to two arguments.

There are several classes that derive from the base class, `ast_node`, and are
themselves superclasses:

- `argument_node`, which represents an argument.
- `cmd_node`, which represents a command.
- `expr_node`, which represents an expression.
- `lvalue_node`, which represents an l-value.
- `type_node`, which represents a type.

Furthermore, there are several derived classes for each of the superclasses:

- `argument_node`:
    - `variable_argument_node`, which represents an argument that's a variable.
      This class has one argument:
        - `token::token arg_1`: A variable token.
- `cmd_node`:
    - `assert_cmd_node`, which represents an `assert` command.
      This class has two arguments:
        - `expr_node* arg_1`: A smart pointer to an expression.
        - `std::string arg_2`: A string message.
    - `let_cmd_node`, which represents a `let` command.
      This class has two arguments:
        - `lvalue_node* arg_1`: A smart pointer to an l-value node.
        - `expr_node* arg_2`: A smart pointer to an expression node.
    - `print_cmd_node`, which represents a `print` command.
      This class has one argument:
        - `std::string arg_1`: A string message.
    - `read_cmd_node`, which represents a `read` command.
      This class has two arguments:
        - `std::string arg_1`: The file to read.
        - `argument_node* arg_2`: A smart pointer to an argument node.
    - `show_cmd_node`, which represents a `show` command.
      This class has one argument:
        - `expr_node* arg_1`: The expression to show.
    - `type_cmd_node`, which represents a `type` command.
      This class has two arguments:
        - `token::token arg_1`: A variable token.
        - `type_node* arg_2`: A smart pointer to a type node.
    - `write_cmd_node`, which represents a `write` command.
      This class has two arguments:
        - `expr_node* arg_1`: A smart pointer to an expression node.
        - `std::string arg_2`: The file to which to write.
- `expr_node`:
    - `false_expr_node`, which represents the boolean `false` literal.
      This class has no arguments.
    - `float_expr_node`, which represents a floating-point literal.
      This class has one argument:
        - `token::token arg_1`: A floating-point literal token.
    - `integer_expr_node`, which represents an integer literal.
      This class has one argument:
        - `token::token arg_1`: An integer literal token.
    - `true_expr_node`, which represents the boolean `true` literal.
      This class has no arguments.
    - `variable_expr_node`, which represents a variable.
      This class has one argument:
        - `token::token arg_1`: A variable token.
- `lvalue_node`:
    - `argument_lvalue_node`, which represents an argument l-value.
      This class has one argument:
        - `argument_node* arg_1`: A smart pointer to an argument node.
- `type_node`:
    - `bool_type_node`, which represents the `bool` type.
      This class has no arguments.
    - `float_type_node`, which represents the `float` type.
      This class has no arguments.
    - `int_type_node`, which represents the `int` type.
      This class has no arguments.
    - `variable_type_node`, which represents a variable type.
      This class has one argument:
        - `token::token arg_1`: A variable token.

## JPL Examples (`examples/`)

The programs in the `examples/` folder are valid JPL programs.
Each program contains a single function, as follows:

- `blur(pict) : pict` (from `blur.jpl`) blurs the given image.
- `circle(float, float) : pict` (from `circle.jpl`) draws a white circle in the
  middle of a black 800x600 image.
- `gradient(int, int, int, int) : {float,float,float,float}` (
  from `gradient.jpl`) is an instructor-provided example that creates a
  multicolored gradient.
- `invert(pict) : pict` (from `invert.jpl`) inverts colors in the given image.
- `red(int, int) : pict` (from `red.jpl`) creates a red image of the given size.
- `sepia(pict) : pict` (from `sepia.jpl`) converts the given image to
  sepia-tone.
- `subtract(float, float) : float` (from `subtract.jpl`) returns the difference
  of the arguments.

## Lexer (`lexer/`)

The lexer tokenizes an input string.
Call it via the method `lexer::lex_all(std::string filename)`.
The returned list (`lexer::token_list_t`) is a vector of smart pointers to
tokens, ready for use by the parser.

It can be called directly from the command line by using the `-l` flag
(e.g., `./jplc -l input_file.jpl`).
The tokens are then printed to the console.

## Parser (`parser/`)

The parser converts the given list of tokens to a set of AST nodes.
Call it via the method `parser::parse(lexer::token_list_t tokens)`.
The returned list is a vector of smart pointers to `ast_node` instances,
ready for use by the type checker.

It can be called directly from the command line in tandem with the lexer by
using the `-p` flag.
The s-expressions of the AST nodes are then printed to the console.

## Tests (`tests/`)

This module defines some test-related infrastructure.
Individual tests are defined in the specific module that they are testing.

## Token (`token/`)

The `token::token` class of this module defines a single token.
A series of tokens is generated by the lexer and consumed by the parser.

