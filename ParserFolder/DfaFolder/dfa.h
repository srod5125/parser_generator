#ifndef _DFA_HEADER_
#define _DFA_HEADER_

#include <unordered_map>
#include <set>
#include <vector>
#include <memory>
#include <string>


#include "../../CommonFolder/common.h"

using std::unordered_map;
using std::set;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;
using std::string;


//dot position, production list, lookahead set
struct line {
    int dotPosition;
    symbol prod; // s-> ABB
    set<string> lookahead;

    line(int,symbol&&,set<string>&&);
    line(int,symbol&,set<string>&&);
    line(int,symbol&,set<string>&);
    line(int,symbol&&,set<string>&);
};
enum class status : char{
    accept,
    closed,
    temp,
};
//list of lines, list of transitions
struct state{
    vector<line> productions;
    unordered_map<string, shared_ptr<state> > transitions; //one way pointer to new state
    int stateNum;
    status rank;

    state();
    state(int,line);

    friend std::ostream& operator<< (std::ostream&, const state&);
};

class Dfa {
    private:
        unique_ptr<state> goToState(line);//recurisve calls clojure, should know whther stat has been set
        void closure(state);//calls goto
        
        bool hasEpsilonProduction(string);
        set<string> first(const string&);

        unordered_map<string,symbol> grammar;
        unique_ptr<state> startPtr;
    public:
        Dfa();
        Dfa(const unordered_map<string,symbol>&);
        ~Dfa();
};

#endif