#include <vector>
#include <string>
#include <unordered_map>
#include <regex>
#include <map>
#include <utility>
#include <iostream>

using std::vector;
using std::string;
using std::unordered_map;
using std::map;

using std::regex_match;
using std::regex;
using std::sregex_iterator;
using std::pair;

#include "lexer.h"

//split string
//https://java2blog.com/split-string-space-cpp/


token::token() : tag("EMPTY"), terminal("\0") {} // empty token init
token::token(const string& t,const string& ter) : tag(t), terminal(terminal) {std::cout<<"constructor "<<terminal<<std::endl;}
//token::token(const string t,const string ter) : tag{t}, terminal{terminal} {}

void Lexer::split(const string& t, const unordered_map<string,regex>& patternAndTag){
    // regex + tag name ; ex : /[0-9]+/, (NUMBER, '123')
    map<std::size_t,token> sortedMap;

    auto end = sregex_iterator();
    for(auto& pattern : patternAndTag){
        //iterater returns pointer to matched substrings
        // we supply current regex to new iterator
        for(auto matchIter = sregex_iterator(t.begin(),t.end(),pattern.second); matchIter != end ; ++matchIter){
            //map inserts by sort automatically, we instert by postion
            //std::cout << pattern.first << " " << matchIter->str() << std::endl;
            token t;
            t.tag = pattern.first;
            t.terminal = matchIter->str();
            sortedMap.insert( pair<std::size_t,token>(matchIter->position(),t));
        }
    }
    //unpack contents of sorted map into tokens array
    for(auto& tok: sortedMap){
        //std::cout << tok.second.first << " " << tok.second.second << std::endl;
        //std::cout << tok.first << " " << tok.second << std::endl;
        //token t{tok.second.first, tok.second.second};
        //std::cout << t.tag;
        tokens.push_back(tok.second);
    }
}

std::ostream& operator<< (std::ostream& out, const Lexer& l)
{
    for(auto& tok: l.tokens){
        out << tok.tag << "\t:  " << tok.terminal << std::endl;
    }

    return out;
}

Lexer::Lexer():tokens{} { } // TODO: add default token
Lexer::~Lexer(){ }

Lexer::Lexer(const string& text, const unordered_map<string,regex>& patternAndTag){
    //string space{" "};
    //decorate(split(text,space), regexTagMap); // split stream passed to decorator
    split(text,patternAndTag);
}

