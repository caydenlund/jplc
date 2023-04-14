TEST=test.jpl

CXX=clang++
CXXFLAGS=-O3 -std=c++17 -Werror -Wall -fsanitize=address,undefined -fno-sanitize-recover=address,undefined -I.

OBJS=main.o ast_node.o ast_node.cp_value.o call_signature.o file.o generator.o lexer.o parser.o resolved_type.o stack_info.o symbol_table.o type_checker.o visitor.o visitor.const_prop_visitor.o

all: run

compile: jplc

jplc: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o jplc

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

ast_node.o: ast_node/ast_node.cpp
	$(CXX) $(CXXFLAGS) -c ast_node/ast_node.cpp -o ast_node.o

ast_node.cp_value.o: ast_node/cp_value.cpp
	$(CXX) $(CXXFLAGS) -c ast_node/cp_value.cpp -o ast_node.cp_value.o

call_signature.o: call_signature/call_signature.cpp
	$(CXX) $(CXXFLAGS) -c call_signature/call_signature.cpp -o call_signature.o

file.o: file/file.cpp
	$(CXX) $(CXXFLAGS) -c file/file.cpp -o file.o

generator.o: generator/generator.cpp
	$(CXX) $(CXXFLAGS) -c generator/generator.cpp -o generator.o

lexer.o: lexer/lexer.cpp
	$(CXX) $(CXXFLAGS) -c lexer/lexer.cpp -o lexer.o

parser.o: parser/parser.cpp
	$(CXX) $(CXXFLAGS) -c parser/parser.cpp -o parser.o

stack_info.o: stack_info/stack_info.cpp
	$(CXX) $(CXXFLAGS) -c stack_info/stack_info.cpp -o stack_info.o

symbol_table.o: symbol_table/symbol_table.cpp
	$(CXX) $(CXXFLAGS) -c symbol_table/symbol_table.cpp -o symbol_table.o

resolved_type.o: resolved_type/resolved_type.cpp
	$(CXX) $(CXXFLAGS) -c resolved_type/resolved_type.cpp -o resolved_type.o

type_checker.o: type_checker/type_checker.cpp
	$(CXX) $(CXXFLAGS) -c type_checker/type_checker.cpp -o type_checker.o

visitor.o: visitor/visitor.cpp
	$(CXX) $(CXXFLAGS) -c visitor/visitor.cpp -o visitor.o

visitor.const_prop_visitor.o: visitor/const_prop_visitor.cpp
	$(CXX) $(CXXFLAGS) -c visitor/const_prop_visitor.cpp -o visitor.const_prop_visitor.o

run: jplc
	./jplc $(TEST) $(FLAGS)

clean:
	rm -f *.o ./jplc
