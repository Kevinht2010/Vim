#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

class Highlight {
  int l,r;
  int lineNumber;
 public:
  Highlight();
  void setHighlight(int left, int right, int line);
  int getLeft() const;
  int getRight() const;
  int getLine() const;
};

#endif
