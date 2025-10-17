#include <iostream>
#include <fstream>
#include "Trie.hh"

using namespace std;

Trie T;

void read_konsole() {
  string word;

  cout << "Escribe un texto a analizar (puedes detener la entrada con '/'):\n";
  char ch;
  while(cin.get(ch) and ch != '/') {
    if (isalpha(ch)) word.push_back(tolower(ch));
    else if (word.size() > 0) {
      T.insert(word);
      word.clear();
      word.shrink_to_fit();
    }
  }
  if (word.size() > 0) T.insert(word);

  cin.clear();
  return;
}

void read_file(string route) {
  ifstream file;
  string word;

  file.open(route, ios::in); // Obrim el fitxer adjunt per llegir-lo

  if (file.fail()) { // Comprovació d'errors
    cout << "Error: el archivo no se ha podido abrir" << endl;
    exit(1);
  }

  char ch;
  while (file.get(ch) and not file.eof()) { // Llegim tot el fitxer per carácters
    if (isalpha(ch)) word.push_back(tolower(ch));
    else if (word.size() > 0) {
      T.insert(word);
      word.clear();
      word.shrink_to_fit();
    }
  }

  file.close(); // Tanquem el fitxer
  return;
}

int main(int argc, char* argv[]) {
  T = Trie();

  if (argc < 2) {
    cout << "Uso: " << argv[0] << " [archivo1.txt] [archivo2.txt ...]\n";
    
    char order;
    cout << "Quieres introducir un texto manualmente? [y/n]: ";
    while(cin >> order and tolower(order) != 'y') {
      if (tolower(order) == 'n') return 0;
    }

    read_konsole();
  }
  else for (int i = 1; i < argc; i++) read_file(argv[i]);

  string word;
  cout << "Que palabra quieres consultar (minúsculas)?\n"; 
  while(cin >> word) {
    if (T.search(word)) cout << "true\n";
    else cout << "false\n";
  }
  
  return 0;
}