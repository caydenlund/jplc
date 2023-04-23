cmake_minimum_required(VERSION 3.10)
project(jplc)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_compile_options(-fsanitize=address,undefined -fno-sanitize-recover=address,undefined)
add_link_options(-fsanitize=address,undefined -fno-sanitize-recover=address,undefined)

include_directories(".")

add_library(ast_node ast_node/ast_node.cpp ast_node/cp_value.cpp ast_node/tc_edge.cpp)
target_link_libraries(ast_node resolved_type)

add_library(call_signature call_signature/call_signature.cpp)
target_link_libraries(ast_node resolved_type)

add_library(file file/file.cpp)

add_library(generator generator/generator.cpp)
target_link_libraries(generator ast_node call_signature stack_info symbol_table)

add_library(lexer lexer/lexer.cpp)
target_link_libraries(lexer file)

add_library(parser parser/parser.cpp)
target_link_libraries(parser ast_node file)

add_library(resolved_type resolved_type/resolved_type.cpp)

add_library(stack_info stack_info/stack_info.cpp)

add_library(symbol_table symbol_table/symbol_table.cpp)

add_library(type_checker type_checker/type_checker.cpp)
target_link_libraries(type_checker ast_node parser resolved_type symbol_table)

add_library(visitor visitor/visitor.cpp visitor/const_prop_visitor.cpp visitor/tensor_contraction_visitor.cpp)
target_link_libraries(visitor ast_node)

add_executable(jplc main.cpp)
target_link_libraries(jplc generator lexer parser type_checker visitor)
