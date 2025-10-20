#include <iostream>
#include <fstream>
#include "Trie.hh"

using namespace std;

Trie T;

/*void read_konsole() {
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
}*/

void read_konsole() {
  string text;
  int line = 0;

  cout << "Escribe un texto a analizar (puedes detener la entrada escribiendo '/' en una linea individualmente):\n";
  while(getline(cin, text) and text != "/") {
    line++;
    string word;

    for (char ch : text) {
      if (isalpha(ch)) word.push_back(tolower(ch));
      else if (ch = ' ' and word.size() > 0) {
        T.insert(word, line);
        word.clear();
        word.shrink_to_fit();
      }
    }
    
    if (word.size() > 0) T.insert(word, line);
  }

  cin.clear();
  return;
}

/*void read_file(string route) {
  ifstream file;
  string word;

  // Abrimos el fichero adjunto para leerlo
  file.open(route, ios::in); 

  if (file.fail()) { // comprobación de errores al abrir el fichero
    cout << "Error: el archivo no se ha podido abrir" << endl;
    exit(1);
  }

  // Leemos todo el fichero por carácteres
  char ch;
  while (file.get(ch) and not file.eof()) { 
    if (isalpha(ch)) word.push_back(tolower(ch));
    else if (word.size() > 0) {
      T.insert(word);
      word.clear();
      word.shrink_to_fit();
    }
  }

  // Cerramos el fichero
  file.close(); 
  return;
}*/

void read_file(string route) {
  ifstream file;

  // Abrimos el fichero adjunto para leerlo
  file.open(route, ios::in); 

  if (file.fail()) { // comprobación de errores al abrir el fichero
    cout << "Error: el archivo no se ha podido abrir" << endl;
    exit(1);
  }

  // Leemos todo el fichero por líneas
  string text;
  int line = 0;
  while(getline(file, text)) {
    line++;
    string word;

    for (char ch : text) {
      if (isalpha(ch)) word.push_back(tolower(ch));
      else if (ch = ' ' and word.size() > 0) {
        T.insert(word, line);
        word.clear();
        word.shrink_to_fit();
      }
    }
    
    if (word.size() > 0) T.insert(word, line);
  }

  // Cerramos el fichero
  file.close(); 
  return;
}

int main(int argc, char* argv[]) {
  
  if (argc < 2) {
    cout << "Uso: " << argv[0] << " [archivo1.txt]\n";

    char order;
    cout << "Quieres introducir un texto manualmente? [y/n]: ";
    while(cin >> order and tolower(order) != 'y') {
      if (tolower(order) == 'n') return 0;
    }

    read_konsole();
  }
  else read_file(argv[1]);

  string word;
  cout << "Que palabra quieres consultar (minúsculas)?\n -> "; 
  while(cin >> word) {
    vector<int> occur = T.search(word);
    if (occur.size() == 0) cout << "No se ha encontrado la palabra " << word << "!!\n";
    else {
      cout << "La palabra " << word << " se ha encontrado en las líneas: [";
      for (int i = 0; i < occur.size(); i++) {
        if (i > 0) cout << ", ";
        cout << occur[i];
      }
      cout << "] num: " << occur.size() << "\n";
    }
    cout << "-> ";
  }
  
  return 0;
}