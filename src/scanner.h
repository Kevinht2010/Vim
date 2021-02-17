#ifndef SCANNER_H
#define SCANNER_H

#include <vector>
#include <string>
#include <set>
#include <map>

class Scanner {
 public:
  enum Node {
    //Accepting States
    ID = 0,
    STRING,
    NUM,
    COMMENT,
    DIRECTIVE,
    KEYWORD,
    ELSE,
    //
    START,
    MID_STRING,
    SLASH
  };
 private:
  std::map<Node,std::map<char, Node>> edges;
  std::set<std::string> keyWord;
  void createEmptyEdge(Node state);
  void createEveryEdge(Node oldState, Node newState);
  void readKeyWords();

  void createLeftOver(Node oldState, Node newState);

  void createEdge(Node oldState, const std::string &chars, Node newState);

  void createEdge(Node oldState, int (*test)(int), Node newState);

  void appendVectors(std::vector<Node> &result, Node state, const std::string &s);
 public:
  Scanner();
  std::vector<Node> scan(const std::string &s);

  ~Scanner() = default;
};

#endif
