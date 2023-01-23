#include <vector>
#include <string>
#include <unordered_map>
#include <regex>
#include <map>
#include <utility>

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


token::token() : tag{"EMPTY"}, terminal{} {} // empty token init
token::token(const string& t,const string& ter) : tag{t}, terminal{terminal} {}

void Lexer::split(const string& t, const unordered_map<string,regex>& patternAndTag){
    // regex + tag name ; ex : /[0-9]+/, (NUMBER, '123')
    map<std::size_t,token> sortedMap;

    auto end = sregex_iterator();
    for(auto& pattern : patternAndTag){
        //iterater returns pointer to matched substrings
        // we supply current regex to new iterator
        for(auto matchIter = sregex_iterator(t.begin(),t.end(),pattern.second); matchIter != end ; ++matchIter){
            //map inserts by sort automatically, we instert by postion
            token t(pattern.first, matchIter->str());
            sortedMap.insert( pair<std::size_t, token>(matchIter->position(), t));
        }
    }
    //unpack contents of sorted map into tokens array
    for(auto& tok: sortedMap){
        tokens.emplace_back(tok.second);
    }
}

//decorate and set to tokens
// void Lexer::decorate(const vector<string>& splitTokens,const unordered_map<regex,string>& regexTagMap){
//     // regexTagMap = pattern + tag name
    
//     //loop through each token, loop through regular expression find match
//     for(const auto& tok : splitTokens){
//         for(const auto& map: regexTagMap){
//             //if valid terminal
//             if(regex_match(tok,map.first)){

//                 token t;
//                 t.tag = map.second; // tag assigned to regex
//                 t.terminal = tok;

//                 tokens.emplace_back(t);
//             }
//         }
//     }
// }


Lexer::Lexer():tokens{} { } // TODO: add default token
Lexer::~Lexer(){ }

Lexer::Lexer(const string& text, const unordered_map<string,regex>& patternAndTag){
    //string space{" "};
    //decorate(split(text,space), regexTagMap); // split stream passed to decorator
    split(text,patternAndTag);
}
