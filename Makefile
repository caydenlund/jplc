TEST=test.jpl

CXX=clang++
CXXFLAGS=-g -Og -std=c++17 -Werror -Wall -fsanitize=address,undefined -fno-sanitize-recover=address,undefined -I.

AST_NODE_OBJS := ast_node/ast_node.o ast_node/cp_value.o ast_node/tc_edge.o
CALL_SIGNATURE_OBJS := call_signature/call_signature.o
FILE_OBJS := file/file.o
GENERATOR_OBJS := generator/generator.o
LEXER_OBJS := lexer/lexer.o
PARSER_OBJS := parser/parser.o
RESOLVED_TYPE_OBJS := resolved_type/resolved_type.o
STACK_INFO_OBJS := stack_info/stack_info.o
SYMBOL_TABLE_OBJS := symbol_table/symbol_table.o
TYPE_CHECKER_OBJS := type_checker/type_checker.o
VISITOR_OBJS := visitor/visitor.o visitor/const_prop_visitor.o visitor/tensor_contraction_visitor.o

OBJS := $(AST_NODE_OBJS) $(CALL_SIGNATURE_OBJS) $(FILE_OBJS) $(GENERATOR_OBJS) $(LEXER_OBJS) $(PARSER_OBJS) \
$(RESOLVED_TYPE_OBJS) $(STACK_INFO_OBJS) $(SYMBOL_TABLE_OBJS) $(TYPE_CHECKER_OBJS) $(VISITOR_OBJS) main.o

all: run

compile: jplc

jplc: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o jplc

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: jplc
	./jplc $(TEST) $(FLAGS)

clean:
	rm -f ./main.o ./*/*.o ./jplc
