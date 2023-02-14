#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <utility>
#include <iostream>
#include <iomanip>
#include <stack>

using std::string;
using std::vector;
using std::unordered_set;
using std::unordered_map;
using std::pair;
using std::stack;
using std::setw;

#include "../../CommonFolder/common.h"
#include "parserTable.h"

#define LOG(msg) std::cout << msg << std::endl;

//step char
char getStepChar(const step thiStep){
    switch (thiStep)
    {
        case step::accept : return 'A';
        case step::error : return 'E';
        case step::none : return 'N';
        case step::reduce : return 'R';
        case step::shift : return 'S';
        
        default: return 'X';
    }
}

//move defs
move::move() : s{step::error},state{-1},nonterminal{},len{0} {}//default
move::move(step stp, int stat) : s(stp),state(stat),nonterminal{},len{0} {}//shift or none
move::move(step stp, int lenOfProd,const string& n) : s(stp),state(-1),nonterminal{n},len{lenOfProd} {}
move::move(step stp, int lenOfProd,string&& n) : s(stp),state(-1),nonterminal{n},len{lenOfProd} {}

bool move::operator==(const move& rhs) const{
    return this->s == rhs.s && this->state == rhs.state 
            && this->nonterminal == rhs.nonterminal && this->len == rhs.len;
}
bool move::operator!=(const move& rhs) const{
    return this->s != rhs.s || this->state != rhs.state 
            || this->nonterminal != rhs.nonterminal || this->len != rhs.len;
}

//helpers
std::size_t stateHash_DiffLk::operator()(const state& s) const{
    std::size_t seed = std::hash<int>()(s.productions.size());// no seed based on state num
    std::hash<string> stringHasher;
    std::hash<int> intHasher;
    std::hash<bool> boolHasher;
    
    for(const auto& l : s.productions){
        seed ^= intHasher(l.dotPosition);
        //seed ^= boolHasher(l.prod.isTerminal);
        for(const auto& p: l.prod){
            seed ^= stringHasher(p);
        }
        //note no look ahead hash
    }
    return seed;
}

bool stateEqual_DiffLk::operator()(const state & lhs, const state & rhs) const{
    bool same = true;
    // we do not test state num equallity
    if(lhs.productions.size() != rhs.productions.size()) {return false;}

    struct stringVecHash {
        size_t operator()(const vector<string>& v) const {
            std::hash<string> hasher;
            size_t seed = v.size();
            for (const auto& i : v) {
                seed ^= hasher(i) + 0x9e3779b9 + (seed<<6) + (seed>>2);
            }
            return seed;
        }
    };
    // struct stringVecEquals {
    //     bool operator()(const vector<string> & v1, const vector<string> & v2) const{
    //         bool sameVec = true;
    //         if (v1.size()!=v2.size()) {return false;}
    //         for(int i=0;i<v1.size() && v2.size();i+=1){
    //             sameVec = v1[i] == v2[i];
    //             if(!sameVec) {return false;}
    //         }
    //         return sameVec;
    //     }
    // };

    unordered_set<vector<string> , stringVecHash > lhs_withoutLk;
    unordered_set<vector<string> , stringVecHash > rhs_withoutLk;

    for(const auto& l: lhs.productions){
        lhs_withoutLk.insert(l.prod);
    }

    for(const auto& l: rhs.productions){
        rhs_withoutLk.insert(l.prod);
    }

    same = lhs_withoutLk == rhs_withoutLk;
    return same;    
}


//parsering table
ParserTable::ParserTable():actionTable{},gotoTable{},actionColumnMap{},gotoColumnMap{},statesToBeMerged{},d{} {}

ParserTable::ParserTable(unordered_map<string,symbol>& g){
    d = Dfa(g);
    init();
    // LOG(actionColumnMap["$"])
    // LOG(actionColumnMap["b"])
    fillInTable();
    // LOG(*this)
    // for(const auto& s : statesToBeMerged){
    //     std::cout << s.first.stateNum << "\t{ ";
    //     for(const auto& el: s.second){
    //         std::cout << el << " ";
    //     }
    //     LOG("}")
    // }
    merge();
    //LOG(actionTable.size())
    // string k =  actionTable[1][actionColumnMap["b"]].s == step::accept ? "acc" : "nope";
    //LOG(getStepChar(actionTable[23][actionColumnMap["1"]].s)<<actionTable[23][actionColumnMap["1"]].nonterminal)
    // LOG(actionColumnMap["$"])
    // LOG(actionColumnMap["b"])
}

