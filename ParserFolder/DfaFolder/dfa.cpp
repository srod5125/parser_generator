#include <set>
#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <utility>
#include <stack>

#include "dfa.h"
#include "../../CommonFolder/common.h"

using std::set;
using std::string;
using std::vector;
using std::set;
using std::pair;
using std::stack;

using lineSet = unordered_set<line,line::hash,line::equal>;

#define LOG(msg) std::cout << msg << std::endl;
#define CONDLOG(cond,msgTrue,msgFalse) if(cond) {std::cout << msgTrue << std::endl;} else {std::cout << msgFalse << std::endl;}
#define PRINTSET(set) std::cout << "{"; for(const auto& el:x){ std::cout << el << " "; } std::cout << "}" << std::endl;

// ----------- line ---------
line::line(int pos,symbol&& sym,unordered_set<string>&& lk): dotPosition{pos},prod{sym},lookahead{lk}{ }
line::line(int pos,symbol& sym,unordered_set<string>&& lk): dotPosition{pos},prod{sym},lookahead{lk} { }
line::line(int pos,symbol& sym,unordered_set<string>& lk): dotPosition{pos},prod{sym},lookahead{lk} { }
line::line(int pos,symbol&& sym,unordered_set<string>& lk): dotPosition{pos},prod{sym},lookahead{lk}{ }
line::line(const line& l,unordered_set<string>& lk): dotPosition{l.dotPosition},prod{l.prod},lookahead{lk}{ }

