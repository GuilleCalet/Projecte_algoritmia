#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm> // Para find()
#include "RadixTrie.hh"
#include "Trie.hh"

using namespace std;

Trie T;
RadixTrie T2;
int eleccion;

// Función para leer el archivo y agregar las palabras al Trie
void read_file(string route) {
    ifstream file;
    file.open(route, ios::in); 

    if (file.fail()) {
        cout << "Error: el archivo no se ha podido abrir" << endl;
        exit(1);
    }

    string text;
    int line = 0;
    size_t word_position = 0;
    while (getline(file, text)) {
        line++;
        string word;  

        for (char ch : text) {
            if (isalpha(ch)) word.push_back(tolower(ch));
            else if (ch == ' ' and word.size() > 0) {
                word_position++;
                if (eleccion == 1) T.insert(word, line, word_position);  // Usar T (Basic Trie)
                else T2.insert(word, line, word_position);  // Usar T2 (Radix Trie)
                word.clear();
                word.shrink_to_fit();
            }
        }

        if (word.size() > 0) {
            word_position++;
            if (eleccion == 1) T.insert(word, line, word_position);
            else T2.insert(word, line, word_position);
        }
    }

    file.close(); 
    return;
}

template <typename TType>
vector<pair<int, size_t>> search_word(TType& trie, const string& word) {
    long long ns = 0;
    // Buscamos la palabra y obtenemos las posiciones (línea, posición)
    vector<pair<int, size_t>> positions = trie.search(word, ns);  
    return positions;
}

template <typename TType>
vector<int> search_phrase(TType& trie, const string& phrase) {
    vector<int> result;
    vector<string> words;  // Dividir la frase en palabras
    string word;
    size_t pos = 0;

    // Dividir la frase en palabras
    while (pos < phrase.size()) {
        if (isspace(phrase[pos])) {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word.push_back(phrase[pos]);  // Añadir carácter a la palabra
        }
        ++pos;
    }
    if (!word.empty()) words.push_back(word);  // Añadir la última palabra

    // Buscar las posiciones de la primera palabra en el Trie
    vector<pair<int, size_t>> candidate_positions = search_word(trie, words[0]);  // Buscar la primera palabra

    // Ahora verificamos que las siguientes palabras estén en el orden correcto y seguidas
    for (size_t i = 1; i < words.size(); ++i) {
        vector<pair<int, size_t>> word_positions = search_word(trie, words[i]);
        vector<pair<int, size_t>> temp_positions;

        // Comprobamos que cada posición candidata contenga la palabra actual en la posición correcta
        for (auto& prev_pos : candidate_positions) {
            size_t prev_line = prev_pos.first;
            size_t prev_position = prev_pos.second;
            bool found_in_order = false;

            // Buscar la palabra en la línea y asegurarse de que esté después de la posición anterior
            for (auto& word_pos : word_positions) {
                if ( word_pos.second == (prev_position + 1)) {
                    temp_positions.push_back(word_pos);
                    found_in_order = true;
                    break;
                }
            }

            if (!found_in_order) {
                continue;  // Si no encontramos la palabra en el orden correcto, la descartamos
            }
        }

        candidate_positions = temp_positions;  // Actualizamos las posiciones candidatas
    }

    // Recopilamos las líneas válidas de las posiciones
    for (auto& pos : candidate_positions) {
        result.push_back(pos.first);  // Agregar la línea a los resultados
    }

    return result;
}




int main(int argc, char* argv[]) {
    cout << "Para usar Basic Trie pulse (1):" << endl;
    cout << "Para usar Radix Trie pulse (2):" << endl;
    cout << "¿Cuál quieres usar?: ";
    cin >> eleccion;
    
    if (argc < 2) {
        cout << "Uso: " << argv[0] << " [archivo1.txt]\n";
        return 0;
    } else {
        auto t0 = std::chrono::high_resolution_clock::now();
        read_file(argv[1]);  // Leer el archivo
        auto t1 = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        cout << "Generar el árbol ha tardado " << ns << " ms" << endl;
    }

    string input;
    cout << "¿Qué palabra o frase quieres consultar? (minúsculas)\n -> "; 
    cin.ignore();  // Limpiar el buffer de entrada
    while (getline(cin, input)) {  // Usamos getline para leer toda la línea
        if (input.find(" ") != string::npos) {  // Si la consulta contiene espacios, es una frase
            vector<int> occur;
            if (eleccion == 1) {
                occur = search_phrase(T, input);  // Usar T (Basic Trie)
            } else {
                occur = search_phrase(T2, input);  // Usar T2 (Radix Trie)
            }
            if (occur.size() == 0) {
                cout << "No se ha encontrado la frase \"" << input << "\"!!\n";
            } else {
                cout << "La frase \"" << input << "\" se ha encontrado en las líneas: [";
                for (int i = 0; i < occur.size(); i++) {
                    if (i > 0) cout << ", ";
                    cout << occur[i];
                }
                cout << "] num: " << occur.size() << "\n";
            }
        } else {  // Si no contiene espacios, es una palabra
            long long ns = 0;
            vector<pair<int, size_t>> occur;
            if (eleccion == 1) occur = T.search(input, ns);  // Usar T (Basic Trie)
            else occur = T2.search(input, ns);  // Usar T2 (Radix Trie)
            
            if (occur.size() == 0) cout << "No se ha encontrado la palabra \"" << input << "\"!!\n";
            else {
                cout << "La palabra \"" << input << "\" se ha encontrado en las líneas: [";
                for (int i = 0; i < occur.size(); i++) {
                    if (i > 0) cout << ", ";
                    cout << occur[i].first;
                }
                cout << "] num: " << occur.size() << "\n";
            }
            cout << "La búsqueda ha tardado " << ns << " ns" << endl;
        }
        cout << "-> ";
    }
    
    return 0;
}