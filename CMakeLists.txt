cmake_minimum_required(VERSION 3.10)
project(jplc)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

#  add_compile_options(-fsanitize=address,undefined -fno-sanitize-recover=address,undefined)
#  add_link_options(-fsanitize=address,undefined -fno-sanitize-recover=address,undefined)

include_directories(".")

add_executable(jplc main.cpp ast_node/ast_node.cpp file/file.cpp lexer/lexer.cpp parser/parser.cpp resolved_type/resolved_type.cpp symbol_table/symbol_table.cpp type_checker/type_checker.cpp)
add_executable(ast_node_tests tests/tests.cpp ast_node/ast_node.tests.cpp ast_node/ast_node.cpp resolved_type/resolved_type.cpp)
add_executable(lexer_tests tests/tests.cpp lexer/lexer.tests.cpp lexer/lexer.cpp file/file.cpp)
add_executable(parser_tests tests/tests.cpp parser/parser.tests.cpp parser/parser.cpp ast_node/ast_node.cpp file/file.cpp lexer/lexer.cpp resolved_type/resolved_type.cpp)
