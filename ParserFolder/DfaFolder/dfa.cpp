#include <set>
#include <string>
#include <iostream>
#include <vector>
#include <set>

#include "dfa.h"
#include "../../CommonFolder/common.h"

using std::set;
using std::string;
using std::vector;
using std::set;


line::line(int pos,symbol&& sym,set<string>&& lk){
    dotPosition = pos;
    prod = sym;
    lookahead = lk;
}
line::line(int pos,symbol& sym,set<string>&& lk){
    dotPosition = pos;
    prod = sym;
    lookahead = lk;
}
line::line(int pos,symbol& sym,set<string>& lk){
    dotPosition = pos;
    prod = sym;
    lookahead = lk;
}
line::line(int pos,symbol&& sym,set<string>& lk){
    dotPosition = pos;
    prod = sym;
    lookahead = lk;
}

state::state(){

}
state::state(int n, line l){
    stateNum = n;
    productions.emplace_back(l);
    rank = status::temp;
}
std::ostream& operator<< (std::ostream& out, const state& s){
    //num status
    out << "STATE:" << s.stateNum << "\t";
    switch (s.rank)
    {
        case status::accept:
            out << "ACCEPTING";
            break;
        case status::closed:
            out << "CLOSED";
            break;
        default:
            break;
    }
    out << std::endl;
    //print rules with dots
    for(const auto& l: s.productions){
        //looping over symbols first production rule
        out << l.prod.name << " -> ";
        for(int i=0;i<l.prod.production_rule[0].size();i+=1){
            if(i==l.dotPosition){
                out << "o";
            }
            out << l.prod.production_rule[0][i];
        }
        if(l.prod.production_rule[0].size()==l.dotPosition){
            out << "o";
        }
        //print look ahead
        out << " {";
        for(const auto& i: l.lookahead){
            out << i << " ";
        }
        out << "}" << std::endl;
    }
    //print transition connections //TODO: fix
    out << "CONNECTIONS:\t";
    for(const auto& t: s.transitions){
        out << t.first;
    }
    out << std::endl << std::endl;

    return out;
}

Dfa::Dfa(const unordered_map<string,symbol>& g){
    grammar = g;

    line augmentedStart = line(0,symbol("S'",{"start"}),{"$"});
    //start = state(0,augmentedStart);
    startPtr = std::make_unique<state>(0,augmentedStart);
    //startPtr = std::make_unique<state>(start);

    //closure(start);

}
Dfa::~Dfa(){
    line augmentedStart = line(0,symbol("S'",{"start"}),{"$"});
    //start = state(0,augmentedStart);
    startPtr = std::make_unique<state>(0,augmentedStart);
}

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

void Dfa::closure(state s){
    for(int i=0,n=s.productions.size();i<n;i+=1){
        //0 is first rule
        if(s.productions[i].dotPosition < s.productions[i].prod.production_rule[0].size())
        {
            string currentDotPosString = s.productions[i].prod.production_rule[0][s.productions[i].dotPosition];
            if(!grammar[currentDotPosString].isTerminal){
                for(const auto& prods: grammar[currentDotPosString].production_rule){
                    //get lookahead
                    set<string> x = i==0 ? s.productions[0].lookahead : s.productions[i-1].lookahead;
                    if(s.productions[i].dotPosition+1 < s.productions[i].prod.production_rule[0].size()){
                        x = first(s.productions[i].prod.production_rule[0][s.productions[i].dotPosition+1]);
                    }
                    line newLine = line(0,symbol(currentDotPosString,vector<string>(prods)),x);
                    
                    s.productions.emplace_back(newLine);
                    n+=1;
                }
            }
        }
        else{
            if(s.productions.size()==1){
                //if s->Abc o , terminate early and set rank to closed
                s.rank = status::closed;
                break;
            }
        }
    }

    std::cout << s;
}
//HERE
void Dfa::goToState(state){

}