void ParserTable::init(){
    int count_nonterminals = 0;
    int count_terminals = 0;

    for(auto& [sym,prod]: d.grammar){
        if(prod.isTerminal){
            actionColumnMap[sym] = count_terminals; //set colum map of terminal to table array
            //LOG(sym << " " << count_terminals)
            count_terminals += 1;
        }
        else{//TODO: ignore S' in cosntruction
            gotoColumnMap[sym] = count_nonterminals; //set colum map of nonterminal to table array
            count_nonterminals += 1;
        }
    }
    //creating action table [numOfStates,currTerminalMapVal]
    for(int i=0;i<d.globalStateNum;i+=1){
        vector< move > tmp(actionColumnMap.size(),move());
        actionTable[i] = tmp;
    }
    //creating goto table [numOfStates,currNonTerminalMapVal]
    for(int i=0;i<d.globalStateNum;i+=1){
        vector< move > tmp(gotoColumnMap.size(),move());
        gotoTable[i] = tmp;
    }
}

std::ostream& operator<< (std::ostream& out, const ParserTable& pT){
    int rowSize = pT.actionTable.size();
    int gotoSize = pT.gotoColumnMap.size();
    int accSize = pT.actionColumnMap.size();

    //const int spc = 7;
    // maps iter to colum map
    unordered_map<int,string> reverseActionCol;
    int tmpCol{0};
    for(const auto& a: pT.actionColumnMap){
        reverseActionCol[tmpCol] = a.first;
        tmpCol+=1;
    }
    unordered_map<int,string> reverseGotoCol;
    tmpCol = 0;
    for(const auto& g: pT.gotoColumnMap){
        reverseGotoCol[tmpCol] = g.first;
        tmpCol+=1;
    }
    auto getStepString = [&out](move m){ //,&spc
        switch (m.s)
        {
            case step::accept: {
                out << "A";
                break;
            }
            case step::none:{
                out << m.state;
                break;
            }
                
            case step::reduce:{
                out << "R" << m.nonterminal << m.len;
                break;
            }
            case step::shift:{
                out << "S" << m.state;
                break;
            }
            default:{
                out << "_"; //out << "E" << -1;
                break;
            }
        }
        out << "\t";
    };
    //print begining offset
    out << "\t"; //setw(spc*2);
    //print line of action and goto in order
    for(int i=0;i<pT.actionColumnMap.size();i+=1){
        out << reverseActionCol[i] << "\t";
    }
    for(int i=0;i<pT.gotoColumnMap.size();i+=1){
        out << reverseGotoCol[i] << "\t";
    }
    out << std::endl;
    for(const auto& row : pT.actionTable){
        out << row.first << "\t";
        //odd err here with setting j
        for(int i = 0,j=accSize*-1; j<gotoSize; i+=1,j+=1){
            //out << i << " " << j << setw(spc);
            if(i<pT.actionColumnMap.size()){
                getStepString(pT.actionTable.at(row.first)[ pT.actionColumnMap.at(reverseActionCol[i])]);
            }
            if(j>=0){
                getStepString(pT.gotoTable.at(row.first)[pT.gotoColumnMap.at(reverseGotoCol[j])]);
            }
        }
        out << std::endl;
    }
    return out;
}
    //perform dfs,
    //as a state is reached add it to map
    //key:state,value:set(state numbers)
    //fill in table transition steps based on grammer
