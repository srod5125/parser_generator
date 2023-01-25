#include "dfa.h"

line::line(int i,string&& n,vector<string>&& prod,set<string>&& lk){
    dotPosition = i;
    tag = n;
    production_list = prod;
    lookahead = lk;
}

state::state(){
    
}
state::state(int n, line l){
}

Dfa::Dfa(const unordered_map<string,symbol>& g){
    grammar = g;

    line augmentedStart = line(-1,"S'",{"S"},{"$"});
    start = state(0,augmentedStart);

    closure(start);

}