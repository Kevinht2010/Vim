#include "scanner.h"
#include "file.h"
#include <iostream>
#include <fstream>
#include <assert.h>
using namespace std;
Scanner::Scanner() {
  readKeyWords();
  createEdge(START, isalpha, ID);
  createEdge(ID, isalnum, ID);

  createEdge(START, isdigit, NUM);
  createEdge(NUM, isdigit, NUM);

  createEdge(START, "#", DIRECTIVE);
  createEveryEdge(DIRECTIVE, DIRECTIVE);

  createEdge(START, "\"", MID_STRING);
  createEdge(MID_STRING, "\"", STRING);
  createLeftOver(MID_STRING, MID_STRING);

  createEdge(START, "/", SLASH);
  createEmptyEdge(ELSE);
  createEdge(SLASH, "/", COMMENT);
  createEveryEdge(COMMENT, COMMENT);

  createLeftOver(START, ELSE);
}

void Scanner::readKeyWords() {
  File tmpFile{"src/keywords.txt"};

  std::vector<std::string> tmpKeyword;
  tmpFile.readFileSpaces(tmpKeyword);
  assert(tmpKeyword.size() != 0);
  for (size_t i = 0; i < tmpKeyword.size(); ++i) {
    std::cout<<tmpKeyword[i]<<'\n';
    keyWord.insert(tmpKeyword[i]);
  }
}

void Scanner::createEveryEdge(Node oldState, Node newState) {
  for (int c = 0; c < 128; ++c) {
    edges[oldState][c] = newState;
  }
}

void Scanner::createEmptyEdge(Node state) {
  edges[state] = std::map<char, Node>();
}

void Scanner::createLeftOver(Node oldState, Node newState) {
  for (int c = 0; c < 128; ++c) {
    if (edges[oldState].find(c) == edges[oldState].end()) {
      edges[oldState][c] = newState;
    }
  }
}

void Scanner::createEdge(Node oldState, const std::string &chars, Node newState) {
  for (char c : chars) {
    edges[oldState][c] = newState;
  }
}

void Scanner::createEdge(Node oldState, int (*test)(int), Node newState) {
  for (int c = 0; c < 128; ++c) {
    if (test(c)) {
      edges[oldState][c] = newState;
    }
  }
}

void Scanner::appendVectors(std::vector<Node> &result, Node state, const std::string &s) {
  if (state == ID) {
    if (keyWord.find(s) != keyWord.end()) {
      state = KEYWORD;
    }
  }
  for(size_t i = 0; i < s.size(); ++i) {
    result.push_back(state);
  }
}

std::vector<Scanner::Node> Scanner::scan(const std::string &s) {
  std::vector<Scanner::Node> result;
  std::string tmpString = "";
  Node state = START;
  for (size_t i = 0; i < s.size(); ++i) {
    if (edges[state].find(s[i]) != edges[state].end()) {
      state = edges[state][s[i]];
      tmpString += s[i];
    }
    else {
      --i;
      appendVectors(result, state, tmpString);
      state = START;
      tmpString = "";
    }
  }
  if (s.size() != 0) {
    appendVectors(result, state, tmpString);
  }
  return result;
}
