#include <vector>
#include <string>
#include <algorithm>

using namespace std;

typedef int WordID;

template <typename TType>
void search_word(const TType& trie, const string& word) {
    if (word.empty()) {
        cout << "Consulta vacía.\n";
        return;
    }

    WordID id = trie.search(word);
    if (id == -1) {
        cout << "No se ha encontrado la palabra '" << word << "' en el texto.\n";
        return;
    }

    vector<int> lines = trie.get_lines(id);
    cout << "Se ha encontrado la palabra '" << word << "' en " << lines.size() << " línea(s): [";
    for (int i = 0; i < lines.size(); i++) {
        if (i != 0) cout << ", ";
        cout << lines[i];
    }
    cout << "]\n";
}

template <typename TType>
void search_phrase(const TType& trie, const string& phrase) {
    vector<WordID> wordIDs;
    
    int i = 0, n = phrase.size();
    while (i < n) {
        while (i < n and phrase[i] == ' ') i++;
        int j = i;
        while (j < n and phrase[j] != ' ') j++;
        if (j > i)  {
            WordID id = trie.search(phrase.substr(i, j-i));
            if (id == -1) {
                cout << "No se ha encontrado la frase '" << phrase << "' en el texto.\n";
                return;
            }
            else wordIDs.push_back(id);
        }
        i = j;
    }

    if (wordIDs.empty()) {
        cout << "Consulta vacía.\n";
        return;
    }

    int k = wordIDs.size(); // k = nº palabras

    // min = palabra con menos ocurrencias en el texto
    int min = 0;
    vector<vector<long long>> occs_pos(k);
    occs_pos[0] = trie.get_positions(wordIDs[0]);
    for (int i = 1; i < k; i++) {
        occs_pos[i] = trie.get_positions(wordIDs[i]);
        if (occs_pos[i].size() < occs_pos[min].size()) min = i;
    }

    vector<int> rel(k);
    for (int i = 0; i < k; i++) rel[i] = i - min;

    vector<int> result_lines;

    for (long long p0 : occs_pos[min]) {
        bool ok = true;
        for (int i = 1; i < k and ok; i++) {
            if (i == min) continue;
            long long need = p0 + rel[i];
            ok = binary_search(occs_pos[i].begin(), occs_pos[i].end(), need);
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

    if (result_lines.empty()) {
        cout << "No se ha encontrado la frase '" << phrase << "' en el texto.\n";
        return;
    }

    cout << "Se ha encontrado la frase '" << phrase << "' en " << result_lines.size() << " línea(s): [";
    for (int i = 0; i < result_lines.size(); i++) {
        if (i != 0) cout << ", ";
        cout << result_lines[i];
    }
    cout << "]\n";
}

template <typename TType>
void search_prefix(const TType& trie, const string& prefix) { 
    vector<WordID> wordIDs = trie.explore_subtree(prefix);

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
    cout << "] nº líneas " << numLines<< "\n";
}

template <typename TType>
void autocomplete(const TType& trie, const string& word) {}