
all: clean compile

execute: compile program
	./program

compile: main.o Trie.o RadixTrie.o
	g++ -o program main.o Trie.o RadixTrie.o
main.o: main.cpp
	g++ -c main.cpp

Trie.o: Trie.cpp Trie.hh
	g++ -c Trie.cpp

RadixTrie.o: RadixTrie.cc RadixTrie.hh
	g++ -c RadixTrie.cc
clean: 
	rm -f *.o program

