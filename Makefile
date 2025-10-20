all: clean compile

execute: compile program
	./program

compile: main.o Trie.o
	g++ -o program main.o Trie.o

main.o: main.cpp
	g++ -c main.cpp

Trie.o: Trie.cpp Trie.hh
	g++ -c Trie.cpp

clean: 
	rm -f *.o program