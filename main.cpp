#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm> // Para find()
#include "RadixTrie.hh"
#include "Trie.hh"
#include "Dictionary.hh"
#include "LexicographicSearch.hh"

using namespace std;

#define TRIE 1
#define RADIX_TRIE 2
int typeTrie;

#define ASCII 1
#define UTF8 2
int dictionary;


Trie T;
RadixTrie RT;

Dictionary Postings;

WordID wordID = 0;

template <typename TType>
void search_word(const Trie& trie, const string& word) {
    if (word.empty()) {
        cout << "Consulta vacía.\n";
        return;
    }

    WordID id = trie.search(word);
    if (id == -1) {
        cout << "No se ha encontrado la palabra '" << word << "' en el texto.\n";
        return;
    }

    const auto& occList = Postings.occurrences(id);
    cout << "Se ha encontrado la palabra '" << word << "' en " << occList.size() << " línea(s): [";
    for (int i = 0; i < occList.size(); i++) {
        if (i != 0) cout << ", ";
        cout << occList[i].line;
    }
    cout << "]\n";
}

bool contains_occ(const vector<Match>& vec, int line, long long pos) {
    return binary_search(vec.begin(), vec.end(), Match{line,pos}, MatchLess{});
}

template <typename TType>
void search_phrase(const TType& trie, const string& phrase) {
    vector<WordID> words;
    
    int i = 0, n = phrase.size();
    while (i < n) {
        while (i < n and phrase[i] == ' ') i++;
        int j = i;
        while (j < n and phrase[j] != ' ') j++;
        if (j > i) words.push_back(trie.search(phrase.substr(i, j-i)));
        i = j;
    }

    if (words.empty()) {
        cout << "Consulta vacía.\n";
        return;
    }

    int k = words.sizze(); // k = nº palabras

    vector<const vector<Match>*> occs(k, nullptr);
    for (int i = 0; i < k; i++) {
        if (words[i] == -1) {
            cout << "No se ha encontrado la frase '" << phrase << "' en el texto.\n";
            return;
        }

        const auto& vec = Postings.occurrences(words[i]);
        occs[i] = &vec;
    }

    // min = palabra con menos ocurrencias en el texto
    int min = 0;
    for (int i = 1; i < k; i++) {
        if (occs[i]->size() < occs[min]->size()) min = i;
    }

    vector<int> rel(k);
    for (int i = 0; i < k; i++) rel[i] = i - min;

    vector<int> result_lines;

    const auto& minOcc = *occs[min];
    for (const auto& match : minOcc) {
        const int line = match.line;
        const long long p0 = match.pos;

        bool ok = true;
        for (int i = 0; i < k and ok; i++) {
            if (i == min) continue;
            const long long need = p0 + rel[i];
            of = contains_occ(*occs[i], line, need);
        }
        if (ok) {
            result_lines.push_back(line);
        }
    }

    if (result_lines.empty()) {
        cout << "No se ha encontrado la frase '" << phrase << "' en el texto.\n";
        return;
    }

    cout << "Se ha encontrado la frase '" << phrase << "' en " << result_lines.size() << " línea(s): [";
    for (int i = 0; i < result_lines.size(); i++) {
        if (i != 0) cout << ", ";
        cout << occList[i].line;
    }
    cout << "]\n";
}

template <typename TType>
void search_prefix(const TType& trie, const string& word) {}

template <typename TType>
void autocomplete(const TType& trie, const string& word) {}

// Indexa e inserta una palabra ya normalizada en las estructuras de búsqueda:
//  - indexar la palabra (asociar a wordID)
//  - insertar la palabra en el trie
//  - añadir la tupla (line,pos) a la tabla de ocurrencias
void index_and_insert(const string& word, int line, long long& pos) {
    if (word.empty()) return;

    WordID id = -1;
    switch (typeTrie) {
     case TRIE:
        id = T.search(word); // buscamos si la palabra ya existe
        // Si es la primera ocurrencia de la palabra generamos su wordID y la insertamos
        if (id == -1) {
            T.insert(word, wordID);
            Postings.add(wordID, line, pos);
            wordID++;
        } 
        // Si la palabra ya existía, solo necesitamos añadir la tupla (line,pos) como ocurrencia
        else {
            Postings.add(id, line, pos);
        }
        break;
     // Mismo procedimiento pero para RadixTrie
     case RADIX_TRIE:
        id = RT.search(word);
        if (id == -1) { 
            RT.insert(word, wordID);
            Postings.add(wordID, line, pos);
            wordID++;
        }
        else { 
            Postings.add(id, line, pos);
        }
    }
    pos++; // siguiente posición global
}

