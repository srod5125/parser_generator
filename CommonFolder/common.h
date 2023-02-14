#ifndef _COMMON_HEADER_
#define _COMMON_HEADER_

#include <string>
#include <vector>
#include <unordered_set>


using std::string;
using std::vector;
using std::unordered_set;

struct token
{
    //is terminal, production rules
    string tag;
    string terminal;

    token();
    token(const string&,const string&); //tag, terminal  //ERROR: cannot initliaze memebers
    token(const string&&,const string&&); 
    token(const string&&); 
    //token(const string,const string); //tag, terminal
    bool operator==(const token&) const;

    friend std::ostream& operator<< (std::ostream&, const token&);
};


struct symbol{
    bool isTerminal;
    string name;
    token t;

    vector<vector<string>> production_rule; // s-> { {a,B,c}, {X,T} }

    symbol();
    symbol(const string&&, const vector<vector<string>>&&); //name,  r value initilization for production_rule
    symbol(const string&&, const vector<string>&&);
    symbol(const string&, const vector<string>&&);
    symbol(token&);
    symbol(token&&);

    bool operator==(const symbol&) const;

    friend std::ostream& operator<< (std::ostream&, const symbol&);
};

//interface for hashable structures
struct index {
    virtual bool operator==(const index&); //TODO: implement as abstract class
    struct hash
    {
        virtual std::size_t operator()( const index& ) const = 0;
    };
    struct equal
    {
        virtual bool operator()(const index&,const index&) const = 0;
    };
};

template <typename T>
void printSet(const unordered_set<T>& x);

#endif


