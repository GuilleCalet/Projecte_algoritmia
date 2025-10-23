#include <vector>
#include <string>
#include <algorithm>
#include<chrono>

using namespace std;

typedef int WordID;

template <typename TType>
static int levenshtein(const string& a, const string& b) {
    // O(|a|*|b|) memoria y tiempo
    const int n = (int)a.size(), m = (int)b.size();
    if (n == 0) return m;
    if (m == 0) return n;
    vector<int> prev(m+1), dp(m+1);
    for (int j = 0; j <= m; ++j) prev[j] = j;
    for (int i = 1; i <= n; ++i) {
        dp[0] = i;
        for (int j = 1; j <= m; ++j) {
            int cost = (a[i-1] == b[j-1]) ? 0 : 1;
            dp[j] = min({ prev[j] + 1,         // borrado en 'a'
                          dp[j-1] + 1,         // inserción en 'a'
                          prev[j-1] + cost }); // sustitución
        }
        swap(prev, dp);
    }
    return prev[m];
}

template <typename TType>
void autocomplete(const TType& trie, const string& word) {
    using clock = std::chrono::high_resolution_clock;

    if (word.empty()) { cout << "Autocompletat: consulta buida.\n"; return; }

    // 1) ¿ya existe?
    auto t0_total = clock::now();
    trie.reset_visited();
    WordID exact = trie.search(word);
    size_t visited = trie.last_visited();
    if (exact != -1) {
        auto t1_total = clock::now();
        auto ns_total = std::chrono::duration_cast<std::chrono::nanoseconds>(t1_total - t0_total).count();
        cout << "La paraula '" << word << "' ja existeix; no cal autocompletar.\n";
        cout << "Temps: " << ns_total << " ns | Nodes visitats: " << visited << "\n";
        return;
    }

    // 2) recolectar todo el vocabulario
    trie.reset_visited();
    vector<WordID> ids = trie.explore_subtree(string{}); // prefijo vacío => todo
    size_t visited_vocab = trie.last_visited();

    if (ids.empty()) {
        auto t1_total = clock::now();
        auto ns_total = std::chrono::duration_cast<std::chrono::nanoseconds>(t1_total - t0_total).count();
        cout << "Autocompletat: diccionari buit.\n";
        cout << "Temps: " << ns_total << " ns | Nodes visitats: " << (visited + visited_vocab) << "\n";
        return;
    }

    // 3) puntuar por distancia de edición
    struct Cand { string w; int d; };
    vector<Cand> cands; cands.reserve(ids.size());
    for (WordID id : ids) {
        string w = trie.word_of(id);     // *** necesita el getter que añadimos ***
        if (!w.empty()) {
            int d = levenshtein<TType>(word, w);
            cands.push_back({w, d});
        }
    }
    // ordenar por (distancia asc, palabra asc)
    sort(cands.begin(), cands.end(), [](const Cand& a, const Cand& b){
        if (a.d != b.d) return a.d < b.d;
        return a.w < b.w;
    });
    if (cands.size() > 5) cands.resize(5);

    auto t1_total = clock::now();
    auto ns_total = std::chrono::duration_cast<std::chrono::nanoseconds>(t1_total - t0_total).count();

    // 4) salida
    if (cands.empty()) {
        cout << "Autocompletat: no hi ha candidats.\n";
    } else {
        cout << "Suggeriments (per distància d'edició a '" << word << "'):\n";
        for (int i = 0; i < (int)cands.size(); ++i) {
            cout << "  " << (i+1) << ") " << cands[i].w << "   [dist=" << cands[i].d << "]\n";
        }
    }
    cout << "Temps: " << ns_total << " ns | Nodes visitats: " << (visited + visited_vocab) << "\n";
}

template <typename TType>
void search_word(const TType& trie, const string& word) {
    if (word.empty()) { cout << "Consulta vacía.\n"; return; }

    auto t0 = std::chrono::high_resolution_clock::now();
    trie.reset_visited();
    WordID id = trie.search(word);
    size_t visited = trie.last_visited();
    auto t1 = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();

    if (id == -1) {
        cout << "No se ha encontrado la palabra '" << word << "' en el texto.\n";
        cout << "Tiempo: " << ns << " ns | Nodos visitados: " << visited << "\n";
        autocomplete(trie, word);
        return;
    }

    vector<int> lines = trie.get_lines(id);
    cout << "Se ha encontrado la palabra '" << word << "' en " << lines.size() << " línea(s): [";
    for (int i = 0; i < (int)lines.size(); i++) {
        if (i != 0) cout << ", ";
        cout << lines[i];
    }
    cout << "]\n";
    cout << "Tiempo: " << ns << " ns | Nodos visitados: " << visited << "\n";
}

