
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

size_t RadixTrie::matchLabel(const std::string& w, size_t i, const std::string& label) {
    size_t k = 0, m = std::min(label.size(), w.size() - i);
    while (k < m && w[i + k] == label[k]) ++k;
    return k;
}


bool RadixTrie::hasChild(const Node* n) {
  for (const Edge* e : n->edges) if (e) return true; 
  return false;
}

void RadixTrie::insert(const string& w, int ID) {
  if (w.empty()) return;
    Node* node = root;
    size_t i = 0;
    while (i < w.size()) {
        char ch = w[i];
        if (node->edges.find(ch) == node->edges.end()) {
            node->edges[ch] = new Edge{w.substr(i), new Node};
            node->edges[ch]->child->wordID = ID;
            return;
        }
        size_t k = std::min(w.size() - i, node->edges[ch]->label.size());
        std::string common = w.substr(i, k);
        if (common == node->edges[ch]->label) {
            node = node->edges[ch]->child;
            i += k;
            continue;
        }

        // Aquí hacemos la separación si el sufijo no coincide completamente
        std::string edgeSuffix = node->edges[ch]->label.substr(k);
        node->edges[ch]->label = common;

        Node* mid = new Node;
        node->edges[ch]->child = mid;
        mid->edges[edgeSuffix[0]] = new Edge{edgeSuffix, new Node};
        mid->edges[edgeSuffix[0]]->child->wordID = ID;
        return;
    }
    node->wordID;
}

int RadixTrie::search(const string& w, long long&ns) const {
    using clock = std::chrono::high_resolution_clock;
   auto t0 = clock::now();

   Node* node = root;
    size_t i = 0;
    while (i < w.size()) {
        char ch = w[i];
        if (node->edges.find(ch) == node->edges.end()) return -1;  // No encontrado
        size_t k = std::min(w.size() - i, node->edges[ch]->label.size());
        if (w.substr(i, k) != node->edges[ch]->label) return -1;  // No coincide la palabra
        i += k;
        node = node->edges[ch]->child;
    }
  
    auto t1 = clock::now();
  ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
  return node->wordID; // si no es palabra final, lines estará vacío
}
