#ifndef _DFA_HEADER_
#define _DFA_HEADER_

#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <vector>
#include <memory>
#include <string>


#include "../../CommonFolder/common.h"

using std::unordered_map;
using std::unordered_set;
using std::set;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;
using std::string;
using std::map;


//dot position, production list, lookahead set
struct line {
    int dotPosition;
    string name;
    vector<string> prod;
    set<string> lookahead;

    line(int,symbol&&,set<string>&&);
    line(int,symbol&,set<string>&&);
    line(int,symbol&,set<string>&);
    line(int,symbol&&,set<string>&);
    line(const line&,set<string>&);

    bool operator==(const line&) const;
    struct hash {
        std::size_t operator()( const line& ) const;
    };
    struct hash_withLk {
        std::size_t operator()( const line& ) const;
    };
    struct equal {
        bool operator()(const line&,const line&) const;
    };
    struct equal_withLk {
        bool operator()(const line&,const line&) const;
    };

    friend std::ostream& operator<< (std::ostream&, const line&);
};
enum class status : char{
    closed,
    intermediate,
    start
};
//list of lines, list of transitions
struct state{
    unordered_set<line,line::hash,line::equal> kernel;
    
    unordered_set<line,line::hash,line::equal> productions;
    map<string, shared_ptr<state> > transitions; //one way pointer to new state
    //TODO: make transitions regular set
    int stateNum;
    status rank;
    bool isAccepting;

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

struct coreHash {
    std::size_t operator()(const unordered_set<line,line::hash,line::equal>&) const;
};
struct coreEqual {
    bool operator()(const unordered_set<line,line::hash,line::equal>&,const unordered_set<line,line::hash,line::equal>&) const;
};
struct coreHash_WithLk {
    std::size_t operator()(const unordered_set<line,line::hash_withLk,line::equal_withLk>&) const;
};
struct coreEqual_WithLk {
    bool operator()(const unordered_set<line,line::hash_withLk,line::equal_withLk>&,const unordered_set<line,line::hash_withLk,line::equal_withLk>&) const;
};
struct coreManager {
    shared_ptr<state> statePtr;
    //std::size_t kernel_WithLK_Hash;
    unordered_set<line,line::hash_withLk,line::equal_withLk> kernelCopy;
    // unordered_map<unordered_set<line,line::hash_withLk,line::equal_withLk>, unordered_set<line,line::hash,line::equal>,
    // coreHash_WithLk,coreEqual_WithLk> kernelCopyMap;
    unordered_map<line,set<string>,line::hash,line::equal> kernelLookaheadCpyMap;
};
class Dfa {
    private:
        bool hasEpsilonProduction(string);
        
        
        unordered_map<string, set<string> > firstCache;
        
        unordered_map< unordered_set<line,line::hash,line::equal>, coreManager, coreHash, coreEqual> coreMap;
        
        void goToState(state&);//recurisve calls clojure, should know whther stat has been set

        
        shared_ptr<state> closure(unordered_set<line,line::hash,line::equal>);
        unordered_set<line,line::hash,line::equal> closure_noState(unordered_set<line,line::hash,line::equal>);
        set<string> first(const string&,set<string>&);

    public:
        shared_ptr<state> startPtr;
        unordered_map<string,symbol> grammar;
        int globalStateNum;

        
        Dfa();
        Dfa(unordered_map<string,symbol>&);
        ~Dfa();

        //unordered_set<string> first(const string&);

};

#endif