void ParserTable::fillInTable(){
    //verified over simple grammar
    stack<state> dfaTrace;
    unordered_set<state,state::hash,state::equal> visited;
    dfaTrace.push(*d.startPtr);

    state curr_state;

    while(!dfaTrace.empty()){
        curr_state = dfaTrace.top();
        dfaTrace.pop();
        if(visited.find(curr_state)==visited.end()){
            LOG(curr_state)
            //LOG(curr_state.stateNum << " : " << stHshLk(curr_state))
            //keep track of states to merge
            statesToBeMerged[curr_state].insert(curr_state.stateNum);
            //switch (curr_state.rank) {
            //-------------------------------------
            //look for closed production
            for(const auto& l : curr_state.productions){
                if(l.dotPosition>=l.prod.size()){
                    for(const auto& lk : l.lookahead){
                        actionTable[curr_state.stateNum][actionColumnMap[lk]] = 
                        move(step::reduce,l.prod.size(),l.name);//add nontmerinal to reduc instruction
                    }
                }
            }
            //for all transitions
            for(const auto& t: curr_state.transitions){
                if(d.grammar.at(t.first).isTerminal){
                    actionTable[curr_state.stateNum][actionColumnMap[t.first]] = 
                    move(step::shift,t.second->stateNum);
                }
                else{
                    gotoTable[curr_state.stateNum][gotoColumnMap[t.first]] =
                    move(step::none,t.second->stateNum);
                }
                //LOG("\tpushing states"<<t.second->stateNum)
                dfaTrace.push(*t.second);
            }
            
            if(curr_state.isAccepting){
                actionTable[curr_state.stateNum][actionColumnMap["$"]] = 
                move(step::accept,-1);
            }
            
            visited.insert(curr_state);
        }

    }
}

//delete duplicate, perform union over equal states with diff lookaheads
void ParserTable::merge(){
//verified over simple grammar
    //write over equal states as equals
    //expensive n^2 operation
    for(const auto& m : statesToBeMerged){
        if(m.second.size()>1){
            auto first = m.second.begin();
            for(auto iter = ++m.second.begin();iter!=m.second.end();++iter){
                replaceEverInstance(*first,*iter);
            }
        }
    }

    //merge states
    for(const auto& m : statesToBeMerged){
        if(m.second.size()>1){
            auto first = m.second.begin();
            for(auto iter = ++m.second.begin();iter!=m.second.end();++iter){

                 if(actionTable[*first]!=actionTable[*iter] || gotoTable[*first]!=gotoTable[*iter]){
                    for(int i=0;i<actionTable[0].size();i+=1){
                        if(actionTable[*iter][i].s!=step::error){
                            actionTable[*first][i] = actionTable[*iter][i];
                        }
                    }
                    for(int i=0;i<gotoTable[0].size();i+=1){
                        if(gotoTable[*iter][i].s!=step::error){
                            gotoTable[*first][i] = gotoTable[*iter][i];
                        }
                    }
                 }
                 //delete row other than first
                 actionTable.erase(actionTable.find(*iter));
                 gotoTable.erase(gotoTable.find(*iter));
            }
        }
    }
}

void ParserTable::replaceEverInstance(int state, int stateToBeReplaced){
    for(auto& aRow : actionTable){
        for(auto& entry : aRow.second){
            if(entry.s != step::error){
                if(entry.state == stateToBeReplaced){
                    entry.state = state;
                }
            }
        }
    }
    for(auto& gRow : gotoTable){
        for(auto& entry : gRow.second){
            if(entry.s != step::error){
                if(entry.state == stateToBeReplaced){
                    entry.state = state;
                }
            }   
        }
    }
}

//does bounds checking
move ParserTable::getMove(int state,const string& val){
    LOG("\t\tS: "<<state<<" V: "<<val)
    if(d.grammar[val].isTerminal){
        //if(0<=state && state<actionTable.size()){
            LOG("\t\t"<<getStepChar(actionTable[state][actionColumnMap[val]].s)<<actionTable[state][actionColumnMap[val]].state)
            return actionTable[state][actionColumnMap[val]];
        // }
        // else{
        //     LOG("\t\tE_1")
        //     return move();
        // }
    }
    else{
        //if(0<=state && state<gotoTable.size()){
            LOG("\t\t"<<getStepChar(gotoTable[state][gotoColumnMap[val]].s)<<gotoTable[state][gotoColumnMap[val]].nonterminal)
            return gotoTable[state][gotoColumnMap[val]];
        // }
        // else{
        //     LOG("\t\tE_2")
        //     return move();
        // }
    }
    // LOG("\t\tE_3")
    // return move();
}

// move ParserTable::getMove(const pair<int,string>& pr){
//     if(d.grammar[pr.second].isTerminal){
//         if(0<=pr.first && pr.first<actionTable.size()){
//             return actionTable[pr.first][actionColumnMap[pr.second]];
//         }
//         else{
//             return move();
//         }
//     }
//     else{
//         if(0<=pr.first && pr.first<gotoTable.size()){
//             return gotoTable[pr.first][gotoColumnMap[pr.second]];
//         }
//         else{
//             return move();
//         }
//     }
//     return move();
// }

ParserTable::~ParserTable(){}
