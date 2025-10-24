#pragma once
#include "Match.hh"

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
  unordered_map<WordID, string> lexicon_;
  static Node* make(char ch) { Node* n = new Node; n->c = ch; return n; }


  size_t count_nodes_rec(const Node* n) const;
  size_t memory_bytes_rec(const Node* n) const;

  mutable size_t visited_nodes_ = 0;

public:
  TernaryTrie() = default;
 

  void add_occur(WordID id, int line, long long pos);
  vector<int> get_lines(WordID id) const;
  vector<long long> get_positions(WordID id) const;


  void insert(const string& word, WordID id, int line, long long pos);
  WordID search(const string& word) const;
  vector<int> explore_subtree(const string& prefix) const;

  vector<WordID> complete_prefix_topk(const string& prefix, size_t k) const;


  size_t node_count() const { return count_nodes_rec(root); }
  size_t memory_bytes_estimate() const;
  void reset_visited() const { visited_nodes_ = 0; }
  size_t last_visited() const { return visited_nodes_; }

  string word_of(WordID id) const {
    auto it = lexicon_.find(id);
    return (it == lexicon_.end()) ? string() : it->second;
  }
};