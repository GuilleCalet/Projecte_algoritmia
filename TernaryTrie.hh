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
    int wordID = -1;  // -1 si no termina palabra aquí
  };

  Node* root = nullptr;

  unordered_map<WordID, vector<Match>> table_; // (line,pos) por palabra

  static Node* make(char ch) { Node* n = new Node; n->c = ch; return n; }
  static void destroy(Node* n) {
    if (!n) return;
    destroy(n->lo);
    destroy(n->eq);
    destroy(n->hi);
    delete n;
  }

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

  // Devuelve todos los wordID bajo el prefijo (incluye el del prefijo si es palabra completa)
  vector<int> explore_subtree(const string& prefix) const;
};
