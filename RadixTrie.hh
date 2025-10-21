// =========================
// RadixTrie.hh
// =========================
#pragma once
#include <string>
#include <vector>
#include <array>

using namespace std;

class RadixTrie {
private:

  struct Node; // fwd
  struct Edge {
    string label; // segmento comprimido del camino
    Node* child = nullptr;
  };
  struct Node {
    // Como en el trie original, guardamos en los nodos finales el vector de líneas
    vector<int> lines;           
    // En un radix trie, desde un nodo no puede haber dos aristas que empiecen por la misma letra
    // Usamos array indexado por 'a'..'z' para acceso O(1)
    array<Edge*,26> edges{};     // punteros nulos por defecto
  };

  Node* root = nullptr;


  static inline int ctoi(char ch) {
    unsigned char uch = static_cast<unsigned char>(ch);
    return tolower(uch) - 'a';
  }
  static size_t lcp(const string& a, const string& b); // longest common prefix
  
  // Utilidades de gestión de memoria/estructura
  static void destroy(Node* n);
  static bool hasChild(const Node* n);

public:
  RadixTrie();
  ~RadixTrie();

  // Inserta todas las apariciones (puede insertar repetidas) de `word` en la línea `line`.
  void insert(const string& word, int line);
  // Devuelve las líneas donde aparece exactamente `word`. Si no existe, vector vacío.
  vector<int> search(const string& word);
};
