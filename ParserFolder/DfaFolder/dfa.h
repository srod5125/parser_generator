#ifndef _DFA_HEADER_
#define _DFA_HEADER_

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <string>


#include "../../CommonFolder/common.h"

using std::unordered_map;
using std::unordered_set;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;
using std::string;


//dot position, production list, lookahead set
struct line {
    int dotPosition;
    symbol prod; // s-> ABB
    unordered_set<string> lookahead;

    line(int,symbol&&,unordered_set<string>&&);
    line(int,const symbol&,unordered_set<string>&&);
    line(int,const symbol&,const unordered_set<string>&);
    line(int,symbol&&,const unordered_set<string>&);

    //to use set
    friend bool operator==(const line&, const line&);
    //size_t operator()(const line& l) const;
    struct hash
    {
        std::size_t operator()( const line& ) const;
    };
};


enum class status : char{
    accept,
    closed,
    intermediate,
};
//list of lines, list of transitions
struct state{
    unordered_set<line> productions;
    unordered_map<string, shared_ptr<state> > transitions; //one way pointer to new state
    int stateNum;
    status rank;

    state();
    state(int,line);
    state(const unordered_set<line,line::hash>&);
    state(const unordered_set<line>&);

    friend std::ostream& operator<< (std::ostream&, const state&);
};

class Dfa {
    private:
        void goToState(state);//recurisve calls clojure, should know whther stat has been set
        
        bool hasEpsilonProduction(string);
        unordered_set<string> first(const string&);

        unordered_map<string,symbol> grammar;
        unique_ptr<state> startPtr;
    public:
        Dfa();
        Dfa(const unordered_map<string,symbol>&);
        ~Dfa();
        state closure(unordered_set<line>);//calls goto
};

#endif