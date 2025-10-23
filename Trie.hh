#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>

#include "Match.hh"

using namespace std;

typedef int WordID;

class Trie {
private:

  // Estructura de un nodo del Trie
  struct Node {
    unordered_map<char, Node*> children; // punteros a los nodos hijos
    int wordID = -1; // identificador de la palabra (-1 si no es un nodo de final de palabra)
  };

  Node* root = nullptr; // raíz del Trie

  unordered_map<WordID, vector<Match>> table_; // tabla de tuplas (line,pos) para cada ocurrencia de cada palabra del texto

  /** @brief Converteix un caràcter a l'index corresponent normalitzat
  \pre El caràcter ch és una lletra da l'alfabet
  \post Retorna el caràcter ch en la seva forma minúscula
  */
  static int ctoi(char ch);

  // ---- MÉTRICAS INTERNAS ----
  size_t count_nodes_rec(const Node* n) const;
  size_t memory_bytes_rec(const Node* n) const;

  // para instrumentación de visitas
  mutable size_t visited_nodes_ = 0;

public:
  /** @brief Crea un Trie buit
  \pre cert
  \post El resultat és un Trie inicialitzat sense cap paraula
  */
  Trie();
  /** @brief Destrueix el trie i allibera la seva memòria
  \pre cert
  \post El trie queda eliminat i la memòria dels nodes queda alliberada
  */
  ~Trie(); 

  // Retorna el vector de ocurrencias (line,pos) de una palabra identificada por id
  vector<Match> occurrences(WordID id) const;

  // Añade una nueva ocurrencia (line,pos) de la palabra identificada por id
  void add_occur(WordID id, int line, long long pos);

  // Retorna el vector de en que línias aparecen las ocurrencias de la palabra identificada por id
  vector<int> get_lines(WordID id) const;

  // Retorna el vector de en que posiciones (generales) aparecen las ocurrencias de la palabra identificada por id
  vector<long long> get_positions(WordID id) const;

  /** @brief Insereix una paraula dins del trie
  \pre La string word només té caràcters alfabètics previament nomalitzats
  \post El trie conté la paraula "word", si ja contenia la paraula, incrementa el seu contador de repeticions
  */
  // Inserta la palabra identificada por id en el trie, y añade su ocurrencia (line,pos)
  void insert(const string& word, WordID id, int line, long long pos);

  /** @brief Comproba si el Trie conté la paraula "word"
  \pre La string word només té caràcters alfabètics previament nomalitzats
  \post Retorna cert si la paraula "word" és al trie, retorna fals en cas contrari
  */
  // Busca la palabra en el trie, y si existe retorna su id (-1 si no existe)
  WordID search(const string& word) const;

  vector<int> explore_subtree(const string& prefix) const;

  // ---- API DE MÉTRICAS ----
  size_t node_count() const;              // nº total de nodos
  size_t memory_bytes_estimate() const;   // estimación de memoria en bytes

  // ---- Instrumentación de visitas por operación ----
  void reset_visited() const { visited_nodes_ = 0; }
  size_t last_visited() const { return visited_nodes_; }
};