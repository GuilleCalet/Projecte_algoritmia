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
        lexicon_[id] = word;
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

  ++visited_nodes_;                // visitamos root
  while (p) {
    if (s[i] < p->c) {
      p = p->lo;
    } else if (s[i] > p->c) {
      p = p->hi;
    } else {
      // s[i] == p->c
      if (i + 1 == word.size()) {
        return p->wordID;         // -1 si no termina aquí
      }
      p = p->eq;
      ++i;
    }
    if (p) ++visited_nodes_;       // contamos el siguiente nodo al que pasamos
  }
  return -1;
}

vector<int> TernaryTrie::explore_subtree(const string& prefix) const {
  vector<int> wordIDs;
  if (!root) return wordIDs;

  if (prefix.empty()) {
    std::stack<const Node*> st;
    st.push(root);
    while (!st.empty()) {
      auto n = st.top(); st.pop();
      ++visited_nodes_;            // cada nodo extraído se considera visitado
      if (!n) continue;
      if (n->wordID != -1) wordIDs.push_back(n->wordID);
      st.push(n->lo);
      st.push(n->eq);
      st.push(n->hi);
    }
    return wordIDs;
  }

  // bajar hasta el nodo del último carácter del prefijo
  const Node* p = root;
  size_t i = 0;
  ++visited_nodes_;                // visit root
  while (p) {
    if (prefix[i] < p->c) {
      p = p->lo;
    } else if (prefix[i] > p->c) {
      p = p->hi;
    } else {
      if (i + 1 == prefix.size()) break;
      p = p->eq;
      ++i;
    }
    if (p) ++visited_nodes_;       // contamos cada avance
  }
  if (!p) return wordIDs;          // prefijo no presente

  if (p->wordID != -1) wordIDs.push_back(p->wordID);

  // explorar continuaciones bajo 'eq'
  std::stack<const Node*> st;
  st.push(p->eq);
  while (!st.empty()) {
    auto n = st.top(); st.pop();
    ++visited_nodes_;
    if (!n) continue;
    if (n->wordID != -1) wordIDs.push_back(n->wordID);
    st.push(n->lo);
    st.push(n->eq);
    st.push(n->hi);
  }
  return wordIDs;
}