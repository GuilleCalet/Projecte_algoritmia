  #include <iostream>
  #include <string>
  #include <vector>
  #include <array>
  #include <unordered_map>


  using namespace std;

  class Trie {
  private:

    // Estructura de un nodo del Trie.
    struct Node {
      unordered_map<char, Node*> children;// punteros a los hijos
      int wordID;
    };

    Node* root = nullptr; // raíz del Trie


    /** @brief Converteix un caràcter a l'index corresponent normalitzat
    \pre El caràcter ch és una lletra da l'alfabet
    \post Retorna el caràcter ch en la seva forma minúscula
    */
    static int ctoi(char ch);

  public:
    /** @brief Crea un Trie buit
    \pre cert
    \post El resultat és un Trie inicialitzat sense cap paraula
    */
    Trie();

    /** @brief Destrueix el trie i allibera la seva memòria
    \pre cert
    \post El trie queda eliminat i la memòria dels nodes queda alliberada
    */
    ~Trie(); 

    /** @brief Insereix una paraula dins del trie
    \pre La string word només té caràcters alfabètics previament nomalitzats
    \post El trie conté la paraula "word", si ja contenia la paraula, incrementa el seu contador de repeticions
    */
    void insert(const string& word, int ID);

    /** @brief Comproba si el Trie conté la paraula "word"
    \pre La string word només té caràcters alfabètics previament nomalitzats
    \post Retorna cert si la paraula "word" és al trie, retorna fals en cas contrari
    */
    int search(const string& word, long long&ns); 
  };