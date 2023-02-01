#include <set>
#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <utility>

#include "dfa.h"
#include "../../CommonFolder/common.h"

using std::set;
using std::string;
using std::vector;
using std::set;
using std::pair;

#define LOG(msg) std::cout << msg << std::endl;

// ----------- line ---------
line::line(int pos,symbol&& sym,unordered_set<string>&& lk){
    dotPosition = pos;
    prod = sym;
    lookahead = lk;
}
line::line(int pos,symbol& sym,unordered_set<string>&& lk){
    dotPosition = pos;
    prod = sym;
    lookahead = lk;
}
line::line(int pos,symbol& sym,unordered_set<string>& lk){
    dotPosition = pos;
    prod = sym;
    lookahead = lk;
}
line::line(int pos,symbol&& sym,unordered_set<string>& lk){
    dotPosition = pos;
    prod = sym;
    lookahead = lk;
}
std::size_t line::hash::operator()( const line& l) const{
    std::size_t acc = std::hash<string>()(l.prod.name);
    std::size_t seed = l.prod.production_rule[0].size();
    acc ^= l.dotPosition + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    for(const auto& el: l.prod.production_rule[0]){
        acc ^= std::hash<string>()(el) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    for(const auto& el: l.lookahead){
        acc ^= std::hash<string>()(el) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return acc;
}
bool operator==(const line& lhs, const line& rhs){
    bool same = lhs.prod.name == rhs.prod.name;
    if(!same) {return false;}
    same = lhs.prod.production_rule[0].size() == rhs.prod.production_rule[0].size();
    if(!same) {return false;}
    for(int i=0;i<lhs.prod.production_rule[0].size() && i<rhs.prod.production_rule[0].size(); i+=1){
        same = lhs.prod.production_rule[0][i] == rhs.prod.production_rule[0][i];
        if(!same) {return false;}
    }
    same = lhs.lookahead.size() == rhs.lookahead.size();
    if(!same) {return false;}
    same = lhs.lookahead == rhs.lookahead;
    if(!same) {return false;}
    return true;
}
std::ostream& operator<< (std::ostream& out, const line& l){
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

    return out;
}
// -------------- state ----------
state::state() : stateNum{0}, rank{status::intermediate}, productions{}, transitions{} { }
state::state(int n, line l) : stateNum{0}, rank{status::intermediate}, transitions{} {
    stateNum = n;
    productions.insert(l);
}
state::state(const unordered_set<line,line::hash>& lineSet)
: stateNum{0}, rank{status::intermediate}, transitions{} {
    productions = lineSet;
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
        case status::intermediate:
            out << "INTERMEDIATE";
            break;
        default:
            break;
    }
    out << std::endl;
    //print rules with dots
    for(const auto& l: s.productions){
        //looping over symbols first production rule
        out << l;
    }
    //print transition connections //TODO: fix
    out << "CONNECTIONS:\t";
    for(const auto& t: s.transitions){
        out << t.first;
    }
    out << std::endl << std::endl;

    return out;
}
//-------------- helpers -------

std::size_t initProdsHash::operator()(const unordered_set<line,line::hash>& lSet) const{
    std::size_t acc;
    std::size_t seed;
    for(const auto& l: lSet){
        seed = l.prod.production_rule[0].size();
        acc ^= line::hash()(l) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return acc;
}
bool initProdsEqual::operator()(const unordered_set<line,line::hash>& lhs,const unordered_set<line,line::hash>& rhs) const{
    return lhs == rhs;
}


// -------------- dfa ----------
Dfa::Dfa():grammar{}, globalStateNum{0} {

}
Dfa::Dfa(const unordered_map<string,symbol>& g): globalStateNum{0} {
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

unordered_set<string> Dfa::first(const string& sym){
    if(grammar[sym].isTerminal){ //handles null case
        unordered_set<string> x{sym};
        return x;
    }
    else {
        unordered_set<string> x;
        for(const auto& seq : grammar[sym].production_rule){
            if(!seq.empty()){
                if(grammar[seq[0]].isTerminal){
                    x.insert(seq[0]);
                }
                else{

                    if(!hasEpsilonProduction(seq[0])){
                        unordered_set<string> tmp = first(seq[0]);
                        x.insert(tmp.begin(),tmp.end());
                    }
                    else {
                        unordered_set<string> tmp = first(seq[0]);
                        tmp.erase("EMPTY");
                        x.insert(tmp.begin(),tmp.end());

                        for(int i=1;i<seq.size();i+=1){
                            if(grammar[seq[i]].isTerminal){
                                x.insert(seq[i]);
                                break;
                            }
                            else{
                                unordered_set<string> tmp = first(seq[i]);
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
state Dfa::closure(unordered_set<line,line::hash> lineSet){
    
    //check out early if is size of 1 and closed or accepting
    if(lineSet.size()==1){
        state s = state(lineSet);
        auto lineSetIter = lineSet.begin();
        if(lineSetIter->dotPosition >= lineSetIter->prod.production_rule[0].size()){ // s-> aAb o
            s.rank = status::closed;
            if(lineSetIter->prod.name == "S'" || lineSetIter->prod.name=="AUGMENTED_START"){
                s.rank = status::accept;
            }
            //std::cout << s;
            return s;
        }
        
    }
    

    unordered_set<line,line::hash> aux;
    struct pairHash {
        size_t operator()(const pair<string,int> i) const {
            return std::hash<string>()(i.first) ^ std::hash<int>()(i.second);
        }
    };
    unordered_set<pair<string,int>,pairHash> alreadySeen;
    auto lineSetIter{lineSet.begin()};
    bool allTerminals{true};
    bool encounteredAcceptCondition{false};

    while(!lineSet.empty()){
        lineSetIter = lineSet.begin();

        if(lineSetIter->dotPosition < lineSetIter->prod.production_rule[0].size())
        {
            string currentDotPosString{lineSetIter->prod.production_rule[0][lineSetIter->dotPosition]};
            
            if(!grammar[currentDotPosString].isTerminal){
                //check if production has already been added at this dotposition
                if(alreadySeen.find( {currentDotPosString,lineSetIter->dotPosition} ) == alreadySeen.end()){ //not present

                    for(const auto& prods: grammar[currentDotPosString].production_rule){
                        //get lookahead
                        unordered_set<string> x = lineSetIter->lookahead;
                        if(lineSetIter->dotPosition+1 < lineSetIter->prod.production_rule[0].size()){
                            x = first(lineSetIter->prod.production_rule[0][lineSetIter->dotPosition+1]);
                        }
                        line newLine = line(0,symbol(currentDotPosString,vector<string>(prods)),x);
                        //introduce new memebers
                        lineSet.insert(newLine);
                    }
                    alreadySeen.insert({currentDotPosString,lineSetIter->dotPosition}); // insert already seen
                }
                allTerminals = false;
            }
        }
        else
        {
            //hits here when dot position is beyond production length
            encounteredAcceptCondition = lineSetIter->prod.name == "S'" || lineSetIter->prod.name=="AUGMENTED_START";
        }
        //delete current memeber
        aux.insert(*lineSetIter);
        lineSet.erase(lineSetIter);
    }

    state sI = state(aux);
    sI.rank = allTerminals ? status::closed : status::intermediate;
    sI.rank = encounteredAcceptCondition ? status::accept : sI.rank;
    //LOG("hit here")
    //std::cout << sI;
    return sI;

}
//goto to transitions for state
void Dfa::goToState(state& s){
    std::cin.get();
    std::cout << s;
    //if state was already constructed set pointer to that
    //set transition to string -> state

    unordered_map< string, unordered_set<line,line::hash> > produtionsAtDotPos;
    //collect set of lines with equal dot position strings
    for(const auto& l : s.productions){
        if(l.dotPosition<l.prod.production_rule[0].size()){
            produtionsAtDotPos[l.prod.production_rule[0][l.dotPosition]].insert(l);
        } 
    }
    // std::cout << "lines in current goto" << std::endl;
    // for(const auto& [prodName, setOfProds]: produtionsAtDotPos){
    //     std::cout << prodName << " ";
    //     for(const auto& l: setOfProds){
    //         std::cout << l;
    //     }
    //     std::cout << std::endl;
    // }
//     LOG("lines in global holder")
//     for(const auto& sets: initProdSMap){
//         LOG(sets.second->stateNum)
//         for(const auto& l: sets.first){
//             std::cout << l;
//         }
//         LOG("-")
//     }
//    LOG(".....")
    //std::cout << s;
    //for all collections, if it was already produced
    //connect
    for(auto& [prodName, setOfProds]: produtionsAtDotPos){// does not contain
        //increment total dots
        unordered_set<line,line::hash> incrementedTemp;
        auto setIter=setOfProds.begin();
        while(!setOfProds.empty()){
            setIter=setOfProds.begin();
            line lNew = *setIter;
            lNew.dotPosition+=1;
            incrementedTemp.insert(lNew);
            setOfProds.erase(setIter++);
        }
        setOfProds.swap(incrementedTemp);
        // std::cout << "in loop" << std::endl;
        // for(const auto& l: setOfProds){
        //     std::cout << l;
        // }
        // std::cout << "out of loop" << std::endl;
        bool previouslySeen = false;
        for(auto initProdSMapIter = initProdSMap.begin(); initProdSMapIter!=initProdSMap.end(); ++initProdSMapIter){
            if(initProdSMapIter->first==setOfProds){
                previouslySeen = true;
                break;
            }
        }
        if(!previouslySeen)// does not contain //DID NOT WORK: initProdSMap.find(setOfProds) == initProdSMap.end()
        { 
            //create new state
            s.transitions[prodName] = std::make_shared<state>(closure(setOfProds));
            
            //set new state num
            s.transitions[prodName]->stateNum = globalStateNum++;
            //hold globally
            initProdSMap[setOfProds] = s.transitions[prodName];
            // if(s.transitions[prodName]==nullptr){
            //     std::cout << "is null ptr" << std::endl;
            // }
            //recusively call
            LOG("> " << prodName)
            goToState(*s.transitions[prodName]);
        }
        else //does contain
        { 
            //defer to that transition
            s.transitions[prodName] = initProdSMap[setOfProds];
            //LOG("\t\t hit old conn")
        }
        //std::cout << *s.transitions[prodName]; //display children
        LOG("<")
    }
}
