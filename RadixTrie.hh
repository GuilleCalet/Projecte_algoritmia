// =========================
// RadixTrie.hh
// =========================
#pragma once
#include <string>
#include <vector>
#include <array>

class RadixTrie {
  struct Node; // fwd
  struct Edge {
    std::string label; // segmento comprimido del camino
    Node* child = nullptr;
  };
  struct Node {
    // Como en el trie original, guardamos en los nodos finales el vector de líneas
    std::vector<int> lines;           
    // En un radix trie, desde un nodo no puede haber dos aristas que empiecen por la misma letra
    // Usamos array indexado por 'a'..'z' para acceso O(1)
    std::array<Edge*,26> edges{};     // punteros nulos por defecto
  };

  Node* root = nullptr;

public:
  RadixTrie();
  ~RadixTrie();

  // Inserta todas las apariciones (puede insertar repetidas) de `word` en la línea `line`.
  void insert(const std::string& word, int line);
  // Devuelve las líneas donde aparece exactamente `word`. Si no existe, vector vacío.
  std::vector<int> search(const std::string& word, long long&ns) const;

private:
  static inline int ctoi(char ch) {
    unsigned char uch = static_cast<unsigned char>(ch);
    return std::tolower(uch) - 'a';
  }
  static size_t lcp(const std::string& a, const std::string& b); // longest common prefix

  // Utilidades de gestión de memoria/estructura
  static void destroy(Node* n);
  static bool hasChild(const Node* n);
};
