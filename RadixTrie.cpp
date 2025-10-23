
// =========================
// RadixTrie.cpp
// =========================
#include "RadixTrie.hh"
#include<iostream>
#include <algorithm>
#include <stack>
#include<chrono>

using namespace std;

RadixTrie::RadixTrie() : root(new Node) {}

RadixTrie::~RadixTrie() { destroy(root); root = nullptr; }

void RadixTrie::destroy(Node* node) {
    if (!node) return;
    for (auto& e : node->edges) {
        destroy(e.second->child);  // Destruir los hijos
        delete e.second;  // Eliminar la arista
    }
    delete node;
}

size_t RadixTrie::lcp(const string& a, const string& b) {
    size_t m = min(a.size(), b.size());
    size_t k = 0;
    while (k < m && a[k] == b[k]) ++k;
    return k;
}

size_t RadixTrie::matchLabel(const std::string& word, size_t i, const std::string& label) {
    size_t k = 0, m = std::min(label.size(), word.size() - i);
    while (k < m && word[i + k] == label[k]) ++k;
    return k;
}


bool RadixTrie::hasChild(const Node* n) {
    return not n->edges.empty();
}

// Retorna el vector de ocurrencias (line,pos) de una palabra identificada por id
vector<Match> RadixTrie::occurrences(WordID id) const {
  auto it = table_.find(id);
  return (it == table_.end()) ? vector<Match>() : it->second;
}

// Añade una nueva ocurrencia (line,pos) de la palabra identificada por id
void RadixTrie::add_occur(WordID id, int line, long long pos) {
  table_[id].push_back(Match{line,pos});
}

// Retorna el vector de en que línias aparecen las ocurrencias de la palabra identificada por id
vector<int> RadixTrie::get_lines(WordID id) const {
  auto it = table_.find(id);
  if (it == table_.end()) return {};

  vector<int> out;
  for (Match m : it->second) out.push_back(m.line);
  return out;
}

// Retorna el vector de en que posiciones (generales) aparecen las ocurrencias de la palabra identificada por id
vector<long long> RadixTrie::get_positions(WordID id) const {
  auto it = table_.find(id);
  if (it == table_.end()) return {};

  vector<long long> out;
  for (Match m : it->second) out.push_back(m.pos);
  return out;
}

// Inserta la palabra identificada por id en el trie, y añade su ocurrencia (line,pos)
void RadixTrie::insert(const string& word, WordID id, int line, long long pos) {
    if (word.empty()) return;

    Node* node = root;
    size_t i = 0;

    while (i < word.size()) {
        char ch = word[i];

        auto it = node->edges.find(ch);

        if (it == node->edges.end()) {
            node->edges[ch] = new Edge{word.substr(i), new Node};
            node->edges[ch]->child->wordID = id;
            lexicon_[id] = word;
            add_occur(id, line, pos);
            return;
        }

        Edge* e = it->second;
        const std::string& label = e->label;

        // Longest common prefix entre label y la parte restante de word
        size_t k = 0;
        const size_t m = std::min(label.size(), word.size() - i);
        while (k < m && word[i + k] == label[k]) ++k;

        if (k == label.size()) {
            // La etiqueta completa coincide: bajamos por la arista
            i += k;
            node = e->child;
            continue;
        }

        // Split de la arista existente en el punto k
        // Guardamos el hijo antiguo y su etiqueta
        Node* oldChild = e->child;
        std::string oldLabel = label;

        // Nodo intermedio
        Node* mid = new Node;

        // La arista desde 'node' ahora termina en 'mid' con el prefijo común
        e->label = oldLabel.substr(0, k);
        e->child = mid;

        // Recolgamos el hijo antiguo bajo 'mid' con el sufijo antiguo
        std::string oldSuffix = oldLabel.substr(k);
        Edge* downOld = new Edge{ oldSuffix, oldChild };
        mid->edges[oldSuffix[0]] = downOld;

        // Resto por insertar desde la palabra actual
        std::string rem = word.substr(i + k);

        if (rem.empty()) {
            // La palabra termina exactamente en 'mid'
            mid->wordID = id;
            lexicon_[id] = word;
            add_occur(id, line, pos);
            return;
        } else {
            // Colgar el resto como nueva arista desde 'mid'
            Edge* newEdge = new Edge{ rem, new Node };
            newEdge->child->wordID = id;
            mid->edges[rem[0]] = newEdge;
            lexicon_[id] = word;
            add_occur(id, line, pos);
            return;
        }
    }

    // Si salimos del bucle, hemos consumido la palabra exactamente en 'node'
    // (sin crear nuevas aristas): marcar terminal y registrar ocurrencia
    node->wordID = id;
    add_occur(id, line, pos);
}

// Busca la palabra en el trie, y si existe retorna su id (-1 si no existe)
WordID RadixTrie::search(const string& word) const {
    Node* node = root;
    ++visited_nodes_;

    size_t i = 0;
    while (i < word.size()) {
        char ch = word[i];
        auto it = node->edges.find(ch);
        if (it == node->edges.end()) return -1;

        const Edge* e = it->second;
        const std::string& label = e->label;

        // Debe coincidir toda la etiqueta de la arista
        const size_t m = std::min(label.size(), word.size() - i);
        size_t k = 0;
        while (k < m && word[i + k] == label[k]) ++k;
        if (k != label.size()) return -1;   // no coincide toda la etiqueta

        i += k;
        node = e->child;
        ++visited_nodes_;
    }

    return node->wordID; // -1 si no es final de palabra
}

