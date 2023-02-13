#include <set>
#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <utility>
#include <stack>
#include <algorithm>

#include "dfa.h"
#include "../../CommonFolder/common.h"

using std::set;
using std::string;
using std::vector;
using std::set;
using std::pair;
using std::stack;

using lineSet = unordered_set<line,line::hash,line::equal>;
using lineSet_WithLk = unordered_set<line,line::hash_withLk,line::equal_withLk>;

#define LOG(msg) std::cout << msg << std::endl;
#define CONDLOG(cond,msgTrue,msgFalse) if(cond) {std::cout << msgTrue << std::endl;} else {std::cout << msgFalse << std::endl;}
#define PRINTSET(set) std::cout << "{"; for(const auto& el:set){ std::cout << el << " "; } std::cout << "}"; std::cout << std::endl;

// ----------- line ---------
line::line(int pos,symbol&& sym,set<string>&& lk): dotPosition{pos},prod{sym.production_rule[0]},name{sym.name},lookahead{lk}{ }
line::line(int pos,symbol& sym,set<string>&& lk): dotPosition{pos},prod{sym.production_rule[0]},name{sym.name},lookahead{lk} { }
line::line(int pos,symbol& sym,set<string>& lk): dotPosition{pos},prod{sym.production_rule[0]},name{sym.name},lookahead{lk} { }
line::line(int pos,symbol&& sym,set<string>& lk): dotPosition{pos},prod{sym.production_rule[0]},name{sym.name},lookahead{lk}{ }
line::line(const line& l,set<string>& lk): dotPosition{l.dotPosition},prod{l.prod},lookahead{lk},name{l.name}{ }

