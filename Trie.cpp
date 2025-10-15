#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Trie {
private:

  // Estructura de un nodo del Trie
  struct Node {
    std::vector<Node*> children{26, nullptr}; // punteros a los hijos
    int terminal = 0; // contador de nºrepeticiones de la palabra
  };

  Node* root = nullptr; // raíz del Trie

  // Convierte un carácter en un índice normalizado (normaliza a minúsculas)
  static int ctoi(char ch) {};

  // Comprueba si en un nodo existe el hijo ch
  static bool existsChild(const Node* node, char ch) {
    int idx = ch - 'a';
    return (node->children[idx] != nullptr);
  }

  // Comprueba si un nodo tiene hijos
  static bool hasChildren(const Node* node) {
    for (const Node* child : node->children) {
      if (child != nullptr) return true;
    }
    return false;
  }

public:
  // Constructor
  Trie() : root(new Node) {}
  // Destructor
  ~Trie();

  // Inserta la palabra en el Trie
  void insert(const std::string& word) {
    Node* node = root;

    for (char ch : word) {
      int idx = ch - 'a';
      // Si un nodo no contiene el hijo ch correspondiente, lo crea
      if (not existsChild(node, ch)) node->children[idx] = new Node;
      node = node->children[idx];
    }

    node->terminal += 1; // augmentar ocurrencias
  }


  // Funció recursiva per eliminar la paraula
 bool eliminateAux(Node* node, const std::string& word, int index) {
    // Cas base: hem arribat al final de la paraula
    if (index == word.size()) {
        if (node->terminal > 0) {
            node->terminal -= 1; // Decrementem les ocurrències
            // Si el node no té fills, el podem eliminar
            return !hasChildren(node);
        }
        return false; // La paraula no existeix
    }

    // Obtenim l'índex de la lletra actual
    int idx = word[index] - 'a';
    
    // Si no existeix el fill, la paraula no està al trie
    if (!existsChild(node, word[index])) {
        return false;
    }

    // Recursió sobre el node fill
    bool shouldDeleteChild = eliminateAux(node->children[idx], word, index + 1);

    if (shouldDeleteChild) {
        // Si el fill es pot eliminar, alliberem la memòria i esborramos la referència
        delete node->children[idx];
        node->children[idx] = nullptr;

        // Si aquest node actual no té fills ni més paraules, també es pot eliminar
        return node->terminal == 0 && !hasChildren(node);
    }

    return false;
 }

 bool eliminate(const std::string& word) {
    return eliminateAux(root, word, 0);
 }

  // Comprueba si el Trie contiene la palabra
  bool search(const std::string& word) {
    Node* node = root;

    for (char ch : word) {
      int idx = ch - 'a'; 
      if (not existsChild(node, ch)) return false; // la palabra no existe
      node = node->children[idx];
    }

    // Comprueba si la palabra existe en el nodo hoja
    return (node->terminal > 0);
  }
};