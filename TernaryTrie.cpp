#include "TernaryTrie.hh"


// Inserta la palabra y añade su ocurrencia
void TernaryTrie::insert(const string& word, WordID id, int line, long long pos) {
  if (word.empty()) return;

  const char* s = word.c_str();
  size_t i = 0;

  if (!root) root = make(s[0]);
  Node* p = root;

  while (true) {
    if (s[i] < p->c) {
      if (!p->lo) p->lo = make(s[i]);
      p = p->lo;
    } else if (s[i] > p->c) {
      if (!p->hi) p->hi = make(s[i]);
      p = p->hi;
    } else {
      // s[i] == p->c
      if (i + 1 == word.size()) {
        // último carácter de la palabra
        lexicon_[id] = word;
        p->wordID = id;
        add_occur(id, line, pos);
        return;
      }
      if (!p->eq) p->eq = make(s[i + 1]);
      p = p->eq;
      ++i;
    }
  }
}

// Busca la palabra y retorna su wordID (-1 si no está)
WordID TernaryTrie::search(const string& word) const {
  if (!root || word.empty()) return -1;

  const char* s = word.c_str();
  size_t i = 0;
  const Node* p = root;

  ++visited_nodes_;                // visitamos root
  while (p) {
    if (s[i] < p->c) {
      p = p->lo;
    } else if (s[i] > p->c) {
      p = p->hi;
    } else {
      // s[i] == p->c
      if (i + 1 == word.size()) {
        return p->wordID;         // -1 si no termina aquí
      }
      p = p->eq;
      ++i;
    }
    if (p) ++visited_nodes_;       // contamos el siguiente nodo al que pasamos
  }
  return -1;
}

vector<int> TernaryTrie::explore_subtree(const string& prefix) const {
  vector<int> wordIDs;
  if (!root) return wordIDs;

  if (prefix.empty()) {
    stack<const Node*> st;
    st.push(root);
    while (!st.empty()) {
      auto n = st.top(); st.pop();
      ++visited_nodes_;            // cada nodo extraído se considera visitado
      if (!n) continue;
      if (n->wordID != -1) wordIDs.push_back(n->wordID);
      st.push(n->lo);
      st.push(n->eq);
      st.push(n->hi);
    }
    return wordIDs;
  }

  // bajar hasta el nodo del último carácter del prefijo
  const Node* p = root;
  size_t i = 0;
  ++visited_nodes_;                // visit root
  while (p) {
    if (prefix[i] < p->c) {
      p = p->lo;
    } else if (prefix[i] > p->c) {
      p = p->hi;
    } else {
      if (i + 1 == prefix.size()) break;
      p = p->eq;
      ++i;
    }
    if (p) ++visited_nodes_;       // contamos cada avance
  }
  if (!p) return wordIDs;          // prefijo no presente

  if (p->wordID != -1) wordIDs.push_back(p->wordID);

  // explorar continuaciones bajo 'eq'
  stack<const Node*> st;
  st.push(p->eq);
  while (!st.empty()) {
    auto n = st.top(); st.pop();
    ++visited_nodes_;
    if (!n) continue;
    if (n->wordID != -1) wordIDs.push_back(n->wordID);
    st.push(n->lo);
    st.push(n->eq);
    st.push(n->hi);
  }
  return wordIDs;
}

vector<WordID> TernaryTrie::complete_prefix_topk(const string& prefix, size_t k) const {
    vector<WordID> out;
    visited_nodes_ = 0;

    // 1) bajar al nodo del ÚLTIMO carácter del prefijo
    const Node* n = root;
    size_t i = 0;
    while (n && i < prefix.size()) {
        ++visited_nodes_;
        if (prefix[i] < n->c)      n = n->lo;
        else if (prefix[i] > n->c) n = n->hi;
        else {
            if (i + 1 == prefix.size()) break; // 'n' es el nodo del último char del prefijo
            n = n->eq; ++i;
        }
    }
    if (!n || i + 1 != prefix.size()) return out;

    // 2) capas: capa 1 = todos los nodos alcanzables tras +1 carácter (arranca en n->eq)
    vector<const Node*> current;
    if (n->eq) current.push_back(n->eq);

    while (!current.empty() && out.size() < k) {
        // Recorremos TODO el bosque de esta capa SOLO por lo/hi (misma profundidad).
        // Y recogemos los eq para la próxima capa.
        vector<const Node*> next;
        stack<const Node*> st;

        for (const Node* seed : current) if (seed) st.push(seed);

        while (!st.empty() && out.size() < k) {
            const Node* u = st.top(); st.pop();
            ++visited_nodes_;

            if (u->wordID != -1) {
                // Palabra encontrada EXACTAMENTE a esta profundidad
                auto it = lexicon_.find(u->wordID);
                if (it != lexicon_.end() && !it->second.empty()) {
                    out.push_back(u->wordID);
                    if (out.size() >= k) break;
                }
            }
            // misma profundidad: explorar lo/hi
            if (u->lo) st.push(u->lo);
            if (u->hi) st.push(u->hi);
            // siguiente capa: guardar eq
            if (u->eq) next.push_back(u->eq);
        }

        current.swap(next); // siguiente profundidad (+1 carácter)
    }

    return out;
}

  void TernaryTrie::add_occur(WordID id, int line, long long pos) {
    table_[id].push_back(Match{line,pos});
  }
  vector<int> TernaryTrie::get_lines(WordID id) const {
    auto it = table_.find(id);
    if (it == table_.end()) return {};
    vector<int> out;
    out.reserve(it->second.size());
    for (const auto& m : it->second) out.push_back(m.line);
    return out;
  }
  vector<long long> TernaryTrie::get_positions(WordID id) const {
    auto it = table_.find(id);
    if (it == table_.end()) return {};
    vector<long long> out;
    out.reserve(it->second.size());
    for (const auto& m : it->second) out.push_back(m.pos);
    return out;
  }

   size_t TernaryTrie::count_nodes_rec(const Node* n) const {
    if (!n) return 0;
    return 1 + count_nodes_rec(n->lo)
             + count_nodes_rec(n->eq)
             + count_nodes_rec(n->hi);
  }
 
  size_t TernaryTrie::memory_bytes_rec(const Node* n) const {
    if (!n) return 0;
    size_t bytes = sizeof(*n);                  // char + 3 punteros + int
    bytes += memory_bytes_rec(n->lo);
    bytes += memory_bytes_rec(n->eq);
    bytes += memory_bytes_rec(n->hi);
    return bytes;
  }

  size_t TernaryTrie::memory_bytes_estimate() const {
    return memory_bytes_rec(root) + sizeof(*this) - sizeof(root);
  }