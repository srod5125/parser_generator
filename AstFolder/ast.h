#ifndef _AST_HEADER_
#define _AST_HEADER_

#include <iostream>
#include <string>
#include <vector>
#include <memory>

using std::string;
using std::vector;
using std::shared_ptr;
using std::unique_ptr;

//TODO: lark exposes data and children

struct block{
  string val;
  vector< shared_ptr<block> > connections;

  block();
  block(string);
};

class Ast {
  private:
  public:
    Ast();
    ~Ast();
    shared_ptr<block> head;

    friend std::ostream& operator<< (std::ostream&, const Ast&);
};

#endif