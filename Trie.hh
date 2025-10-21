  #include <iostream>
  #include <string>
  #include <vector>

  using namespace std;

  class Trie {
  private:

    // Estructura de un nodo del Trie.
    struct Node {
      vector<Node*> children{26, nullptr}; // punteros a los hijos
      vector<int> lines; // en que líneas aparece la palabra
    };

    Node* root = nullptr; // raíz del Trie


    /** @brief Converteix un caràcter a l'index corresponent normalitzat
    \pre El caràcter ch és una lletra da l'alfabet
    \post Retorna el caràcter ch en la seva forma minúscula
    */
    static int ctoi(char ch);

    /** @brief Comproba si un node té un fill associat a un caràcter determinat
    \pre 'node' és un punter vàlid del Trie, 'ch' és una lletra de l'alfabet normalitzada
    \post Retorna cert si el node té un fill 'ch', fals en cas contrari
    */
    static bool existsChild(const Node* node, char ch);
    static bool existsChild(const Node* node, int idx);

    /** @brief Comproba si un node té un algun fill
    \pre 'node' es un punter vàlid a un node del trie
    \post Retorna cert si el node té almenys un fill, fals si és una fulla
    */
    static bool hasChildren(const Node* node); // Comprueba si un nodo tiene hijos

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
    void insert(const string& word, int line);

    /** @brief Elimina la paraula "word" del trie
    \pre  La string word només té caràcters alfabètics previament nomalitzats
    \post Si la paraula estava al trie, l'elimina, si no estava retorna fals
    */
    bool eliminate(const string& word);
    bool eliminate(const string& word, int line);

    /** @brief Comproba si el Trie conté la paraula "word"
    \pre La string word només té caràcters alfabètics previament nomalitzats
    \post Retorna cert si la paraula "word" és al trie, retorna fals en cas contrari
    */
    vector<int> search(const string& word); 
  };