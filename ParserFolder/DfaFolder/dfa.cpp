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
                if(grammar[seq[0]].isTerminal){
                    x.insert(seq[0]);
                }
                else{

                    if(!hasEpsilonProduction(seq[0])){
                        set<string> tmp = first(seq[0]);
                        x.insert(tmp.begin(),tmp.end());
                    }
                    else {
                        set<string> tmp = first(seq[0]);
                        tmp.erase("EMPTY");
                        x.insert(tmp.begin(),tmp.end());

                        for(int i=1;i<seq.size();i+=1){
                            if(grammar[seq[i]].isTerminal){
                                x.insert(seq[i]);
                                break;
                            }
                            else{
                                set<string> tmp = first(seq[i]);
                                x.insert(tmp.begin(),tmp.end());
                            }
                        }
                    }
                }
            }
        }
        return x;
    }
}

bool Dfa::hasEpsilonProduction(string nonterminal){
    bool hasEpsilonProduction{false};
    for(const auto& i: grammar[nonterminal].production_rule){
        if(!i.empty()){
            if(i.size()==1 && i[0]=="EMPTY"){
                hasEpsilonProduction=true;
            }
        }
    }
    return hasEpsilonProduction;
}
