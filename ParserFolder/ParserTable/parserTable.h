// action part -> terminals
// goto part -> nonterminals
// number of rows: number of states in dfa
// state - (nonterminal/terminal) -> other state
// goto tanstions only hold state num
// nonterminal transtions in intermediate states are shifted, S+state num
// if state is closed 
    // if state is accepting write acepting in table
    // else:
    // getnumber of grammar rule : reduce, R+grammar rule num

#ifndef _PARSERTABLE_HEADER_
#define _PARSERTABLE_HEADER_

#include <utility>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

#include "../DfaFolder/dfa.h"

using std::pair;
using std::vector;
using std::unordered_map;

//states to be merged map(setoflinesNoLookAhead,statenum)

//colmap(string terminal, num) -> action
//colmap(string terminal, num) -> goto
//

//initial pass over map get sizes, terminals and nonterminals
//fill in tables
    // while filling in keep track of states to merge
//merge

enum class step:char {
    none,
    shift,
    reduce,
    accept
};

struct stateHash_DiffLk {
    std::size_t operator()(const state&) const;
};
struct stateEqual_DiffLk {
    bool operator()(const state&,const state&) const;
};

class ParserTable {
    private:
        vector< vector< pair<step,int> > > actionTable;
        vector< vector< pair<step,int> > > gotoTable;

        unordered_map<string,int> actionColumnMap;
        unordered_map<string,int> gotoColumnMap;

        void init(const Dfa&);
        void fillInTable(const Dfa&);
        unordered_map< state , unordered_set<int> , stateHash_DiffLk, stateEqual_DiffLk > statesToBeMerged;

        void merge();
    public:
        ParserTable();
        ParserTable(const Dfa&);
        ~ParserTable();
    
        friend std::ostream& operator<< (std::ostream&, const ParserTable&);
};

#endif