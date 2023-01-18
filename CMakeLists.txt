cmake_minimum_required(VERSION 3.10)
project(jplc)

set(CMAKE_CXX_STANDARD 17)

include_directories(".")

add_executable(jplc compiler.cpp lexer/lexer.cpp)
add_executable(lexer_tests tests/tests.cpp lexer/lexer.tests.cpp lexer/lexer.cpp)
