cmake_minimum_required(VERSION 3.10)
project(jplc)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

include_directories(".")

add_executable(jplc main.cpp ast_node/ast_node.cpp file/file.cpp lexer/lexer.cpp parser/parser.cpp)
add_executable(lexer_tests tests/tests.cpp lexer/lexer.tests.cpp lexer/lexer.cpp file/file.cpp)
add_executable(parser_tests tests/tests.cpp parser/parser.tests.cpp parser/parser.cpp ast_node/ast_node.cpp)
