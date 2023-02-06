#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <utility>
#include <iostream>
#include <iomanip> 

using std::string;
using std::vector;
using std::unordered_set;
using std::unordered_map;
using std::pair;
using std::setw;


#include "parserTable.h"


std::size_t stateHash_DiffLk::operator()(const state& s) const{
    std::size_t seed = std::hash<int>()(s.stateNum);
    std::hash<string> stringHasher;
    //acc ^= l.dotPosition + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    for(const auto& l : s.productions){
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
    same = lhs.productions.size() == rhs.productions.size();
    if(!same) {return false;}

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

ParserTable::ParserTable():actionTable{},gotoTable{},actionColumnMap{},gotoColumnMap{},statesToBeMerged{} {}

ParserTable::ParserTable(const Dfa& d){
    init(d);
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
        vector< pair<step,int> > tmp(count_terminals,std::make_pair(step::none,-1));
        actionTable.emplace_back(tmp);
    }
    //creating goto table [numOfStates,currNonTerminalMapVal]
    for(int i=0;i<d.globalStateNum+1;i+=1){
        vector< pair<step,int> > tmp(count_nonterminals,std::make_pair(step::none,-1));
        gotoTable.emplace_back(tmp);
    }
}

std::ostream& operator<< (std::ostream& out, const ParserTable& pT){
    int rowSize{pT.actionTable.size()};
    int colSize{pT.actionColumnMap.size()+pT.gotoColumnMap.size()};

    const int spc = 4;
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
    auto getStepString = [&out,&spc](step s,int state){
        switch (s)
        {
            case step::accept: 
                out << "A" << state;
                break;
            case step::none:
                out << " " << -1;
                break;
            case step::reduce:
                out << "R" << state;
                break;
            case step::shift:
                out << "S" << state;
                break;
            default:
                out << "E" << -1;
                break;
        }
        out << setw(spc);
    };
    //print begining offset
    out << setw(spc);
    //print line of action and goto in order
    for(int i=0;i<pT.actionColumnMap.size();i+=1){
        out << reverseActionCol[i] << setw(spc);
    }
    for(int i=0;i<pT.gotoColumnMap.size();i+=1){
        out << reverseGotoCol[i] << setw(spc);
    }
    out << std::endl;
    for(int row=0;row<pT.actionTable[0].size();row+=1){
        out << row << setw(spc);
        for(int i=0, j=colSize*-1; j<colSize; ++i,++j){
            if(i<pT.actionColumnMap.size()){
                getStepString(pT.actionTable[row][i].first,pT.actionTable[row][i].second);
            }
            if(j>=0){
                getStepString(pT.gotoTable[row][j].first,pT.gotoTable[row][j].second);
            }
        }
        out << std::endl;
    }

}
