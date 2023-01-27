#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <string>

#include "dfa.h"
#include "../../CommonFolder/common.h"

using std::unordered_map;
using std::unordered_set;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;
using std::string;

line::line(int pos,symbol&& sym, unordered_set<string>&& lk){
    dotPosition = pos;
    prod = sym;
    lookahead = lk;
}


Dfa::Dfa(){
    line l = line(0,symbol(),unordered_set<string>({"sdf","sdf"}));
}

Dfa::~Dfa(){
}