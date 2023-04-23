TEST=test.jpl

CXX=clang++
CXXFLAGS=-g -Og -std=c++17 -Werror -Wall -fsanitize=address,undefined -fno-sanitize-recover=address,undefined -I.

BUILD_DIR = ./build

AST_NODE_SRCS := ast_node/ast_node.cpp ast_node/cp_value.cpp ast_node/tc_edge.cpp
CALL_SIGNATURE_SRCS := call_signature/call_signature.cpp
FILE_SRCS := file/file.cpp
GENERATOR_SRCS := generator/generator.cpp
LEXER_SRCS := lexer/lexer.cpp
PARSER_SRCS := parser/parser.cpp
RESOLVED_TYPE_SRCS := resolved_type/resolved_type.cpp
STACK_INFO_SRCS := stack_info/stack_info.cpp
SYMBOL_TABLE_SRCS := symbol_table/symbol_table.cpp
TYPE_CHECKER_SRCS := type_checker/type_checker.cpp
VISITOR_SRCS := visitor/visitor.cpp visitor/const_prop_visitor.cpp visitor/tensor_contraction_visitor.cpp

SRCS := $(AST_NODE_SRCS) $(CALL_SIGNATURE_SRCS) $(FILE_SRCS) $(GENERATOR_SRCS) $(LEXER_SRCS) $(PARSER_SRCS) \
$(RESOLVED_TYPE_SRCS) $(STACK_INFO_SRCS) $(SYMBOL_TABLE_SRCS) $(TYPE_CHECKER_SRCS) $(VISITOR_SRCS) main.cpp

OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))

all: run

compile: jplc

jplc: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o jplc

$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: jplc
	./jplc $(TEST) $(FLAGS)

clean:
	rm -rf $(BUILD_DIR) ./jplc
