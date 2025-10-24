#pragma once
#include "Match.hh"


typedef int WordID;

class RadixTrie {
private:
  struct Node; 

  // Estructura de una arista del Trie
  struct Edge {
    string label; // segmento comprimido del camino
    Node* child = nullptr; // puntero al nodo que une (hijo)
  };

  // Estructura de un nodo del Trie
  struct Node {
    unordered_map<char, Edge*> edges; // punteros a las aristas que salen del nodo
    int wordID = -1; // identificador de la palabra (-1 si no es un nodo de final de palabra)
  };

  Node* root = nullptr; // raíz del Trie

  unordered_map<WordID, vector<Match>> table_; // tabla de tuplas (line,pos) para cada ocurrencia de cada palabra del texto
  
  //diccionario id -> palabra
  unordered_map<WordID, string> lexicon_;

  size_t count_nodes_rec(const Node* n) const;
  size_t memory_bytes_rec(const Node* n) const;

  // visitas por operación
  mutable size_t visited_nodes_ = 0;

public:
  RadixTrie();


  // Añade una nueva ocurrencia (line,pos) de la palabra identificada por id
  void add_occur(WordID id, int line, long long pos);

  // Retorna el vector de en que línias aparecen las ocurrencias de la palabra identificada por id
  vector<int> get_lines(WordID id) const;

  // Retorna el vector de en que posiciones (generales) aparecen las ocurrencias de la palabra identificada por id
  vector<long long> get_positions(WordID id) const;

  // Inserta la palabra identificada por id en el trie, y añade su ocurrencia (line,pos)
  void insert(const string& word, WordID id, int line, long long pos);

  // Busca la palabra en el trie, y si existe retorna su id (-1 si no existe)
  WordID search(const string& word) const;

  vector<int> explore_subtree(const string& prefix) const;

  vector<WordID> complete_prefix_topk(const string& prefix, size_t k) const;


  size_t node_count() const;
  size_t memory_bytes_estimate() const;

  string word_of(WordID id) const {
    auto it = lexicon_.find(id);
    return (it == lexicon_.end()) ? string() : it->second;
  }

  void reset_visited() const { visited_nodes_ = 0; }
  size_t last_visited() const { return visited_nodes_; }
};
