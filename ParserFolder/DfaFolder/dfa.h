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
    line(int,symbol&,unordered_set<string>&&);
    line(int,symbol&,unordered_set<string>&);
    line(int,symbol&&,unordered_set<string>&);
    line(const line&,unordered_set<string>&);

    bool operator==(const line&) const;
    struct hash
    {
        std::size_t operator()( const line& ) const;
    };
    struct equal
    {
        bool operator()(const line&,const line&) const;
    };

    friend std::ostream& operator<< (std::ostream&, const line&);
};
enum class status : char{
    accept,
    closed,
    intermediate,
    start
};
//list of lines, list of transitions
struct state{
    unordered_set<line,line::hash,line::equal> productions;
    unordered_map<string, shared_ptr<state> > transitions; //one way pointer to new state
    int stateNum;
    status rank;

    state();
    state(int,line);
    state(const unordered_set<line,line::hash,line::equal>&);

    bool operator==(const state&) const;

    struct hash
    {
        std::size_t operator()( const state& ) const;
    };
    struct equal
    {
        bool operator()(const state&,const state&) const;
    };

    friend std::ostream& operator<< (std::ostream&, const state&);
};

struct initProdsHash {
    std::size_t operator()(const unordered_set<line,line::hash,line::equal>&) const;
};
struct initProdsEqual {
    bool operator()(const unordered_set<line,line::hash,line::equal>&,const unordered_set<line,line::hash,line::equal>&) const;
};
class Dfa {
    private:
        bool hasEpsilonProduction(string);
        
        
        unordered_map<string, unordered_set<string> > firstCache;
        
        unordered_map< unordered_set<line,line::hash,line::equal>, shared_ptr<state>, initProdsHash, initProdsEqual> initProdSMap;
        
        void goToState(state&);//recurisve calls clojure, should know whther stat has been set

    public:
        shared_ptr<state> startPtr;
        unordered_map<string,symbol> grammar;
        int globalStateNum;

        shared_ptr<state> closure(unordered_set<line,line::hash,line::equal>);
        unordered_set<string> first(const string&,unordered_set<string>&);
        
        Dfa();
        Dfa(unordered_map<string,symbol>&);
        ~Dfa();

        //unordered_set<string> first(const string&);

};

#endif