TEST=test.jpl

CXX=clang++
CXXFLAGS=-Og -std=c++17 -Werror -Wall -fsanitize=address,undefined -fno-sanitize-recover=address,undefined -I.

all: run

compile: compiler.o

compiler.o: compiler.cpp
	$(CXX) $(CXXFLAGS) -c compiler.cpp -o compiler.o

lexer.o: lexer/lexer.cpp
	$(CXX) $(CXXFLAGS) -c lexer/lexer.cpp -o lexer.o

jplc: compiler.o lexer.o
	$(CXX) $(CXXFLAGS) compiler.o -o jplc

run: jplc
	./jplc $(TEST)

clean:
	rm -f *.o ./jplc