template <typename TType>
void search_phrase(const TType& trie, const string& phrase) {
    using clock = std::chrono::high_resolution_clock;
    auto t0_total = clock::now();
    size_t visited_total = 0;

    vector<WordID> wordIDs;
    int i = 0, n = phrase.size();
    while (i < n) {
        while (i < n && phrase[i] == ' ') i++;
        int j = i;
        while (j < n && phrase[j] != ' ') j++;
        if (j > i)  {
            trie.reset_visited();
            WordID id = trie.search(phrase.substr(i, j-i));
            visited_total += trie.last_visited();
            if (id == -1) {
                auto t1_total = clock::now();
                auto ns_total = std::chrono::duration_cast<std::chrono::nanoseconds>(t1_total - t0_total).count();
                cout << "No se ha encontrado la frase '" << phrase << "' en el texto.\n";
                cout << "Tiempo: " << ns_total << " ns | Nodos visitados: " << visited_total << "\n";
                return;
            } else wordIDs.push_back(id);
        }
        i = j;
    }

    if (wordIDs.empty()) {
        auto t1_total = clock::now();
        auto ns_total = std::chrono::duration_cast<std::chrono::nanoseconds>(t1_total - t0_total).count();
        cout << "Consulta vacía.\n";
        cout << "Tiempo: " << ns_total << " ns | Nodos visitados: " << visited_total << "\n";
        return;
    }

    int k = (int)wordIDs.size();
    int min = 0;
    vector<vector<long long>> occs_pos(k);
    occs_pos[0] = trie.get_positions(wordIDs[0]);
    for (int p = 1; p < k; p++) {
        occs_pos[p] = trie.get_positions(wordIDs[p]);
        if (occs_pos[p].size() < occs_pos[min].size()) min = p;
    }

    vector<int> rel(k);
    for (int p = 0; p < k; p++) rel[p] = p - min;

    vector<int> result_lines;
    for (long long p0 : occs_pos[min]) {
        bool ok = true;
        for (int p = 1; p < k && ok; p++) {
            if (p == min) continue;
            long long need = p0 + rel[p];
            ok = binary_search(occs_pos[p].begin(), occs_pos[p].end(), need);
        }
        if (ok) {
            auto it = find(occs_pos[0].begin(), occs_pos[0].end(), p0 + rel[0]);
            if (it != occs_pos[0].end()) {
                int idx = (int)(it - occs_pos[0].begin());
                vector<int> word1_lines = trie.get_lines(wordIDs[0]);
                result_lines.push_back(word1_lines[idx]);
            }
        }
    }

    auto t1_total = clock::now();
    auto ns_total = std::chrono::duration_cast<std::chrono::nanoseconds>(t1_total - t0_total).count();

    if (result_lines.empty()) {
        cout << "No se ha encontrado la frase '" << phrase << "' en el texto.\n";
        cout << "Tiempo: " << ns_total << " ns | Nodos visitados: " << visited_total << "\n";
        return;
    }

    cout << "Se ha encontrado la frase '" << phrase << "' en " << result_lines.size() << " línea(s): [";
    for (int r = 0; r < (int)result_lines.size(); r++) {
        if (r != 0) cout << ", ";
        cout << result_lines[r];
    }
    cout << "]\n";
    cout << "Tiempo: " << ns_total << " ns | Nodos visitados: " << visited_total << "\n";
}


template <typename TType>
void search_prefix(const TType& trie, const string& prefix) { 
    using clock = std::chrono::high_resolution_clock;
    auto t0 = clock::now();
    trie.reset_visited();
    vector<WordID> wordIDs = trie.explore_subtree(prefix);
    size_t visited = trie.last_visited();
    auto t1 = clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();

    int numLines = 0;
    cout << "Se ha encontrado el prefijo '" << prefix << "' en las línea(s): [";
    for (int id = 0; id < wordIDs.size(); id++) {
        vector<int> result_lines = trie.get_lines(wordIDs[id]);
        numLines += result_lines.size();
        for (int i = 0; i < result_lines.size(); i++) {
            if (id != 0 or i != 0) cout << ", ";
            cout << result_lines[i];
        }
    }
    cout << "] nº líneas " << numLines << "\n";
    cout << "Tiempo: " << ns << " ns | Nodos visitados: " << visited << "\n";
}

