// Trie.cpp
#include "Trie.hh"
#include <cctype>
#include <algorithm>
#include <stack>
#include<chrono>
#include <utility>

using namespace std;

// Convierte un carácter en un índice normalizado (normaliza a minúsculas)
int Trie::ctoi(char ch) {
  unsigned char uch = static_cast<unsigned char>(ch);
  return tolower(uch) - 'a';
}

// Constructor
Trie::Trie() : root(new Node) {}

// Destructor
Trie::~Trie() {
  if (!root) return;
  stack<Node*> st;
  st.push(root);
  while (!st.empty()) {
    Node* n = st.top(); st.pop();
    for (Node* c : n->children) if (c) st.push(c);
    delete n;
  }
  root = nullptr;
}

/*** operaciones públicas ***/
// Inserta la palabra en el Trie
void Trie::insert(const string& word, int ID) {
  Node* node = root;

  for (char ch : word) {
    int idx = ctoi(ch);
    if (idx < 0 or idx >= 26) {
      cerr << "Error: la palabra " << word << " no se ha podido insertar porque tiene carácteres no alfabéticos\n";
      return; // ignora palabras con carácteres no alfabéticos
    }
    // Si un nodo no contiene el hijo ch correspondiente, lo crea
    if (node->children.find(ch) == node->children.end()) node->children[ch] = new Node;
    node = node->children[ch];
  }

  node->wordID = ID;  // Guardar la línea y la posición
}

// Comprueba si el Trie contiene la palabra
int Trie::search(const string& word, long long&ns) {
  using clock = std::chrono::high_resolution_clock;
  auto t0 = clock::now();
  Node* node = root;

  for (char ch : word) {
    int idx = ctoi(ch);
    if (idx < 0 or idx >= 26) {
      cerr << "Error: la palabra buscada " << word << " tiene carácteres no compatibles\n";
      return -1; // ignora palabras con carácteres no alfabéticos
    }
    if (node->children.find(ch) == node->children.end()) return -1; // la palabra no existe
    node = node->children.at(ch);
  }

  // Comprueba si la palabra acaba en un nodo de final de palabra
  auto t1 = clock::now();
  ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
  return node->wordID;  // Retornar las posiciones (líneas y posiciones dentro de las líneas)
}