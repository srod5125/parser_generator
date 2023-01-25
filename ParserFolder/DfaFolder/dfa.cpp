#include <set>
#include <string>

#include "dfa.h"

using std::set;
using std::string;

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

    line augmentedStart = line(-1,"S'",{"start"},{"$"});
    start = state(0,augmentedStart);

    //closure(start);

}
Dfa::~Dfa(){}

set<string> Dfa::first(const string& sym){
    if(grammar[sym].isTerminal){ //handles null case
        set<string> x{sym};
        return x;
    }
    else {
        set<string> x;
        for(const auto& seq : grammar[sym].production_rule){
            if(!seq.empty()){
                for(const auto& i: seq){
                    if(grammar[i].isTerminal){
                        x.insert(i);
                        break;
                    }
                    else{
                        //union
                        set<string> tmp = first(i);
                        x.insert(tmp.begin(),tmp.end());
                    }
                }
            }
        }
        return x;
    }
}
