#ifndef _COMMON_HEADER_
#define _COMMON_HEADER_

#include <string>
#include <vector>


using std::string;
using std::vector;

struct token
{
    //is terminal, production rules
    string tag{};
    string terminal{};

    token();
    token(const string&,const string&); //tag, terminal  //ERROR: cannot initliaze memebers
    token(const string&&,const string&&); 
    //token(const string,const string); //tag, terminal
};


struct symbol{
    bool isTerminal;
    string name;
    token t;

    vector<vector<string>> production_rule; // s-> { {a,B,c}, {X,T} }

    symbol();
    symbol(const string&&, const vector<vector<string>>&&); //name,  r value initilization for production_rule
    symbol(const string&&, const string&&);
    symbol(token&);
    symbol(token&&);

    friend std::ostream& operator<< (std::ostream&, const symbol&);
};

#endif


