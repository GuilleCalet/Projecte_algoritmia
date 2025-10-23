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
/*if (!root) return;
  stack<Node*> st;
  st.push(root);
  while (!st.empty()) {
    Node* n = st.top(); st.pop();
    for (Node* c : n->children) if (c) st.push(c);
    delete n;
  }
  root = nullptr;*/
}

// Retorna el vector de ocurrencias (line,pos) de una palabra identificada por id
vector<Match> Trie::occurrences(WordID id) const {
  auto it = table_.find(id);
  return (it == table_.end()) ? vector<Match>() : it->second;
}

// Añade una nueva ocurrencia (line,pos) de la palabra identificada por id
void Trie::add_occur(WordID id, int line, long long pos) {
  table_[id].push_back(Match{line,pos});
}

// Retorna el vector de en que línias aparecen las ocurrencias de la palabra identificada por id
vector<int> Trie::get_lines(WordID id) const {
  auto it = table_.find(id);
  if (it == table_.end()) return {};

  vector<int> out;
  for (Match m : it->second) out.push_back(m.line);
  return out;
}

// Retorna el vector de en que posiciones (generales) aparecen las ocurrencias de la palabra identificada por id
vector<long long> Trie::get_positions(WordID id) const {
  auto it = table_.find(id);
  if (it == table_.end()) return {};

  vector<long long> out;
  for (Match m : it->second) out.push_back(m.pos);
  return out;
}

/*** operaciones públicas ***/
// Inserta la palabra identificada por id en el trie, y añade su ocurrencia (line,pos)
void Trie::insert(const string& word, WordID id, int line, long long pos) {
  Node* node = root;

  for (char ch : word) {
    // Si un nodo no contiene el hijo ch correspondiente, lo crea
    if (node->children.find(ch) == node->children.end()) node->children[ch] = new Node;
    node = node->children[ch];
  }

  node->wordID = id;  
  add_occur(id, line, pos); // Guardar la línea y la posición
}

// Busca la palabra en el trie, y si existe retorna su id (-1 si no existe)
WordID Trie::search(const string& word) const {
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
  return node->wordID;
}

vector<int> Trie::explore_subtree(const string& prefix) {
    vector<int> wordIDs;
    Node* node = root;

    // Buscar el nodo correspondiente al último carácter del prefijo
    for (char ch : prefix) {
        if (node->children.find(ch) == node->children.end()) {
            return {};  // No se encuentra el prefijo en el Trie
        }
        node = node->children.at(ch);
    }

    // Recorrer el subárbol a partir de este nodo
    stack<Node*> nodes;
    nodes.push(node);

    while (!nodes.empty()) {
        Node* currentNode = nodes.top();
        nodes.pop();

        // Si el nodo es un nodo final de palabra, agregar su wordID
        if (currentNode->wordID != -1) {
            wordIDs.push_back(currentNode->wordID);
        }

        // Añadir todos los hijos del nodo actual a la pila
        for (auto& child : currentNode->children) {
            nodes.push(child.second);
        }
    }

    return wordIDs;
}
