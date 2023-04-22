cmake_minimum_required(VERSION 3.10)
project(jplc)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_compile_options(-fsanitize=address,undefined -fno-sanitize-recover=address,undefined)
add_link_options(-fsanitize=address,undefined -fno-sanitize-recover=address,undefined)

include_directories(".")

add_executable(jplc main.cpp
        ast_node/ast_node.cpp
        ast_node/cp_value.cpp
        call_signature/call_signature.cpp
        file/file.cpp
        generator/generator.cpp
        lexer/lexer.cpp
        parser/parser.cpp
        resolved_type/resolved_type.cpp
        stack_info/stack_info.cpp
        symbol_table/symbol_table.cpp
        type_checker/type_checker.cpp
        visitor/const_prop_visitor.cpp
        visitor/tensor_contraction_visitor.cpp
        visitor/visitor.cpp)
