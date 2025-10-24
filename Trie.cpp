#include "Trie.hh"


// Constructor
Trie::Trie() : root(new Node) {}


// Añade una nueva ocurrencia (line,pos) de la palabra identificada por id
void Trie::add_occur(WordID id, int line, long long pos) {
  table_[id].push_back(Match{line,pos});
}

// Retorna el vector de en que línias aparecen las ocurrencias de la palabra identificada por id
vector<int> Trie::get_lines(WordID id) const {
  auto it = table_.find(id);
  if (it == table_.end()) return {};

  vector<int> out;
  for (Match m : it->second) out.push_back(m.line);
  return out;
}

// Retorna el vector de en que posiciones (generales) aparecen las ocurrencias de la palabra identificada por id
vector<long long> Trie::get_positions(WordID id) const {
  auto it = table_.find(id);
  if (it == table_.end()) return {};

  vector<long long> out;
  for (Match m : it->second) out.push_back(m.pos);
  return out;
}

// Inserta la palabra identificada por id en el trie, y añade su ocurrencia (line,pos)
void Trie::insert(const string& word, WordID id, int line, long long pos) {
  Node* node = root;

  for (char ch : word) {
    // Si un nodo no contiene el hijo ch correspondiente, lo crea
    if (node->children.find(tolower(ch)) == node->children.end()) node->children[ch] = new Node;
    node = node->children[ch];
  }

  node->wordID = id;  
  lexicon_[id] = word;
  add_occur(id, line, pos); // Guardar la línea y la posición
}

// Busca la palabra en el trie, y si existe retorna su id (-1 si no existe)
WordID Trie::search(const string& word) const {
  Node* node = root;
  ++visited_nodes_;

  for (char ch : word) {
    if (node->children.find(tolower(ch)) == node->children.end()) return -1; // la palabra no existe
    node = node->children.at(ch);
    ++visited_nodes_;
  }

  return node->wordID;
}

vector<int> Trie::explore_subtree(const string& prefix) const {
    vector<int> wordIDs;
    Node* node = root;
    ++visited_nodes_;

    // Buscar el nodo correspondiente al último carácter del prefijo
    for (char ch : prefix) {
      if (node->children.find(ch) == node->children.end()) return {};  // No se encuentra el prefijo en el Trie
      node = node->children.at(ch);
      ++visited_nodes_;
    }

    // Recorrer el subárbol a partir de este nodo
    stack<Node*> nodes;
    nodes.push(node);

    while (!nodes.empty()) {
      Node* currentNode = nodes.top();
      nodes.pop();
      ++visited_nodes_;
      // Si el nodo es un nodo final de palabra, agregar su wordID
      if (currentNode->wordID != -1) wordIDs.push_back(currentNode->wordID);
      // Añadir todos los hijos del nodo actual a la pila
      for (auto& child : currentNode->children) nodes.push(child.second);
    }
    return wordIDs;
}


size_t Trie::count_nodes_rec(const Node* n) const {
  if (!n) return 0;
  size_t cnt = 1; // este nodo
  for (const auto& kv : n->children) cnt += count_nodes_rec(kv.second);
  return cnt;
}

size_t Trie::node_count() const {
  return count_nodes_rec(root);
}


size_t Trie::memory_bytes_rec(const Node* n) const {
  if (!n) return 0;
  size_t bytes = sizeof(*n); // Node
  // coste de las entradas
  bytes += n->children.size() * (sizeof(char) + sizeof(Node*));
  // coste aproximado de buckets del hash
  bytes += n->children.bucket_count() * sizeof(void*);
  for (const auto& kv : n->children) bytes += memory_bytes_rec(kv.second);
  return bytes;
}

size_t Trie::memory_bytes_estimate() const {
  return memory_bytes_rec(root) + sizeof(*this) - sizeof(root); 
}

vector<WordID> Trie::complete_prefix_topk(const string& prefix, size_t k) const {
    vector<WordID> out;
    visited_nodes_ = 0;

    // 1) bajar al nodo del prefijo
    const Node* node = root;
    ++visited_nodes_;                 // raíz
    for (char ch : prefix) {
        auto it = node->children.find(ch);
        if (it == node->children.end()) return out; // no hay subárbol con ese prefijo
        node = it->second;
        ++visited_nodes_;
    }

    // 2) BFS por capas: iniciar con los hijos a profundidad = 1
    struct Item { const Node* n; int depth; };
    queue<Item> q;
    for (const auto& kv : node->children) q.push({kv.second, 1});

    while (!q.empty() && out.size() < k) {
        Item cur = q.front(); q.pop();
        ++visited_nodes_;

        if (cur.n->wordID != -1) {
            out.push_back(cur.n->wordID);      // palabra a esta profundidad
            if (out.size() >= k) break;
        }
        for (const auto& kv : cur.n->children) q.push({kv.second, cur.depth + 1});
    }

    return out;
}