std::vector<int> RadixTrie::explore_subtree(const std::string& prefix) const {
    std::vector<int> wordIDs;
    Node* node = root;
    ++visited_nodes_;
    size_t i = 0;

    // Buscar el nodo correspondiente al último carácter del prefijo
    while (i < prefix.size()) {
        char ch = prefix[i];
        if (node->edges.find(ch) == node->edges.end()) {
            return {};  // El prefijo no se encuentra en el RadixTrie
        }
        size_t k = std::min(prefix.size() - i, node->edges[ch]->label.size());
        if (prefix.substr(i, k) != node->edges[ch]->label) {
            return {};  // El prefijo no coincide
        }
        i += k;
        node = node->edges[ch]->child;
        ++visited_nodes_;
    }

    // Recorrer el subárbol a partir del nodo encontrado
    std::stack<Node*> nodes;
    nodes.push(node);

    while (!nodes.empty()) {
        Node* currentNode = nodes.top();
        nodes.pop();
        ++visited_nodes_;
        // Si el nodo es un nodo final de palabra, agregar su wordID
        if (currentNode->wordID != -1) {
            wordIDs.push_back(currentNode->wordID);
        }

        // Añadir todos los hijos del nodo actual a la pila
        for (auto& edge : currentNode->edges) {
            nodes.push(edge.second->child);
        }
    }

    return wordIDs;
}

// ---- Conteo de nodos ----
size_t RadixTrie::count_nodes_rec(const Node* n) const {
  if (!n) return 0;
  size_t cnt = 1;
  for (const auto& kv : n->edges) cnt += count_nodes_rec(kv.second->child);
  return cnt;
}

size_t RadixTrie::node_count() const {
  return count_nodes_rec(root);
}

// ---- Estimación de memoria ----
// Aproximación portable (depende del impl. de std::unordered_map y std::string):
//  - sizeof(Node) por nodo
//  - por cada Edge: sizeof(Edge) + label.capacity() (buffer del string)
//  - entradas del unordered_map: (clave char + puntero Edge*)
//  - buckets del unordered_map: bucket_count * sizeof(void*)
size_t RadixTrie::memory_bytes_rec(const Node* n) const {
  if (!n) return 0;
  size_t bytes = sizeof(*n);
  // coste hash de las entradas y buckets
  bytes += n->edges.size() * (sizeof(char) + sizeof(Edge*));
  bytes += n->edges.bucket_count() * sizeof(void*);
  for (const auto& kv : n->edges) {
    Edge* e = kv.second;
    bytes += sizeof(*e);
    bytes += e->label.capacity(); // buffer del string (aprox.)
    bytes += memory_bytes_rec(e->child);
  }
  return bytes;
}

size_t RadixTrie::memory_bytes_estimate() const {
  return memory_bytes_rec(root) + sizeof(*this) - sizeof(root);
}

std::vector<WordID> RadixTrie::complete_prefix_topk(const std::string& prefix, size_t k) const {
    std::vector<WordID> out;
    visited_nodes_ = 0;

    const Node* node = root;
    ++visited_nodes_;

    // 1) Bajar siguiendo el prefijo (permitiendo acabar a mitad de etiqueta)
    size_t i = 0;
    size_t carry = 0; // chars pendientes si el prefijo acaba a mitad de label
    while (i < prefix.size()) {
        char ch = prefix[i];
        auto it = node->edges.find(ch);
        if (it == node->edges.end()) return out;

        const Edge* e = it->second;
        const std::string& lab = e->label;

        const size_t m = std::min(lab.size(), prefix.size() - i);
        size_t kcmp = 0;
        while (kcmp < m && prefix[i + kcmp] == lab[kcmp]) ++kcmp;
        if (kcmp < m) return out;  // el prefijo no coincide

        i += kcmp;
        if (kcmp < lab.size()) {
            // Prefijo termina dentro de esta etiqueta
            carry = lab.size() - kcmp;     // falta por “consumir” para llegar al hijo
            node  = e->child;
            ++visited_nodes_;
            break;
        }
        node = e->child;
        ++visited_nodes_;
    }

    // 2) Min-heap por profundidad (chars desde el prefijo)
    //    Item: (depth, rem, node). Mientras rem>0, “consumimos” 1 char y
    //    reencolamos el MISMO nodo con depth+1 y rem-1. Cuando rem==0,
    //    podemos considerar wordID y expandir hijos.
    struct Item {
        size_t depth;      // caracteres añadidos desde el prefijo
        size_t rem;        // chars pendientes para “llegar” al nodo actual
        const Node* n;
        bool operator>(const Item& o) const {
            if (depth != o.depth) return depth > o.depth;
            return rem > o.rem; // desempate estable
        }
    };

    std::priority_queue<Item, std::vector<Item>, std::greater<Item>> pq;
    pq.push({0, carry, node});

    while (!pq.empty() && out.size() < k) {
        Item it = pq.top(); pq.pop();
        ++visited_nodes_;

        if (it.rem > 0) {
            // Aún estamos “dentro” de una etiqueta: consumimos 1 char y seguimos en este nodo
            pq.push({it.depth + 1, it.rem - 1, it.n});
            continue;
        }

        // Ya “hemos llegado” al nodo
        if (it.depth >= 1 && it.n->wordID != -1) {
            out.push_back(it.n->wordID);
            if (out.size() >= k) break;
        }

        // Expandir hijos: cada arista añade e->label.size() caracteres
        for (const auto& kv : it.n->edges) {
            const Edge* e = kv.second;
            const size_t L = e->label.size();
            pq.push({it.depth + L, L, e->child});
        }
    }

    return out;
}
