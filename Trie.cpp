// Trie.cpp
#include "Trie.hh"
#include <cctype>
#include <stack>
#include <utility>

using namespace std;

/*** helpers estáticos declarados en el .hh ***/
int Trie::ctoi(char ch) {
  unsigned char uch = static_cast<unsigned char>(ch);
  return std::tolower(uch) - 'a';
}

bool Trie::existsChild(const Node* node, char ch) {
  int idx = ctoi(ch);
  if (idx < 0 || idx >= 26) return false;
  return node->children[idx] != nullptr;
}

bool Trie::hasChildren(const Node* node) {
  for (const Node* child : node->children) if (child) return true;
  return false;
}

/*** ctor / dtor ***/
Trie::Trie() : root(new Node) {}

Trie::~Trie() {
  if (!root) return;
  std::stack<Node*> st;
  st.push(root);
  while (!st.empty()) {
    Node* n = st.top(); st.pop();
    for (Node* c : n->children) if (c) st.push(c);
    delete n;
  }
  root = nullptr;
}

/*** operaciones públicas ***/
void Trie::insert(const std::string& word) {
  Node* node = root;
  for (char ch : word) {
    int idx = ctoi(ch);
    if (idx < 0 || idx >= 26) continue; // ignora no letras
    if (!node->children[idx]) node->children[idx] = new Node;
    node = node->children[idx];
  }
  node->terminal += 1;
}

bool Trie::search(const std::string& word) {
  const Node* node = root;
  for (char ch : word) {
    int idx = ctoi(ch);
    if (idx < 0 || idx >= 26) return false;
    if (!node->children[idx]) return false;
    node = node->children[idx];
  }
  return node->terminal > 0;
}

bool Trie::eliminate(const std::string& word) {
  std::vector<std::pair<Node*, int>> path; path.reserve(word.size());
  Node* node = root;
  for (char ch : word) {
    int idx = ctoi(ch);
    if (idx < 0 || idx >= 26) return false;
    if (!node->children[idx]) return false; // no existe
    path.emplace_back(node, idx);
    node = node->children[idx];
  }
  if (node->terminal == 0) return false; // no era palabra completa
  node->terminal -= 1;

  // si siguen habiendo ocurrencias o tiene hijos, no hay poda
  if (node->terminal > 0 || hasChildren(node)) return true;

  // Poda ascendente
  for (int i = (int)path.size()-1; i >= 0; --i) {
    Node* parent = path[i].first;
    int idx = path[i].second;
    delete parent->children[idx];
    parent->children[idx] = nullptr;
    if (parent->terminal > 0 || hasChildren(parent)) break;
  }
  return true;
}