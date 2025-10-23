#include "TernaryTrie.hh"
#include <algorithm>

using namespace std;

// Inserta la palabra y añade su ocurrencia
void TernaryTrie::insert(const string& word, WordID id, int line, long long pos) {
  if (word.empty()) return;

  const char* s = word.c_str();
  size_t i = 0;

  if (!root) root = make(s[0]);
  Node* p = root;

  while (true) {
    if (s[i] < p->c) {
      if (!p->lo) p->lo = make(s[i]);
      p = p->lo;
    } else if (s[i] > p->c) {
      if (!p->hi) p->hi = make(s[i]);
      p = p->hi;
    } else {
      // s[i] == p->c
      if (i + 1 == word.size()) {
        // último carácter de la palabra
        p->wordID = id;
        add_occur(id, line, pos);
        return;
      }
      if (!p->eq) p->eq = make(s[i + 1]);
      p = p->eq;
      ++i;
    }
  }
}

// Busca la palabra y retorna su wordID (-1 si no está)
WordID TernaryTrie::search(const string& word) const {
  if (!root || word.empty()) return -1;

  const char* s = word.c_str();
  size_t i = 0;
  const Node* p = root;

  while (p) {
    if (s[i] < p->c) {
      p = p->lo;
    } else if (s[i] > p->c) {
      p = p->hi;
    } else {
      // s[i] == p->c
      if (i + 1 == word.size()) {
        return p->wordID; // -1 si no era final de palabra
      }
      p = p->eq;
      ++i;
    }
  }
  return -1;
}

// Recorre el subárbol a partir del nodo que corresponde al prefijo
vector<int> TernaryTrie::explore_subtree(const string& prefix) const {
  vector<int> wordIDs;
  if (!root) return wordIDs;
  if (prefix.empty()) {
    // Prefijo vacío: devolver todas las palabras del trie
    // DFS desde root, acumulando wordID de cada nodo terminal
    std::stack<const Node*> st;
    st.push(root);
    while (!st.empty()) {
      auto n = st.top(); st.pop();
      if (!n) continue;
      if (n->wordID != -1) wordIDs.push_back(n->wordID);
      st.push(n->lo);
      st.push(n->eq);
      st.push(n->hi);
    }
    return wordIDs;
  }

  // 1) Descender hasta el nodo del último carácter del prefijo
  const Node* p = root;
  size_t i = 0;
  while (p) {
    if (prefix[i] < p->c) {
      p = p->lo;
    } else if (prefix[i] > p->c) {
      p = p->hi;
    } else {
      // letra coincide
      if (i + 1 == prefix.size()) break; // 'p' es el nodo del último char del prefijo
      p = p->eq;
      ++i;
    }
  }
  if (!p) return wordIDs; // prefijo no presente

  // 2) Si el prefijo en sí es palabra, incluir su wordID
  if (p->wordID != -1) wordIDs.push_back(p->wordID);

  // 3) Recorrer todo el subárbol bajo el hijo 'eq' (continuaciones del prefijo)
  std::stack<const Node*> st;
  st.push(p->eq);
  while (!st.empty()) {
    auto n = st.top(); st.pop();
    if (!n) continue;
    if (n->wordID != -1) wordIDs.push_back(n->wordID);
    // DFS ternario
    st.push(n->lo);
    st.push(n->eq);
    st.push(n->hi);
  }
  return wordIDs;
}
