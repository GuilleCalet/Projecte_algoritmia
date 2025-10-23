#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>

#include "Match.hh"

using namespace std;

typedef int WordID;

class TernaryTrie {
private:
  struct Node {
    char c = 0;
    Node* lo = nullptr;
    Node* eq = nullptr;
    Node* hi = nullptr;
    int wordID = -1;
  };

  Node* root = nullptr;

  unordered_map<WordID, vector<Match>> table_;

  static Node* make(char ch) { Node* n = new Node; n->c = ch; return n; }
  static void destroy(Node* n) {
    if (!n) return;
    destroy(n->lo);
    destroy(n->eq);
    destroy(n->hi);
    delete n;
  }

  // ====== MÉTRICAS (igual que en Trie) ======
  // recuento de nodos
  size_t count_nodes_rec(const Node* n) const {
    if (!n) return 0;
    return 1 + count_nodes_rec(n->lo)
             + count_nodes_rec(n->eq)
             + count_nodes_rec(n->hi);
  }
  // estimación de memoria
  size_t memory_bytes_rec(const Node* n) const {
    if (!n) return 0;
    size_t bytes = sizeof(*n);                  // char + 3 punteros + int
    bytes += memory_bytes_rec(n->lo);
    bytes += memory_bytes_rec(n->eq);
    bytes += memory_bytes_rec(n->hi);
    return bytes;
  }

  // instrumentación de visitas
  mutable size_t visited_nodes_ = 0;

public:
  TernaryTrie() = default;
  ~TernaryTrie() { destroy(root); root = nullptr; }

  // ---- Postings-like API (idéntica a tus otros tries) ----
  vector<Match> occurrences(WordID id) const {
    auto it = table_.find(id);
    return (it == table_.end()) ? vector<Match>() : it->second;
  }
  void add_occur(WordID id, int line, long long pos) {
    table_[id].push_back(Match{line,pos});
  }
  vector<int> get_lines(WordID id) const {
    auto it = table_.find(id);
    if (it == table_.end()) return {};
    vector<int> out;
    out.reserve(it->second.size());
    for (const auto& m : it->second) out.push_back(m.line);
    return out;
  }
  vector<long long> get_positions(WordID id) const {
    auto it = table_.find(id);
    if (it == table_.end()) return {};
    vector<long long> out;
    out.reserve(it->second.size());
    for (const auto& m : it->second) out.push_back(m.pos);
    return out;
  }

  // ---- núcleo ----
  void insert(const string& word, WordID id, int line, long long pos);
  WordID search(const string& word) const;
  vector<int> explore_subtree(const string& prefix) const;

  // ---- API de métricas (mismo interfaz que Trie) ----
  size_t node_count() const { return count_nodes_rec(root); }
  size_t memory_bytes_estimate() const {
    // incluye todos los nodos + coste del propio objeto (orientativo)
    return memory_bytes_rec(root) + sizeof(*this) - sizeof(root);
  }
  void reset_visited() const { visited_nodes_ = 0; }
  size_t last_visited() const { return visited_nodes_; }
};