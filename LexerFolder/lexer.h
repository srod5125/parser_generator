#ifndef _LEX_HEADER_
#define _LEX_HEADER_

#include <vector>
#include <string>
#include <unordered_map>
#include <regex>
#include <iostream>

#include "../CommonFolder/common.h"

using std::vector;
using std::string;
using std::unordered_map;
using std::regex;


class Lexer {
    private:
        vector<token> tokens;
        void split(const string&,const unordered_map<string,regex>&);
    public:
        Lexer();//TODO: add default
        ~Lexer();
        Lexer(const string&, const unordered_map<string,regex>&);
        const vector<token>& getTokens();

        friend std::ostream& operator<< (std::ostream&, const Lexer&);
};

#endif