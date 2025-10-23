// =========================
// RadixTrie.hh
// =========================
#include <string>
#include <vector>
#include <array>
#include <unordered_map>

#include "Match.hh"

using namespace std;

typedef int WordID;

class RadixTrie {
private:
  struct Node; // fwd

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
  
  static inline int ctoi(char ch) {
    unsigned char uch = static_cast<unsigned char>(ch);
    return tolower(uch) - 'a';
  }

  static size_t lcp(const string& a, const string& b); // longest common prefix
  static size_t matchLabel(const string& w, size_t i, const string& label); // longest common prefix


  // Utilidades de gestión de memoria/estructura
  static void destroy(Node* n);
  static bool hasChild(const Node* n);

public:
  RadixTrie();
  ~RadixTrie();

  // Retorna el vector de ocurrencias (line,pos) de una palabra identificada por id
  vector<Match> occurrences(WordID id) const;

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
};
