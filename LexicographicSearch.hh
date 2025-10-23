#include <vector>
#include <string>
#include "Dictionary.hh"

using namespace std;

template <typename TType>
void search_word(TType& trie, const string& word) {
  long long ns = 0;
  // Buscamos la palabra y obtenemos las posiciones (línea, posición)
  vector<Match> positions = trie.search(word, ns);  
  return positions;
}

template <typename TType>
void search_phrase(TType& trie, const string& phrase) {
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