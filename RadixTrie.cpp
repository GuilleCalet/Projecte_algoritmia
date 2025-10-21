
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

void RadixTrie::destroy(Node* n) {
  if (!n) return;
  for (Edge* e : n->edges) {
    if (e) { destroy(e->child); delete e; }
  }
  delete n;
}

size_t RadixTrie::lcp(const string& a, const string& b) {
  size_t m = min(a.size(), b.size());
  size_t k = 0;
  while (k < m && a[k] == b[k]) ++k;
  return k;
}

bool RadixTrie::hasChild(const Node* n) {
  for (const Edge* e : n->edges) if (e) return true; 
  return false;
}

void RadixTrie::insert(const string& w, int line) {
  if (w.empty()) return;

  // validación de caracteres
  for (char ch : w) { 
    int idx = ctoi(ch); 
    if (idx < 0 || idx >= 26) {
      cerr << "Error: la palabra " << w << " no se ha podido insertar porque tiene carácteres no alfabéticos\n";
      return; // ignora palabras con carácteres no alfabéticos
    }
  }

  Node* node = root;
  size_t i = 0;
  while (i < w.size()) {
    int idx = ctoi(w[i]);
    Edge* &edge = node->edges[idx];
    
    if (!edge) {
      // no hay arista: creamos una que cuelga todo el sufijo restante
      edge = new Edge{ w.substr(i), new Node };
      edge->child->lines.push_back(line);
      return;
    }

    // existe arista, comparamos etiqueta
    size_t k = lcp(w.substr(i), edge->label);
    if (k == edge->label.size()) {
      // consumimos arista completa y seguimos abajo
      i += k; 
      node = edge->child; 
      continue;
    }

    // split en 3 casos: común k, resto de arista, y resto de palabra
    Node* mid = new Node;
    // 1) la parte común pasa a ser la etiqueta de la arista original hacia mid
    string common = edge->label.substr(0, k);
    string edgeSuffix = edge->label.substr(k);
    string wordSuffix = w.substr(i + k);

    // reusar el objeto Edge existente para que siga en su misma casilla (idx)
    Node* oldChild = edge->child;
    edge->label = common;
    edge->child = mid;

    // 2) colgamos la parte antigua (edgeSuffix) desde mid hacia oldChild
    int idxOld = ctoi(edgeSuffix[0]);
    mid->edges[idxOld] = new Edge{ edgeSuffix, oldChild };

    if (wordSuffix.empty()) {
      // 3a) la palabra termina justo en mid
      mid->lines.push_back(line);
      return;
    } else {
      // 3b) añadimos una nueva arista para el sufijo de la palabra hacia un nuevo nodo final
      int idxNew = ctoi(wordSuffix[0]);
      mid->edges[idxNew] = new Edge{ wordSuffix, new Node };
      mid->edges[idxNew]->child->lines.push_back(line);
      return;
    }
  }

  // i == w.size(): la palabra acaba exactamente en `node`
  node->lines.push_back(line);
}

vector<int> RadixTrie::search(const string& w) {
  if (w.empty()) return {};
  
  for (char ch : w) {
    int idx = ctoi(ch);
    if (idx < 0 || idx >= 26) {
      cerr << "Error: la palabra buscada " << w << " tiene carácteres no compatibles\n";
      return {}; // ignora palabras con carácteres no alfabéticos
    } 
  }

  Node* node = root;
  size_t i = 0;
  while (i < w.size()) {
    int idx = ctoi(w[i]);
    Edge* e = node->edges[idx];
    if (!e) return {};
    size_t k = lcp(w.substr(i), e->label);
    if (k < e->label.size()) return {}; // nos paramos a mitad de etiqueta => palabra inexistente
    i += k; // consumimos etiqueta
    node = e->child;
  }

  return node->lines; // si no es palabra final, lines estará vacío
}
