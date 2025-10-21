#include <iostream>
#include <fstream>
#include <chrono>
#include "RadixTrie.hh"
#include"Trie.hh"

using namespace std;

Trie T; // eleccion:(1)
RadixTrie RT; // eleccion:(2)

int eleccion;

void read_konsole(auto& time) {
  time = 0; // asegurarnos de que hemos reiniciado el timer
  string text;

  int line = 0;
  cout << "Escribe un texto a analizar (puedes detener la entrada escribiendo '/' en una linea individualmente):\n";
  while(getline(cin, text) and text != "/") {
    line++;
    string word;
    for (char ch : text) {
      if (isalpha(ch)) word.push_back(tolower(ch));
      else if (ch == ' ' and word.size() > 0) {
        auto t0 = std::chrono::high_resolution_clock::now();
        (eleccion == 1) ? T.insert(word, line) : RT.insert(word, line);
        auto t1 = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
        time += ns;
        word.clear();
        word.shrink_to_fit();
      }
    }
    
    if (word.size() > 0) {
      auto t0 = std::chrono::high_resolution_clock::now();
      (eleccion == 1) ? T.insert(word, line) : RT.insert(word, line);
      auto t1 = std::chrono::high_resolution_clock::now();
      auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
      time += ns;
    }
  }

  cin.clear();
  return;
}

void read_file(string route, auto& time) {
  time = 0; // asegurarnos de que hemos reiniciado el timer
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
      else if (ch == ' ' and word.size() > 0) {
        auto t0 = std::chrono::high_resolution_clock::now();
        (eleccion == 1) ? T.insert(word, line) : RT.insert(word, line);
        auto t1 = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
        time += ns;
        word.clear();
        word.shrink_to_fit();
      }
    }
    
    if (word.size() > 0) {
      auto t0 = std::chrono::high_resolution_clock::now();
      (eleccion == 1) ? T.insert(word, line) : RT.insert(word, line);
      auto t1 = std::chrono::high_resolution_clock::now();
      auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
      time += ns;
    }
  }

  // Cerramos el fichero
  file.close();
  return;
}

int main(int argc, char* argv[]) {
  cout << "Para usar Basic Trie pulse (1)\n";
  cout << "Para usar Radix Trie pulse (2)\n";
  cout << "Cual quieres usar?: ";
  cin >> eleccion;
  
  if (argc < 2) {
    cout << "Uso: " << argv[0] << " [archivo1.txt]\n";
    char order;
    cout << "Quieres introducir un texto manualmente? [y/n]: ";
    while(cin >> order and tolower(order) != 'y') {
      if (tolower(order) == 'n') return 0;
    }
    auto time = 0; // reiniciar el timer
    read_konsole(time);
    cout << "Generar el arbol  ha tardado " << time << " ns\n";
  }
  else {
    auto time = 0; // reiniciar el timer
    read_file(argv[1], time);
    cout << "Generar el arbol  ha tardado " << time << " ns\n";
  }

  string word;
  cout << "Que palabra quieres consultar (minúsculas)?\n -> "; 
  while(cin >> word) {
    auto t0 = std::chrono::high_resolution_clock::now();
    vector<int> occur = (eleccion == 1) ? T.search(word) : RT.search(word);
    auto t1 = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
    if (occur.size() == 0) cout << "No se ha encontrado la palabra " << word << "!!\n";
    else {
      cout << "La palabra " << word << " se ha encontrado en las líneas: [";
      for (int i = 0; i < occur.size(); i++) {
        if (i > 0) cout << ", ";
        cout << occur[i];
      }
      cout << "] num: " << occur.size() << "\n";
    }
    cout << "La busqueda ha tardado " << time << " ns"<<endl;
    cout << "-> ";
  }
  
  return 0;
}