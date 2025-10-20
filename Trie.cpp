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

// Comprueba si en un nodo existe el hijo ch
bool Trie::existsChild(const Node* node, char ch) {
  int idx = ctoi(ch);
  if (idx < 0 or idx >= 26) return false;
  return (node->children[idx] != nullptr);
}
bool Trie::existsChild(const Node* node, int idx) {
  if (idx < 0 or idx >= 26) return false;
  return (node->children[idx] != nullptr);
}

// Comprueba si un nodo tiene hijos
bool Trie::hasChildren(const Node* node) {
  for (const Node* child : node->children) if (child) return true;
  return false;
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
void Trie::insert(const string& word, int line) {
  Node* node = root;

  for (char ch : word) {
    int idx = ctoi(ch);
    if (idx < 0 or idx >= 26) {
      cerr << "Error: la palabra " << word << " no se ha podido insertar porque tiene carácteres no alfabéticos\n";
      return; // ignora palabras con carácteres no alfabéticos
    }
    // Si un nodo no contiene el hijo ch correspondiente, lo crea
    if (not existsChild(node, idx)) node->children[idx] = new Node;
    node = node->children[idx];
  }

  node->lines.push_back(line); // insertar la linea donde aparece
}

// Elimina, si existe, la palabra del Trie (todas las ocurrencias)
bool Trie::eliminate(const string& word) {
  Node* node = root;

  // Path que hay que seguir para encontrar la palabra buscada
  vector<pair<Node*, int>> path; // (parent, idx del hijo tomado)
  path.reserve(word.size());

  // Descenso por el Trie guardando el path
  for (char ch : word) {
    int idx = ctoi(ch);
    if (idx < 0 or idx >= 26) {
      cerr << "Error: la palabra a eliminar " << word << " tiene carácteres no compatibles\n";
      return false; // ignora palabras con carácteres no alfabéticos
    }
    if (not existsChild(node, idx)) return false; // la palabra no existe
    path.push_back(pair(node,idx));
    node = node->children[idx];
  }

  // La palabra debe acabar en un nodo de final de palabra
  if (node->lines.size() == 0) return false;

  // Eliminamos todas las ocurrencias de la palabra
  node->lines.clear();
  node->lines.shrink_to_fit();

  // No hace falta borrar ningún nodo (podar), solo borrar las ocurrencias, cuando:
    //  - la palabra = word es prefijo de otra (tiene hijos)
  if (hasChildren(node)) return true;

  // Ascenso por el Trie borrando nodos innecesarios (poda ascendente) hasta encontrar:
    //  - un nodo con lines.size>0 (palabra más corta que debemos preservar)
    //  - un nodo que tenga algún otro hijo
  for (int i = path.size()-1; i >= 0; --i) {
    Node* parent = path[i].first; // nodo padre
    int idx = path[i].second; // índice del hijo tomado
    Node* curr = parent->children[idx]; // nodo hijo

    // Eliminamos el nodo hijo (liberamos memoria)
    delete parent->children[idx];
    parent->children[idx] = nullptr;

    // Si el nodo padre sigue siendo necesario, dejamos de eliminar nodos
    if (parent->lines.size() > 0 or hasChildren(parent)) return true;
  }

  return true;
}

// Elimina, si existe, la palabra del Trie (solo las ocurrencias de la linea deseada)
bool Trie::eliminate(const string& word, int line) {
  Node* node = root;

  // Path que hay que seguir para encontrar la palabra buscada
  vector<pair<Node*, int>> path; // (parent, idx del hijo tomado)
  path.reserve(word.size());

  // Descenso por el Trie guardando el path
  for (char ch : word) {
    int idx = ctoi(ch);
    if (idx < 0 or idx >= 26) {
      cerr << "Error: la palabra a eliminar " << word << " tiene carácteres no compatibles\n";
      return false; // ignora palabras con carácteres no alfabéticos
    }
    if (not existsChild(node, idx)) return false; // la palabra no existe
    path.push_back(pair(node,idx));
    node = node->children[idx];
  }

  // La palabra debe acabar en un nodo de final de palabra
  if (node->lines.size() == 0) return false;

  // Buscamos y borramos las ocurrencias de la palabra en la línea deseada
  auto [it1, it2] = equal_range(node->lines.begin(), node->lines.end(), line);
  if (it1 != it2) node->lines.erase(it1, it2);
  else return false; // no se ha encontrado la palabra en la línea buscada
  
  // No hace falta borrar ningún nodo (podar) cuando:
    //  - aún quedan ocurrencias de la palabra
    //  - la palabra = word es prefijo de otra (tiene hijos)
  if (node->lines.size() > 0 or hasChildren(node)) return true;
  
  // Ascenso por el Trie borrando nodos innecesarios (poda ascendente) hasta encontrar:
    //  - un nodo con lines.size>0 (palabra más corta que debemos preservar)
    //  - un nodo que tenga algún otro hijo
  for (int i = path.size()-1; i >= 0; --i) {
    Node* parent = path[i].first; // nodo padre
    int idx = path[i].second; // índice del hijo tomado

    // Eliminamos el nodo hijo (liberamos memoria)
    delete parent->children[idx];
    parent->children[idx] = nullptr;

    // Si el nodo padre sigue siendo necesario, dejamos de eliminar nodos
    if (parent->lines.size() > 0 or hasChildren(parent)) return true;
  }

  return true;
}

// Comprueba si el Trie contiene la palabra
vector<int> Trie::search(const string& word, long long&ns) {
  using clock = std::chrono::high_resolution_clock;
  auto t0 = clock::now();
  Node* node = root;

  for (char ch : word) {
    int idx = ctoi(ch);
    if (idx < 0 or idx >= 26) {
      cerr << "Error: la palabra buscada " << word << " tiene carácteres no compatibles\n";
      return vector<int>(); // ignora palabras con carácteres no alfabéticos
    }
    if (not existsChild(node, idx)) return vector<int>(); // la palabra no existe
    node = node->children[idx];
  }

  // Comprueba si la palabra acaba en un nodo de final de palabra
  auto t1 = clock::now();
  ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
  return (node->lines.size() > 0) ? node->lines : vector<int>();
}