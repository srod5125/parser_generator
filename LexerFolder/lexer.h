#ifndef _LEX_HEADER_
#define _LEX_HEADER_

#include <vector>
#include <string>
#include <unordered_map>
#include <regex>


using std::vector;
using std::string;
using std::unordered_map;
using std::regex;

struct token
{
    //regex pattern;
    string tag;
    string terminal;
};

class Lexer {
    private:
        vector<token> tokens;
        const vector<string> split(const string&,const string&);
        void decorate(const vector<string>&,const unordered_map<regex,string>&);
    public:
        Lexer();
        ~Lexer();
        Lexer(const string&, const unordered_map<regex,string>&);
};

#endif