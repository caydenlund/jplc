TEST=test.jpl

CXX=clang++
CXXFLAGS=-O3 -std=c++17 -Werror -Wall -fsanitize=address,undefined -fno-sanitize-recover=address,undefined -I.

OBJS=main.o ast_node.o file.o lexer.o parser.o resolved_type.o symbol_table.o type_checker.o

all: run

compile: jplc

jplc: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o jplc

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

ast_node.o: ast_node/ast_node.cpp
	$(CXX) $(CXXFLAGS) -c ast_node/ast_node.cpp -o ast_node.o

file.o: file/file.cpp
	$(CXX) $(CXXFLAGS) -c file/file.cpp -o file.o

lexer.o: lexer/lexer.cpp
	$(CXX) $(CXXFLAGS) -c lexer/lexer.cpp -o lexer.o

parser.o: parser/parser.cpp
	$(CXX) $(CXXFLAGS) -c parser/parser.cpp -o parser.o

symbol_table.o: symbol_table/symbol_table.cpp
	$(CXX) $(CXXFLAGS) -c symbol_table/symbol_table.cpp -o symbol_table.o

resolved_type.o: resolved_type/resolved_type.cpp
	$(CXX) $(CXXFLAGS) -c resolved_type/resolved_type.cpp -o resolved_type.o

type_checker.o: type_checker/type_checker.cpp
	$(CXX) $(CXXFLAGS) -c type_checker/type_checker.cpp -o type_checker.o

run: jplc
	./jplc $(TEST) $(FLAGS)

clean:
	rm -f *.o ./jplc
