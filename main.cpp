#include <iostream>
#include <fstream>
#include <chrono>

#include "RadixTrie.hh"
#include "Trie.hh"
#include "TextSearch.hh"

using namespace std;

#define TRIE 1
#define RADIX_TRIE 2
int typeTrie;

#define ASCII 1
#define UTF8 2
int dictionary;


Trie T;
RadixTrie RT;

WordID wordID = 0;


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
            T.insert(word, wordID, line, pos);
            wordID++;
        } 
        // Si la palabra ya existía, solo necesitamos añadir la tupla (line,pos) como ocurrencia
        else {
            T.add_occur(id, line, pos);
        }
        break;
     // Mismo procedimiento pero para RadixTrie
     case RADIX_TRIE:
        id = RT.search(word);
        if (id == -1) { 
            RT.insert(word, wordID, line, pos);
            wordID++;
        }
        else { 
            RT.add_occur(id, line, pos);
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
            "- Para usar ASCII pulse (1):\n"
            "- Para usar UTF-8 pulse (2):\n";
    cout << "¿Cuál quieres usar?: ";
    cin >> dictionary;

    cout << endl;

    // Decidir que tipo de trie se usará
    cout << "¿Que tipo de trie quieres usar?\n"
            "- Para usar Basic Trie pulse (1)\n"
            "- Para usar Radix Trie pulse (2)\n";
    cout << "¿Cuál quieres usar?: ";
    cin >> typeTrie;

    cout << endl;

    // Leer el archivo de entrada y generar estructuras de búsqueda
    auto t0 = std::chrono::high_resolution_clock::now();
    read_file(argv[1]);  // Leer el archivo
    auto t1 = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    cout << "Generar el árbol ha tardado " << ns << " ms" << endl;

    switch (typeTrie) {
        case TRIE:
            cout << "Trie: nodos = " << T.node_count()
                << " | memoria ~ " << T.memory_bytes_estimate() << " bytes\n";
            break;
        case RADIX_TRIE:
            cout << "RadixTrie: nodos = " << RT.node_count()
                << " | memoria ~ " << RT.memory_bytes_estimate() << " bytes\n";
            break;
    }

    cout << endl;

    string input; 
    cin.ignore();  // Limpiar el buffer de entrada
    cout << "¿Qué palabra, prefijo o frase quieres consultar? (minúsculas)\n"
            "- Para consultar una palabra escribe primero el commando word>\n"
            "- Para consultar un prefijo escribe primero el commando prefix>\n"
            "- Para consultar una frase escribe primero el commando phrase>\n"
            "-> "; 
    string consult;
    while (cin >> consult) {
        
        if (consult == "phrase>") { // Si la consulta contiene espacios, es una frase
            getline(cin, input);
            switch (typeTrie) {
             case TRIE: search_phrase(T, input); break;
             case RADIX_TRIE: search_phrase(RT, input); break;
            }
        }
        else if (consult == "word>") { // Si no contiene espacios, es una palabra o prefijo
            cin >> input;
            switch (typeTrie) {
             case TRIE: search_word(T, input); break;
             case RADIX_TRIE: search_word(RT, input); break;
            }
        }
        else if (consult == "prefix>") {
            cin >> input;
            switch (typeTrie) {
             case TRIE: search_prefix(T, input); break;
             case RADIX_TRIE: search_prefix(RT, input); break;
            }
        }
        cout << "-> ";
    }
    
    return 0;
}