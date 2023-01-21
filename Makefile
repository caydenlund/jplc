TEST=test.jpl

CXX=clang++
CXXFLAGS=-Og -std=c++17 -Werror -Wall -fsanitize=address,undefined -fno-sanitize-recover=address,undefined -I.

OBJS=main.o file.o lexer.o

all: run

compile: jplc

jplc: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o jplc

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

file.o: file/file.cpp
	$(CXX) $(CXXFLAGS) -c file/file.cpp -o file.o

lexer.o: lexer/lexer.cpp
	$(CXX) $(CXXFLAGS) -c lexer/lexer.cpp -o lexer.o

run: jplc
	./jplc $(TEST)

clean:
	rm -f *.o ./jplc
