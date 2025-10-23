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

std::vector<WordID> TernaryTrie::complete_prefix_topk(const std::string& prefix, size_t k) const {
    std::vector<WordID> out;
    visited_nodes_ = 0;

    // 1) Descenso al último carácter del prefijo
    const Node* n = root;
    size_t i = 0;
    while (n && i < prefix.size()) {
        ++visited_nodes_;
        if (prefix[i] < n->c)      n = n->lo;
        else if (prefix[i] > n->c) n = n->hi;
        else {
            if (i + 1 == prefix.size()) break;  // 'n' es el nodo del último char del prefijo
            n = n->eq; ++i;
        }
    }
    if (!n || i + 1 != prefix.size()) return out;

    // 2) BFS por capas: capa 1 comienza en n->eq
    struct Item { const Node* node; int depth; };
    std::queue<Item> q;
    if (n->eq) q.push({n->eq, 1});

    while (!q.empty() && out.size() < k) {
        Item cur = q.front(); q.pop();
        ++visited_nodes_;

        if (cur.node->wordID != -1) {
            out.push_back(cur.node->wordID);
            if (out.size() >= k) break;
        }
        // Misma profundidad: lo / hi
        if (cur.node->lo) q.push({cur.node->lo, cur.depth});
        if (cur.node->hi) q.push({cur.node->hi, cur.depth});
        // Profundidad siguiente: eq
        if (cur.node->eq) q.push({cur.node->eq, cur.depth + 1});
    }

    return out;
}