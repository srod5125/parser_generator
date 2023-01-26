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
using std::string;


//dot position, production list, lookahead set
struct line {
    int dotPosition;
    string tag;
    vector<string> production_list; // s-> ABB
    set<string> lookahead;

    line(int,string&&,vector<string>&&,set<string>&&);
};
enum status : char{
    accept,
    closed,
};
//list of lines, list of transitions
struct state{
    vector<line> productions;
    unordered_map<string, unique_ptr<state> > transitions;
    int stateNum;
    status s;

    state();
    state(int,line);

    friend std::ostream& operator<< (std::ostream&, const state&);
};

class Dfa {
    private:
        void closure(state);//calls goto
        unique_ptr<state> goToState(line);//recurisve calls clojure, should know whther stat has been set
        
        bool hasEpsilonProduction(string);
        set<string> first(const string&);
        
        unordered_map<string,symbol> grammar;
        state start;
    public:
        Dfa();
        Dfa(const unordered_map<string,symbol>&);
        ~Dfa();
};

#endif