// Dada una línea del texto la separa por palabras y las normaliza (minúsculas):
//  - UTF8: nos quedamos con cualquier tipo de caracter excepto espacios
void tokenize_line_ascii(const string& text, int line, long long& pos) {
    string word;

    for (int i = 0; i < text.size(); ++i) {
        char ch = text[i];
        // Vamos construyendo la palabra a medida que no nos encontremos con un espacio
        if (isalpha(ch)) { // comprobamos que es ASCII
            word.push_back(tolower(ch)); // normalizamos minúsculas
        }
        // Si nos encontramos un espacio, hemos leído una palabra y la debemos indexar e insertar
        else if (ch == ' ') {
            index_and_insert(word, line, pos);
            word.clear(); // vaciar la palabra
            word.shrink_to_fit();
        }
    }

    // Procesamos última palabra (getline no incluye '\n')
    index_and_insert(word, line, pos);
    word.clear(); // vaciar la palabra
    word.shrink_to_fit();
}

// Dada una línea del texto la separa por palabras y las normaliza (minúsculas):
//  - UTF8: nos quedamos con cualquier tipo de caracter excepto espacios
void tokenize_line_utf8(const string& text, int line, long long& pos) {
    string word;

    for (int i = 0; i < text.size(); i++) {
        char ch = text[i];
        // Si nos encontramos un espacio, hemos leído una palabra y la debemos indexar e insertar
        if (ch == ' ') {
            index_and_insert(word, line, pos);
            word.clear(); // vaciar palabra
            word.shrink_to_fit();
        }
        // Vamos construyendo la palabra a medida que no nos encontremos con un espacio, 
        else {
            if (ch < 128 && isalpha(ch)) word.push_back(tolower(ch)); // normalizamos a minúsculas letras (a-z)
            else word.push_back(ch);
        }
    }

    // Procesamos última palabra (getline no incluye '\n')
    index_and_insert(word, line, pos);
    word.clear(); // vaciar la palabra
    word.shrink_to_fit();
}

// Función para leer el archivo y agregar las palabras al Trie
void read_file(string route) {
    ifstream file;

    // Abrimos el fichero
    file.open(route, ios::in); 
    if (file.fail()) { // error al abrirlo
        cout << "Error: el archivo no se ha podido abrir" << endl;
        exit(1);
    }
    
    int line = 1; // contador de líneas en el texto
    long long pos = 1; // contador de palabras en el texto (indica la posición general de una palabra)
    
    // Recorremos el texto por líneas y las tokenizamos
    string text;
    while (getline(file, text)) {
        switch (dictionary) {
         case ASCII: tokenize_line_ascii(text, line, pos); break;
         case UTF8: tokenize_line_utf8(text, line, pos);
        }
        line++; // siguiente línea
    }

    // Cerramos el fichero
    file.close();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Uso: " << argv[0] << " [archivo1.txt]\n";
        return 0;
    }

    // Decidir que diccionario se usará (ASCII/UTF8)
    cout << "¿Que diccionario quieres usar?\n"
            " Para usar ASCII pulse (1):\n"
            " Para usar UTF-8 pulse (2):\n";
    cout << "¿Cuál quieres usar?: ";
    cin >> dictionary;

    cout << endl;

    // Decidir que tipo de trie se usará
    cout << "¿Que tipo de trie quieres usar?\n"
            " Para usar Basic Trie pulse (1)\n"
            " Para usar Radix Trie pulse (2)\n";
    cout << "¿Cuál quieres usar?: ";
    cin >> typeTrie;

    // Leer el archivo de entrada y generar estructuras de búsqueda
    auto t0 = std::chrono::high_resolution_clock::now();
    read_file(argv[1]);  // Leer el archivo
    auto t1 = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    cout << "Generar el árbol ha tardado " << ns << " ms" << endl;

    string input; 
    cin.ignore();  // Limpiar el buffer de entrada
    cout << "¿Qué palabra, prefijo o frase quieres consultar? (minúsculas)\n -> "; 
    while (getline(cin, input)) {
        if (input.find(' ') != string::npos) { // Si la consulta contiene espacios, es una frase
            switch (typeTrie) {
             case TRIE: search_phrase(T, input); break;
             case RADIX_TRIE: search_phrase(RT, input); break;
            }
        }
        else { // Si no contiene espacios, es una palabra o prefijo
            switch (typeTrie) {
             case TRIE: search_word(T, input); break;
             case RADIX_TRIE: search_word(RT, input); break;
            }
        }
        cout << "-> ";
    }
    
    return 0;
}