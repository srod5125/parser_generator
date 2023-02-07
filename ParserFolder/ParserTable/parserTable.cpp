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


#include "parserTable.h"

#define LOG(msg) std::cout << msg << std::endl;

//move defs
move::move() : s{step::error},state{-1},nonterminal{} {}//default
move::move(step stp, int stat) : s(stp),state(stat),nonterminal{} {}//shift or none
move::move(step stp, int stat,const string& n) : s(stp),state(stat),nonterminal{n} {}
move::move(step stp, int stat,string&& n) : s(stp),state(stat),nonterminal{n} {}

bool move::operator==(const move& rhs) const{
    return this->s == rhs.s && this->state == rhs.state && this->nonterminal == rhs.nonterminal;
}
bool move::operator!=(const move& rhs) const{
    return this->s != rhs.s || this->state != rhs.state || this->nonterminal != rhs.nonterminal;
}

//helpers
std::size_t stateHash_DiffLk::operator()(const state& s) const{
    std::size_t seed = std::hash<int>()(s.productions.size());// no seed based on state num
    std::hash<string> stringHasher;
    std::hash<int> intHasher;
    std::hash<bool> boolHasher;
    
    for(const auto& l : s.productions){
        seed ^= intHasher(l.dotPosition) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= boolHasher(l.prod.isTerminal) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        for(const auto& p: l.prod.production_rule[0]){
            seed ^= stringHasher(p) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
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
            size_t seed = 0;
            for (const auto& i : v) {
                seed ^= hasher(i) + 0x9e3779b9 + (seed<<6) + (seed>>2);
            }
            return seed;
        }
    };
    struct stringVecEquals {
        bool operator()(const vector<string> & v1, const vector<string> & v2) const{
            bool sameVec = true;
            if (v1.size()!=v2.size()) {return false;}
            for(int i=0;i<v1.size() && v2.size();i+=1){
                sameVec = v1[i] == v2[i];
                if(!sameVec) {return false;}
            }
            return sameVec;
        }
    };

    unordered_set<vector<string> , stringVecHash , stringVecEquals> lhs_withoutLk;
    unordered_set<vector<string> , stringVecHash , stringVecEquals> rhs_withoutLk;

    for(const auto& l: lhs.productions){
        lhs_withoutLk.insert(l.prod.production_rule[0]);
    }

    for(const auto& l: rhs.productions){
        rhs_withoutLk.insert(l.prod.production_rule[0]);
    }

    same = lhs_withoutLk == rhs_withoutLk;
    return same;    
}


//parsering table
ParserTable::ParserTable():actionTable{},gotoTable{},actionColumnMap{},gotoColumnMap{},statesToBeMerged{} {}

ParserTable::ParserTable(const Dfa& d){
    init(d);
    fillInTable(d);
    
    // for(const auto& s : statesToBeMerged){
    //     std::cout << s.first.stateNum << "\t{ ";
    //     for(const auto& el: s.second){
    //         std::cout << el << " ";
    //     }
    //     LOG("}")
    // }
    merge();
    //LOG(actionTable.size())
}

void ParserTable::init(const Dfa& d){
    int count_nonterminals = 0;
    int count_terminals = 0;

    for(auto& [sym,prod]: d.grammar){
        if(prod.isTerminal){
            actionColumnMap[sym] = count_terminals; //set colum map of terminal to table array
            count_terminals += 1;
        }
        else{
            gotoColumnMap[sym] = count_nonterminals; //set colum map of nonterminal to table array
            count_nonterminals += 1;
        }
    }
    //creating action table [numOfStates,currTerminalMapVal]
    for(int i=0;i<d.globalStateNum+1;i+=1){
        vector< move > tmp(actionColumnMap.size(),move());
        actionTable[i] = tmp;
    }
    //creating goto table [numOfStates,currNonTerminalMapVal]
    for(int i=0;i<d.globalStateNum+1;i+=1){
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
            case step::accept: 
                out << "A" << m.state;
                break;
            case step::none:
                out << " " << m.state;
                break;
            case step::reduce:
                out << "R" << m.state << m.nonterminal;
                break;
            case step::shift:
                out << "S" << m.state;
                break;
            default:
                out << "_"; //out << "E" << -1;
                break;
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

void ParserTable::fillInTable(const Dfa& d){
    //perform dfs,
    //as a state is reached add it to map
    //key:state,value:set(state numbers)
    //fill in table transition steps based on grammer
    stateHash_DiffLk stHshLk;

    stack<state> dfaTrace;
    unordered_set<state,state::hash,state::equal> visited;
    dfaTrace.push(*d.startPtr);

    state curr_state;

    while(!dfaTrace.empty()){
        curr_state = dfaTrace.top();
        dfaTrace.pop();
        if(visited.find(curr_state)==visited.end()){
            //LOG(curr_state.stateNum << " : " << stHshLk(curr_state))
            //keep track of states to merge
            statesToBeMerged[curr_state].insert(curr_state.stateNum);
            if(curr_state.rank == status::closed){
                //for each prodution 
                for(const auto& l : curr_state.productions){
                    for(const auto& lk : l.lookahead){
                        actionTable[curr_state.stateNum][actionColumnMap[lk]] = 
                        move(step::reduce,l.prod.production_rule[0].size(),l.prod.name);//add nontmerinal to reduc instruction
                        //size in subsitute for canonical item number
                        //already available
                    }
                }
            }
            else{
                for(const auto& t: curr_state.transitions){
                    if(t.second->rank == status::accept){
                        actionTable[curr_state.stateNum][actionColumnMap["$"]] = 
                        move(step::accept,-1);
                    }
                    else{
                        if(d.grammar.at(t.first).isTerminal){
                            actionTable[curr_state.stateNum][actionColumnMap[t.first]] = 
                            move(step::shift,t.second->stateNum);
                        }
                        else{
                            gotoTable[curr_state.stateNum][gotoColumnMap[t.first]] =
                            move(step::none,t.second->stateNum);
                        }
                    }

                    dfaTrace.push(*t.second);
                }
            }
            visited.insert(curr_state);
        }

    }

}


void ParserTable::merge(){
    for(const auto& m : statesToBeMerged){
        if(m.second.size()>1){
            auto first = m.second.begin();
            for(auto iter = std::next(m.second.begin());iter!=m.second.end();++iter){
                 if(actionTable[*first]!=actionTable[*iter] && gotoTable[*first]!=gotoTable[*iter]){
                    for(int i=0;i<actionTable[0].size();i+=1){
                        if(actionTable[*first][i].s!=step::error&&actionTable[*iter][i].s!=step::error
                        &&actionTable[*first][i]!=actionTable[*iter][i]){
                            LOG("merge conflict")
                        }
                        else{
                            if(actionTable[*iter][i].s!=step::error){
                                actionTable[*first][i] = actionTable[*iter][i];
                            }
                        }
                    }
                    for(int i=0;i<gotoTable[0].size();i+=1){
                        if(gotoTable[*first][i].s!=step::error&&gotoTable[*iter][i].s!=step::error
                        &&gotoTable[*first][i]!=gotoTable[*iter][i]){
                            LOG("merge conflict")
                        }
                        else{
                            if(gotoTable[*iter][i].s!=step::error){
                                gotoTable[*first][i] = gotoTable[*iter][i];
                            }
                        }
                    }
                 }
                 //delete row other than first
                 actionTable.erase(actionTable.find(*iter));
                 gotoTable.erase(gotoTable.find(*iter));

                 replaceEverInstance(*first,*iter);//loop through each table for any isntance of
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

ParserTable::~ParserTable(){}
