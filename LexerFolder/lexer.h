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
    const string tag;
    const string terminal;

    token();
    token(const string&,const string&); //tag, terminal
};

class Lexer {
    private:
        vector<token> tokens;
        void split(const string&,const unordered_map<string,regex>&);
    public:
        Lexer();
        ~Lexer();
        Lexer(const string&, const unordered_map<string,regex>&);
};

#endif