all: clean compile

compile: main.o Trie.o RadixTrie.o TernaryTrie.o
	g++ -o program main.o Trie.o RadixTrie.o TernaryTrie.o

main.o: main.cpp
	g++ -c main.cpp

Trie.o: Trie.cpp Trie.hh
	g++ -c Trie.cpp

RadixTrie.o: RadixTrie.cpp RadixTrie.hh
	g++ -c RadixTrie.cpp

TernaryTrie.o: TernaryTrie.cpp TernaryTrie.hh
	g++ -c TernaryTrie.cpp

clean: 
	rm -f *.o program