std::size_t line::hash::operator()( const line& l) const{
    std::size_t seed = l.prod.production_rule[0].size();
    std::hash<string> stringHasher;
    seed ^= stringHasher(l.prod.name) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<int>()(l.dotPosition) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<bool>()(l.prod.isTerminal) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    for(const auto& el: l.prod.production_rule[0]){
        seed ^= stringHasher(el) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    for(const auto& el: l.lookahead){
        seed ^= stringHasher(el); //bruh
    }
    return seed;
}
bool line::equal::operator()(const line& lhs,const line& rhs) const{
    if(!(lhs.prod == rhs.prod)) {return false;}
    if(lhs.lookahead != rhs.lookahead) {return false;}
    return true;
}
bool line::operator==(const line& rhs) const{
    if(!(this->prod == rhs.prod)) {return false;}
    if(this->lookahead != rhs.lookahead) {return false;}
    return true;
}
std::ostream& operator<< (std::ostream& out, const line& l){
    out << l.prod.name << " -> ";
    for(int i=0;i<l.prod.production_rule[0].size();i+=1){
        if(i==l.dotPosition){
            out << ".";
        }
        out << l.prod.production_rule[0][i];
    }
    if(l.prod.production_rule[0].size()==l.dotPosition){
        out << ".";
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
state::state(const lineSet& lSet)
: stateNum{0}, rank{status::intermediate}, transitions{} {
    productions.insert(lSet.begin(),lSet.end());
}
std::ostream& operator<< (std::ostream& out, const state& s){
    //num status
    out << "STATE:" << s.stateNum << "\t";
    switch (s.rank)
    {
        case status::accept:{
            out << "ACCEPTING";
            break;
        }
        case status::closed:{
            out << "CLOSED";
            break;
        }
        case status::intermediate:{
            out << "INTERMEDIATE";
            break;
        }
        case status::start:{
            out << "START";
            break;
        }
            
        default:
            break;
    }
    out << std::endl;
    //print rules with dots
    for(const auto& l: s.productions){
        //looping over symbols first production rule
        out << l;
    }
    
    out << "CONNECTIONS:  ";
    for(const auto& t: s.transitions){
        if(t.second.get() != nullptr){
            out << t.first << ">" << t.second->stateNum << " ";
        }
    }
    out << std::endl << std::endl;

    return out;
}
std::size_t state::hash::operator()( const state& s) const{
    std::size_t seed = std::hash<int>()(s.stateNum);
    std::hash<string> stringHasher;
    line::hash lineHasher;
    
    for(const auto& l : s.productions){
        seed ^= lineHasher(l);
    }
    for(const auto& t: s.transitions){
        seed ^= stringHasher(t.first) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    seed ^=  std::hash<char>()(static_cast<char>(s.rank)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
}
bool state::equal::operator()(const state& lhs,const state& rhs) const{
    return lhs.stateNum == rhs.stateNum; //lazy equality, statemun is unique
}
bool state::operator==(const state& rhs) const{
    if(this->stateNum != rhs.stateNum) {return false;}
    if(this->rank != rhs.rank) {return false;}
    if(this->productions != rhs.productions) {return false;}
    return true;
}
//-------------- helpers -------

std::size_t initProdsHash::operator()(const lineSet & lSet) const
{
    std::size_t seed = lSet.size();
    line::hash lineHahser;
    for(const auto& l: lSet){
        //std::cout << l << "\t here" << std::endl;
        seed ^= lineHahser(l) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}

bool initProdsEqual::operator()(const lineSet& lhs, const lineSet& rhs) const {
    return lhs == rhs;
}

// -------------- dfa ----------
Dfa::Dfa():grammar{}, globalStateNum{1}, firstCache{}, initProdSMap{} {
    // line augmentedStart = line(0,symbol("S'",{"S"}),{"$"}); // line augmentedStart = line(0,symbol("S'",{"start"}),{"$"}); 
    // unordered_set<line,line::hash,line::equal> x;
    // x.insert(augmentedStart);
    // startPtr = closure(x);
    // startPtr->rank = status::start;
    // startPtr->stateNum = 0;
    // goToState(*startPtr);
}
Dfa::Dfa(unordered_map<string,symbol>& g): globalStateNum{1}, firstCache{}, initProdSMap{}  {
    grammar = g;

    // symbol s0 = symbol("S'",{"S"}); // TODO replace S with start
    // g["S'"] = s0;

    // line augmentedStart = line(0,s0,{"$"});
    // lineSet x;
    // x.insert(augmentedStart);
    // startPtr = closure(x);
    // startPtr->rank = status::start;
    // startPtr->stateNum = 0;
    // goToState(*startPtr);
}
Dfa::~Dfa() {}

// void printStack(stack<string> s){
//     while(!s.empty()){
//         std::cout << s.top() << " ";
//         s.pop();
//     }
//     LOG("")
// }

unordered_set<string> Dfa::first(const string& sym,unordered_set<string>& alreadySeen){
    //std::cin.get();
    if(firstCache.find(sym) != firstCache.end()){
        //LOG("hit 1")
        alreadySeen.insert(sym);
        return firstCache[sym];
    }
    else{
        if(grammar[sym].isTerminal){ //handles null case
            //LOG(grammar[sym])
            //LOG("hit 2")
            return {sym};
        }
        else {
            //CONDLOG(alreadySeen.find(sym)==alreadySeen.end(),"not in","in")
                // std::cout << "{";
                // for(const auto& el:firstCache[sym]){
                //     std::cout << el << " ";
                // }
                // std::cout << "}";

            if(alreadySeen.find(sym)==alreadySeen.end()){
                alreadySeen.insert(sym);
                //LOG(sym)
                unordered_set<string> x;
                for(const auto& seq : grammar[sym].production_rule){
                    //LOG("hit 3")
                    if(!seq.empty()){
                        if(grammar[seq[0]].isTerminal){
                            x.insert(seq[0]);
                        }
                        else{

                            if(!hasEpsilonProduction(seq[0])){
                                //LOG("hit 4")
                                unordered_set<string> tmp = first(seq[0],alreadySeen);
                                //LOG("hit 4 return")
                                x.insert(tmp.begin(),tmp.end());
                            }
                            else {
                                //LOG("hit 5")
                                unordered_set<string> tmp = first(seq[0],alreadySeen);
                                //LOG("hit 5 return")
                                tmp.erase("EMPTY");
                                x.insert(tmp.begin(),tmp.end());

                                for(int i=1;i<seq.size();i+=1){
                                    //LOG(seq[i])
                                    if(grammar[seq[i]].isTerminal){
                                        x.insert(seq[i]);
                                        break;
                                    }
                                    else{
                                        //LOG("hit "<<seq[i])
                                        //LOG("hit 6")
                                        unordered_set<string> tmp = first(seq[i],alreadySeen);
                                        //LOG("hit 6 return")
                                        x.insert(tmp.begin(),tmp.end());
                                    }
                                }
                            }
                        }
                    }
                }
                //LOG("hit 7")
                firstCache[sym] = x;
                //PRINTSET(firstCache[sym])
                return x;
            }
            //LOG("hit 8")
            return {};
        }
    }
    //LOG("hit 9")
    return {};
}

bool Dfa::hasEpsilonProduction(string nonterminal){
    bool hasEpsilonProduction{false};
    for(const auto& i: grammar[nonterminal].production_rule){
        if(!i.empty()){
            if(i.size()==1 && i[0]=="EMPTY"){
                hasEpsilonProduction=true;
                break;
            }
        }
    }
    return hasEpsilonProduction;
}


shared_ptr<state> Dfa::closure(lineSet lSet){
    
    //check out early if is size of 1 and closed or accepting
    if(lSet.size()==1){
        shared_ptr<state> s = std::make_shared<state>(lSet);
        auto lineSetIter = lSet.begin();
        if(lineSetIter->dotPosition >= lineSetIter->prod.production_rule[0].size()){ // s-> aAb o
            s->rank = status::closed;
            if(lineSetIter->prod.name == "S'" || lineSetIter->prod.name=="AUGMENTED_START"){
                s->rank = status::accept;
            }
            //std::cout << s;
            return s;
        }
        
    }
    
    struct lineNoSetHash {
        size_t operator()(const line& l) const {
            std::size_t seed = l.prod.production_rule[0].size();
            std::hash<string> stringHasher;
            seed ^= stringHasher(l.prod.name);
            seed ^= l.dotPosition + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            for(const auto& el: l.prod.production_rule[0]){
                seed ^= stringHasher(el) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            //note we do not hash the set
            //LOG(l << seed)
            return seed;
        }
    };
    struct lineNoSetEqual {
        bool operator() (const line& lhs, const line& rhs) const { 
            if(lhs.prod.name != rhs.prod.name) {return false;}
            if(lhs.prod.production_rule[0].size() != rhs.prod.production_rule[0].size()) {return false;}
            if(lhs.prod.production_rule[0] != rhs.prod.production_rule[0]) {return false;}
            return true;
         }
    };
    unordered_map<line,unordered_set<string>,lineNoSetHash,lineNoSetEqual> alreadySeen;//a line map
    //map between string+vector : set

    auto lineSetIter{lSet.begin()};
    bool allClosed{true};
    bool encounteredAcceptCondition{false};

    while(!lSet.empty()){
        lineSetIter = lSet.begin();
        LOG(*lineSetIter);

        //if not closed
        if(lineSetIter->dotPosition < lineSetIter->prod.production_rule[0].size())
        {
            string currentDotPosString{lineSetIter->prod.production_rule[0][lineSetIter->dotPosition]};
            
            if(!grammar[currentDotPosString].isTerminal){
                //check if production has already been added at this dotposition
                if(alreadySeen.find( *lineSetIter ) == alreadySeen.end()){ //not present

                    for(const auto& prods: grammar[currentDotPosString].production_rule){
                        //get lookahead
                        unordered_set<string> x = lineSetIter->lookahead;
                        //LOG(lineSetIter->dotPosition+1)
                        //LOG("\t"<<lineSetIter->prod.production_rule[0].size())
                        if(lineSetIter->dotPosition+1 < lineSetIter->prod.production_rule[0].size()){
                            unordered_set<string> firstHelper{};
                            x = first(lineSetIter->prod.production_rule[0][lineSetIter->dotPosition+1],firstHelper);
                            //printSet(x);
                            //LOG("hit")
                        }
                        line newLine{line(0,symbol(currentDotPosString,vector<string>(prods)),x)};
                        LOG("\t"<<newLine)
                        //introduce new memebers
                        lSet.insert(newLine);
                    }
                    //alreadySeen.insert({currentDotPosString,lineSetIter->dotPosition}); // insert already seen
                    
                }
            }
            allClosed = false;
        }
        else
        {
            //hits here when dot position is beyond production length
            encounteredAcceptCondition = lineSetIter->prod.name == "S'" || lineSetIter->prod.name=="AUGMENTED_START";
        }
        //(string,vector<string>) insert new set
        //alreadySeen[*lineSetIter].insert(lineSetIter->lookahead.begin(),lineSetIter->lookahead.end());
        //LOG(*lineSetIter)
        //printSet(alreadySeen[*lineSetIter]);
        //LOG("--")
        //delete current memeber
        alreadySeen[*lineSetIter].insert(lineSetIter->lookahead.begin(),lineSetIter->lookahead.end());
        lSet.erase(lineSetIter);

    }

    lineSet aux{};
    //pump into new states
    for(auto& [lineNoSet, firstSet]: alreadySeen){
        aux.insert(line(lineNoSet,firstSet));
    }
    shared_ptr<state> sI = std::make_shared<state>(aux);
    sI->rank = encounteredAcceptCondition ? status::accept : sI->rank;
    sI->rank = (allClosed && !encounteredAcceptCondition) ? status::closed : status::intermediate;
    //LOG("hit here")
    //std::cout << sI;
    return sI;

}
//goto to transitions for state
void Dfa::goToState(state& s){ //TODO: fix
    //if state was already constructed set pointer to that
    //set transition to string -> state
    //std::cin.get();
    //LOG("hit1")
    unordered_map< string, lineSet > produtionsAtDotPos;
    //collect set of lines with equal dot position strings
    for(const auto& l : s.productions){
        if(l.dotPosition<l.prod.production_rule[0].size()){
            produtionsAtDotPos[l.prod.production_rule[0][l.dotPosition]].insert(l);
        } 
    }
    
    //unordered_set<string> toVisit;
    //for all collections, if it was already produced connect
    for(auto& [prodName, setOfProds]: produtionsAtDotPos){// does not contain
        //increment total dots
        lineSet incrementedTemp;
        auto setIter=setOfProds.begin();
        while(!setOfProds.empty()){
            setIter=setOfProds.begin();
            line lNew{*setIter};
            lNew.dotPosition+=1;
            incrementedTemp.insert(lNew);
            setOfProds.erase(setIter);
            //LOG("hit2")
        }
        setOfProds.swap(incrementedTemp);
        //LOG(s.stateNum)
        //LOG(initProdsHash()(setOfProds))
        if(initProdSMap.find(setOfProds) == initProdSMap.end())// does not contain
        { 
            //create new state
            s.transitions[prodName] = closure(setOfProds);
            //set new state num
            s.transitions[prodName]->stateNum = globalStateNum++;
            // LOG("<"<<s.stateNum<<">")
            // LOG(s.transitions[prodName]->stateNum)
            // //LOG(initProdsHash()(setOfProds))
            // for(const auto& l123:setOfProds){
            //     std::cout<<l123;
            // }
            //hold globally
            initProdSMap[setOfProds] = s.transitions[prodName];
            //recusive call
            //LOG("hit3")
            goToState(*s.transitions[prodName]);
            //LOG("hit4")
        }
        else //does contain
        { 
            //defer to that transition
            s.transitions[prodName] = initProdSMap[setOfProds];
            //LOG("hit5")
        }
        //LOG("<")
    }
    
    // for(const auto& arrow : toVisit){
    //     goToState(*s.transitions[arrow]);
    // }
    LOG(s);
    
}

