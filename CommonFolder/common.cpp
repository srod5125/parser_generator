#include <string>
#include <vector>


using std::string;
using std::vector;


#include "common.h"

token::token() : tag("EMPTY"), terminal("") {} // empty token init
token::token(const string& t,const string& ter) : tag(t), terminal(terminal) { } //std::cout<<"constructor "<<terminal<<std::endl;}
token::token(const string&& t,const string&& ter) : tag{t}, terminal{terminal} {}
token::token(const string&& t) : tag{t}, terminal{t} {} // same

symbol::symbol() : isTerminal{true}, name{"EMPTY"}, t{}, production_rule{} {}
symbol::symbol(const string&& n, const vector<vector<string>>&& rules){
    isTerminal=false;
    name=n;
    production_rule = rules;
}
symbol::symbol(const string&& n, const string&& t){
    isTerminal=true;
    name=n;
    this->t.tag = n;
    this->t.terminal = t;
}
symbol::symbol(token& t){
    isTerminal=true;
    name=t.tag;
    t=t;
}
symbol::symbol(token&& t){
    isTerminal=true;
    name=t.tag;
    t=t;
}

