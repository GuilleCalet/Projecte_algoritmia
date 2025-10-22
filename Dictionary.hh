#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

typedef int WordID;

class Dictionary {
public:

  struct Match {
    int line;
    long long pos;

    bool operator<(const Match& other) const {
      return ((line < other.line) or (line == other.line and pos < other.pos));
    }
    bool operator==(const Match& other) const {
      return (line == other.line and pos == other.pos);
    }
  };

private:
  unordered_map<WordID,vector<Match>> table_;
  
public:
  Dictionary();
  ~Dictionary();

  void add(WordID id, int line, long long pos) {
    table_[id].push_back(Match{line,pos});
  }

  bool has(WordID id) const {
    return (table_.find(id) != table_.end());
  }

  const vector<Match>& occurrences(WordID id) const {
    auto it = table_.find(id);
    return (it == table_.end()) ? vector<Match>() : it->second;
  }

};