std::size_t line::hash::operator()( const line& l) const{
    std::size_t seed = l.prod.size();
    std::hash<string> stringHasher;
    seed ^= stringHasher(l.name) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<int>()(l.dotPosition) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    //seed ^= std::hash<bool>()(l.prod.isTerminal) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    for(const auto& el: l.prod){
        seed ^= stringHasher(el) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    // for(const auto& el: l.lookahead){
    //     seed ^= stringHasher(el); //bruh
    // }
    return seed;
}
std::size_t line::hash_withLk::operator()(const line& l) const{
    std::size_t seed = l.prod.size();
    std::hash<string> stringHasher;
    seed ^= stringHasher(l.name) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<int>()(l.dotPosition) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    //seed ^= std::hash<bool>()(l.prod.isTerminal) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    for(const auto& el: l.prod){
        seed ^= stringHasher(el) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    
    for(const auto& el: l.lookahead){
        seed ^= stringHasher(el) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}
bool line::equal::operator()(const line& lhs,const line& rhs) const{
    if(!(lhs.prod == rhs.prod)) {return false;}
    //if(lhs.lookahead != rhs.lookahead) {return false;}
    return true;
}
bool line::equal_withLk::operator()(const line& lhs,const line& rhs) const{
    if(!(lhs.prod == rhs.prod)) {return false;}
    if(lhs.lookahead != rhs.lookahead) {return false;}
    return true;
}
bool line::operator==(const line& rhs) const{
    if(!(this->prod == rhs.prod)) {return false;}
    //if(this->lookahead != rhs.lookahead) {return false;}
    return true;
}
std::ostream& operator<< (std::ostream& out, const line& l){
    out << l.name << " -> ";
    for(int i=0;i<l.prod.size();i+=1){
        if(i==l.dotPosition){
            out << ".";
        }
        out << l.prod[i];
    }
    if(l.prod.size()==l.dotPosition){
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
state::state() : stateNum{0}, rank{status::intermediate}, productions{}, transitions{},isAccepting{false} { }
state::state(int n, line l) : stateNum{0}, rank{status::intermediate}, transitions{},isAccepting{false} {
    stateNum = n;
    productions.insert(l);
}
state::state(const lineSet& lSet)
: stateNum{0}, rank{status::intermediate}, transitions{},isAccepting{false} {
    productions.insert(lSet.begin(),lSet.end());
}
std::ostream& operator<< (std::ostream& out, const state& s){
    //num status
    out << "STATE:" << s.stateNum << "\t";
    switch (s.rank)
    {
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
    if(s.isAccepting){
        out << " ACCEPTING";
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
    seed ^=  std::hash<int>()(static_cast<int>(s.rank)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
}
bool state::equal::operator()(const state& lhs,const state& rhs) const{
    return lhs.stateNum == rhs.stateNum &&
             lhs.rank == rhs.rank &&
             lhs.isAccepting == rhs.isAccepting &&
             lhs.productions == rhs.productions;
}
bool state::operator==(const state& rhs) const{
    if(this->stateNum != rhs.stateNum) {return false;}
    if(this->rank != rhs.rank) {return false;}
    if(this->productions != rhs.productions) {return false;}
    return true;
}
//-------------- helpers -------

std::size_t coreHash::operator()(const lineSet & lSet) const
{
    std::size_t seed = lSet.size();
    line::hash lineHahser;
    for(const auto& l: lSet){
        //std::cout << l << "\t here" << std::endl;
        seed ^= lineHahser(l); // + 0x9e3779b9 + (seed << 6) + (seed >> 2)
    }
    return seed;
}

bool coreEqual::operator()(const lineSet& lhs, const lineSet& rhs) const {
    return lhs == rhs;
}

// -------------- dfa ----------
Dfa::Dfa():grammar{}, globalStateNum{1}, firstCache{}, coreMap{} {
    line augmentedStart = line(0,symbol("S'",{"S"}),{"$"}); // line augmentedStart = line(0,symbol("S'",{"start"}),{"$"}); 
    unordered_set<line,line::hash,line::equal> x;
    x.insert(augmentedStart);
    startPtr = closure(x);
    startPtr->rank = status::start;
    startPtr->stateNum = 0;
    goToState(*startPtr);
}
Dfa::Dfa(unordered_map<string,symbol>& g): globalStateNum{1}, firstCache{}, coreMap{}  {
    grammar = g;

    symbol s0 = symbol("S'",{"S"}); // TODO replace S with start
    g["S'"] = s0;

    line augmentedStart = line(0,s0,{"$"});
    lineSet x;
    x.insert(augmentedStart);
    startPtr = closure(x);
    startPtr->rank = status::start;
    startPtr->stateNum = 0;
    goToState(*startPtr);
}
Dfa::~Dfa() {}

// void printStack(stack<string> s){
//     while(!s.empty()){
//         std::cout << s.top() << " ";
//         s.pop();
//     }
//     LOG("")
// }

set<string> Dfa::first(const string& sym,set<string>& alreadySeen){
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
                set<string> x;
                for(const auto& seq : grammar[sym].production_rule){
                    //LOG("hit 3")
                    if(!seq.empty()){
                        if(grammar[seq[0]].isTerminal){
                            x.insert(seq[0]);
                        }
                        else{

                            if(!hasEpsilonProduction(seq[0])){
                                //LOG("hit 4")
                                set<string> tmp = first(seq[0],alreadySeen);
                                //LOG("hit 4 return")
                                x.insert(tmp.begin(),tmp.end());
                            }
                            else {
                                //LOG("hit 5")
                                set<string> tmp = first(seq[0],alreadySeen);
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
                                        set<string> tmp = first(seq[i],alreadySeen);
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
    //LOG("kernel")
    //PRINTSET(lSet)
    //check out early if is size of 1 and closed or accepting
    if(lSet.size()==1){
        shared_ptr<state> s = std::make_shared<state>(lSet);
        auto lineSetIter = lSet.begin();
        if(lineSetIter->dotPosition >= lineSetIter->prod.size()){ // s-> aAb o
            s->rank = status::closed;
            if(lineSetIter->name == "S'" || lineSetIter->name=="AUGMENTED_START"){
                s->isAccepting = true;
            }
            // LOG(">")
            //LOG(*s)
            return s;
        }
        
    }
    
    unordered_map<line,set<string>,line::hash,line::equal> alreadySeen;//a line map
    //map between string+vector : set

    auto lineSetIter{lSet.begin()};
    bool allClosed{true};
    bool encounteredAcceptCondition{false};

    while(!lSet.empty()){
        lineSetIter = lSet.begin();
        //LOG(*lineSetIter);

        //if not closed
        if(lineSetIter->dotPosition < lineSetIter->prod.size())
        {
            string currentDotPosString{lineSetIter->prod[lineSetIter->dotPosition]};
            
            if(!grammar[currentDotPosString].isTerminal){
                //check if production has already been added at this dotposition
                if(alreadySeen.find( *lineSetIter ) == alreadySeen.end()){ //not present

                    for(const auto& prods: grammar[currentDotPosString].production_rule){
                        //get lookahead
                        set<string> x = lineSetIter->lookahead;
                        //LOG(lineSetIter->dotPosition+1)
                        //LOG("\t"<<lineSetIter->prod.production_rule[0].size())
                        if(lineSetIter->dotPosition+1 < lineSetIter->prod.size()){
                            set<string> firstHelper{};
                            x = first(lineSetIter->prod[lineSetIter->dotPosition+1],firstHelper);
                            //printSet(x);
                            //LOG("hit")
                        }
                        line newLine{line(0,symbol(currentDotPosString,vector<string>(prods)),x)};
                        //LOG("\t"<<newLine)
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
            encounteredAcceptCondition = lineSetIter->name == "S'" || lineSetIter->name=="AUGMENTED_START";
            //CONDLOG(encounteredAcceptCondition,"encounted accept","did not encounter accept")
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
    sI->rank = allClosed  ? status::closed : status::intermediate;
    sI->isAccepting = encounteredAcceptCondition;
    //LOG("hit here")
    //std::cout << sI;
    // LOG(">")
    //LOG(*sI)
    //LOG("--")
    return sI;

}
lineSet Dfa::closure_noState(lineSet lSet){

    unordered_map<line,set<string>,line::hash,line::equal> alreadySeen; 
    auto lineSetIter{lSet.begin()};

    while(!lSet.empty()){
        lineSetIter = lSet.begin();
        
        if(lineSetIter->dotPosition < lineSetIter->prod.size())
        {
            string currentDotPosString{lineSetIter->prod[lineSetIter->dotPosition]};
            
            if(!grammar[currentDotPosString].isTerminal){
                //check if production has already been added at this dotposition
                if(alreadySeen.find( *lineSetIter ) == alreadySeen.end()){ //not present

                    for(const auto& prods: grammar[currentDotPosString].production_rule){
                        //get lookahead
                        set<string> x = lineSetIter->lookahead;
                        //LOG(lineSetIter->dotPosition+1)
                        //LOG("\t"<<lineSetIter->prod.production_rule[0].size())
                        if(lineSetIter->dotPosition+1 < lineSetIter->prod.size()){
                            set<string> firstHelper{};
                            x = first(lineSetIter->prod[lineSetIter->dotPosition+1],firstHelper);
                            //printSet(x);
                            //LOG("hit")
                        }
                        line newLine{line(0,symbol(currentDotPosString,vector<string>(prods)),x)};
                        //LOG("\t"<<newLine)
                        //introduce new memebers
                        lSet.insert(newLine);
                    }
                    //alreadySeen.insert({currentDotPosString,lineSetIter->dotPosition}); // insert already seen
                    
                }
            }
            
        }
        //LOG("\t\t\tin new c"<<*lineSetIter)
        alreadySeen[*lineSetIter].insert(lineSetIter->lookahead.begin(),lineSetIter->lookahead.end());
        lSet.erase(lineSetIter);
    }

    lineSet aux{};
    //pump into new states
    for(auto& [lineNoSet, firstSet]: alreadySeen){
        aux.insert(line(lineNoSet,firstSet));
    }
    return aux;//TODO: return by rvalue
}
//goto to transitions for state
void Dfa::goToState(state& s){ //TODO: fix
    //if state was already constructed set pointer to that
    //set transition to string -> state
    //std::cin.get();
    //LOG("hit1")
    LOG(s)
    unordered_map< string, pair<lineSet,lineSet_WithLk> > produtionsAtDotPos;
    //collect set of lines with equal dot position strings
    for(const auto& l : s.productions){
        if(l.dotPosition<l.prod.size()){
            line newLine{l};
            newLine.dotPosition+=1;
            produtionsAtDotPos[l.prod[l.dotPosition]].first.insert(newLine);
            produtionsAtDotPos[l.prod[l.dotPosition]].second.insert(newLine);
            //LOG("\t"<<l)
        } 
    }
    
    //unordered_set<string> toVisit;
    //for all collections, if it was already produced connect
    for(auto& [prodName, setOfProds]: produtionsAtDotPos){// does not contain
        //increment total dots
        // lineSet incrementedTemp;
        // auto setIter=setOfProds.begin();
        // while(!setOfProds.empty()){
        //     setIter=setOfProds.begin();
        //     line lNew{*setIter};
        //     lNew.dotPosition+=1;
        //     incrementedTemp.insert(lNew);
        //     setOfProds.erase(setIter);
        //     //LOG("hit2")
        // }
        // setOfProds.swap(incrementedTemp);
        //LOG(s.stateNum)
        //LOG(initProdsHash()(setOfProds))
        if(coreMap.find(setOfProds.first) == coreMap.end())// does not contain
        { 
            //create new state
            s.transitions[prodName] = closure(setOfProds.first);
            //LOG("kernel")
            //PRINTSET(setOfProds.first)
            //set new state num
            s.transitions[prodName]->stateNum = globalStateNum++;
            //LOG(*s.transitions[prodName])
            // LOG("<"<<s.stateNum<<">")
            // LOG(s.transitions[prodName]->stateNum)
            // //LOG(initProdsHash()(setOfProds))
            // for(const auto& l123:setOfProds){
            //     std::cout<<l123;
            // }
            //hold globally
            coreMap[setOfProds.first].statePtr = s.transitions[prodName];
            coreMap[setOfProds.first].kernelCopy = setOfProds.second;
            for(const auto& l: setOfProds.second){
                coreMap[setOfProds.first].kernelLookaheadCpyMap[l] = l.lookahead;
            }
            //recusive call
            //LOG("hit3")
            goToState(*s.transitions[prodName]);
            //LOG("hit4")
        }
        else //does contain
        { 
            //defer to that transition
            
            // keep track of lookaheads
            // if same leave as is
            // else
            // reconstruct state and replace with existing state
            // give state function that holds initial kernel and merges with
            // new set of kernels
            // propogate new look aheads
            // recall goto on
            bool isSubset = true;
            for(const auto& lS:setOfProds.second){
                if(coreMap[setOfProds.first].kernelLookaheadCpyMap.find(lS)!=coreMap[setOfProds.first].kernelLookaheadCpyMap.end()){
                    auto tmpLineCpy = coreMap[setOfProds.first].kernelLookaheadCpyMap.find(lS);
                    if(!std::includes(tmpLineCpy->second.begin(),tmpLineCpy->second.end(),lS.lookahead.begin(),lS.lookahead.end())){
                        isSubset = false;
                        break;
                    }
                }
                else{//this case will never be hit
                    isSubset = false;
                    break;
                }
            }
            if(isSubset){
                //LOG("SECOND")
                //PRINTSET(setOfProds.second)
                s.transitions[prodName] = coreMap[setOfProds.first].statePtr;
            }
            else{
                //propogate new lookahead recusively
                lineSet mergedSets;
                lineSet_WithLk mergedSetsCpy;
                for(const auto& l: setOfProds.second){
                    coreMap[setOfProds.first].kernelLookaheadCpyMap[l]
                    .insert(l.lookahead.begin(),l.lookahead.end());
                    line newLine{l};
                    newLine.lookahead = coreMap[setOfProds.first].kernelLookaheadCpyMap[l];
                    mergedSets.insert(newLine);
                    mergedSetsCpy.insert(newLine);
                }
                // LOG("UNEQUAL")
                // PRINTSET(coreMap[setOfProds.first].kernelCopy)
                // PRINTSET(setOfProds.second)
                // LOG("...")
                coreMap[setOfProds.first].kernelCopy = mergedSetsCpy;
                // PRINTSET(mergedSets)
                coreMap[setOfProds.first].statePtr->productions = closure_noState(mergedSets);
                //LOG(">"<<coreMap[setOfProds.first].statePtr->stateNum)
                //LOG(*coreMap[setOfProds.first].statePtr)
                goToState(*(coreMap[setOfProds.first].statePtr));
                //LOG(s.stateNum<<"<")
            }
        }
        //LOG("<")
    }
    
    // for(const auto& arrow : toVisit){
    //     goToState(*s.transitions[arrow]);
    // }
    //LOG(s);
    
}

