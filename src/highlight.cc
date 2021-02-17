#include "highlight.h"

Highlight::Highlight() : l{-1}, r{-1}, lineNumber{-1} {}

void Highlight::setHighlight(int left, int right, int line) {
  l = left;
  r = right;
  lineNumber = line;
}

int Highlight::getLeft() const {
  return l;
}

int Highlight::getRight() const {
  return r;
}

int Highlight::getLine() const {
  return lineNumber;
}
