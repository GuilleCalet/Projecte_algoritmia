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

  // Elimina, si existe, la palabra del Trie
  bool eliminate(const std::string& word) {
    Node* node = root;

    // Path que hay que seguir para encontrar la palabra buscada
    std::vector<std::pair<Node*,int>> path; // (parent, idx del hijo tomado)

    // Descenso por el Trie guardando el path
    for (char ch : word) {
      int idx = ch - 'a';
      if (not existsChild(node, ch)) return false; // la palabra no existe
      path.push_back(pair(node,idx));
      node = node->children[idx];
    }

    // La palabra debe existir en el nodo hoja
    if (node->terminal == 0) return false;
    node->terminal -= 1; // Si existe, decrementar ocurrencias

    // No hace falta borrar ningún nodo cuando:
    //  - aún quedan ocurrencias de la palabra
    //  - la palabra = word es prefijo de otra (tiene hijos)
    if (node->terminal > 0 or hasChildren(node)) return true;

    // Ascenso por el Trie borrando nodos innecesarios hasta encontrar:
    //  - un nodo con terminal>0 (palabra más corta que debemos preservar)
    //  - un nodo que tenga algún otro hijo
    for (int i = path.size() - 1; i >= 0; i--) {
      Node* parent = path[i].first; // nodo padre
      int idx = path[i].second; // índice del hijo tomado
      Node* curr = parent->children[idx]; // nodo hijo

      // Si el nodo actual sigue siendo necesario, dejamos de eliminar nodos
      if (curr->terminal > 0 or hasChildren(curr)) return true;

      // En caso contrario lo eliminamos (liberamos memoria)
      delete curr;
      parent->children[idx] = nullptr;
    }

    return true;
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