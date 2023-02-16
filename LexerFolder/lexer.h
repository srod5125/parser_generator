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
        unordered_map<string,regex> matchingRules;

        vector<token> tokens;
    public:
        Lexer();
        ~Lexer();
        Lexer(const unordered_map<string,regex>&);
        const vector<token>& getTokens();

        void marchingSplit(const string&);//TODO: delete
        void marchingSplit(string&&);

        
        void split(const string&);
        void split(string&&);

        bool isNotSet();


        friend std::ostream& operator<< (std::ostream&, const Lexer&);
};

#endif