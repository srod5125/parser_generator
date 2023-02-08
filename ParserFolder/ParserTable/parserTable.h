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
#include <set>
#include <map>
#include <iostream>

#include "../DfaFolder/dfa.h"
#include "../../CommonFolder/common.h"

using std::pair;
using std::vector;
using std::unordered_map;
using std::unordered_set;
using std::set;
using std::map;

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
    accept,
    error
};

struct stateHash_DiffLk {
    std::size_t operator()(const state&) const;
};
struct stateEqual_DiffLk {
    bool operator()(const state&,const state&) const;
};

struct move{
    step s;
    int state;
    string nonterminal;

    move();
    move(step,int);//shift setter;
    move(step,int,const string&);
    move(step,int,string&&);

    bool operator==(const move&) const;
    bool operator!=(const move&) const;
};

class ParserTable {
    private:
        Dfa d;//here : make parser table accept grammer then instantiate dfa

        map< int , vector<move> > actionTable; //state num // transitions
        map< int , vector<move> > gotoTable;

        unordered_map<string,int> actionColumnMap;
        unordered_map<string,int> gotoColumnMap;

        void init();
        void fillInTable();
        unordered_map< state , set<int> , stateHash_DiffLk, stateEqual_DiffLk > statesToBeMerged;

        void merge(); 
        void replaceEverInstance(int,int);
    public:
        ParserTable();
        ParserTable(unordered_map<string,symbol>& g);
        ~ParserTable();

        move getMove(int,const string&);
        move getMove(int,string&&);
    
        friend std::ostream& operator<< (std::ostream&, const ParserTable&);
};

#endif