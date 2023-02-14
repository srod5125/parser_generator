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

#include "../CommonFolder/common.h"
#include "lexer.h"

#define LOG(msg) std::cout << msg << std::endl;


//split string
//https://java2blog.com/split-string-space-cpp/


void Lexer::split(const string& t){
    // regex + tag name ; ex : /[0-9]+/, (NUMBER, '123')
    map<int,token> sortedMap;

    auto end = sregex_iterator();
    for(auto& pattern : matchingRules){
        //iterater returns pointer to matched substrings
        // we supply current regex to new iterator
        for(auto matchIter = sregex_iterator(t.begin(),t.end(),pattern.second); matchIter != end ; ++matchIter){
            //map inserts by sort automatically, we instert by postion
            //std::cout << pattern.first << " " << matchIter->str() << std::endl;
            token t;
            //token t(pattern.first, matchIter->str()); //ERROR here
            t.tag = pattern.first;
            t.terminal = matchIter->str();
            sortedMap.insert( pair<int,token>(matchIter->position(),t));
        }
    }
    //unpack contents of sorted map into tokens array
    for(auto& tok: sortedMap){
        tokens.emplace_back(tok.second);
    }
}
void Lexer::split(string&& t){
    // regex + tag name ; ex : /[0-9]+/, (NUMBER, '123')
    map<int,token> sortedMap;

    auto end = sregex_iterator();
    for(auto& pattern : matchingRules){
        //iterater returns pointer to matched substrings
        // we supply current regex to new iterator
        for(auto matchIter = sregex_iterator(t.begin(),t.end(),pattern.second); matchIter != end ; ++matchIter){
            //map inserts by sort automatically, we instert by postion
            //std::cout << pattern.first << " " << matchIter->str() << std::endl;
            //token t;
            token t(pattern.first, matchIter->str());
            //t.tag = pattern.first;
            //t.terminal = matchIter->str();
            sortedMap.insert({matchIter->position(),t});
        }
    }
    //unpack contents of sorted map into tokens array
    for(auto& tok: sortedMap){
        //here stratgey use substring size, and gaps to emplace back non matching string
        LOG("")
        tokens.emplace_back(tok.second);
    }
}

void Lexer::marchingSplit(const string& s){
    string::size_type lowerBound = 0;
    string::size_type upperBound = lowerBound+1;

    regex whiteSpace("s+");
    auto matchIter = sregex_iterator(s.begin(),s.end(),whiteSpace);

    string sub;
    auto end = sregex_iterator();
    //bool allFailed = false;

    while( lowerBound < s.size() ){

        sub = s.substr(lowerBound,upperBound);
        //allFailed = true;

        //go through all matches
        for(const auto& r : matchingRules){
            if(std::regex_match(sub,r.second)){
                token t{r.first,sub};
                tokens.emplace_back(t);
                //allFailed = false;
                lowerBound = upperBound+1;
                upperBound = 1;
                break;
            }
        }

        upperBound += 1;

        if( lowerBound == matchIter->position() && matchIter != end){
            lowerBound += matchIter->size();
            upperBound = 1;
            ++matchIter;
        }

        if( lowerBound+upperBound >= s.size() ){
            LOG("lexer failed")
            break;
            //TODO: emit lexer error
        }

    }
}

void Lexer::marchingSplit(string&& s){
    string::size_type lowerBound = 0;
    string::size_type upperBound = lowerBound+1;

    regex whiteSpace("\\s+");
    auto matchIter = sregex_iterator(s.begin(),s.end(),whiteSpace);

    string sub;
    auto end = sregex_iterator();
    //bool allFailed = false;

    while( lowerBound < s.size() ){
        std::cin.get();
        sub = s.substr(lowerBound,upperBound);
        LOG(sub<<" LB:"<<lowerBound<<" UP:"<<upperBound)
        //allFailed = true;
        //go through all matches
        //LOG("match pos "<<matchIter->position())
        if( lowerBound == matchIter->position() && matchIter != end){
            lowerBound += matchIter->size();
            upperBound = 1;
            ++matchIter;
            LOG("\thit space LB:"<<lowerBound)
        }
        else{
            for(const auto& r : matchingRules){
                if(std::regex_match(sub,r.second)){
                    token t{r.first,sub};
                    tokens.emplace_back(t);
                    //allFailed = false;
                    lowerBound += upperBound;
                    upperBound = 1;
                    LOG("\tmatch LB:"<<lowerBound<<" UP:"<<upperBound)
                    break;
                }
            }
            upperBound += 1;
        }


        if( lowerBound+upperBound >= s.size() ){
            break;
            //TODO: emit lexer error
        }

    }
    bool finalTokenFailed;
    for(const auto& r : matchingRules){
        if(std::regex_match(sub,r.second)){
            token t{r.first,sub};
            tokens.emplace_back(t);
            
            break;
        }
    }
}

std::ostream& operator<< (std::ostream& out, const Lexer& l)
{
    for(const auto& tok: l.tokens){
        out << tok;
    }

    return out;
}

const vector<token>& Lexer::getTokens(){
    return tokens;
}

Lexer::Lexer(): tokens{} { }
Lexer::~Lexer(){ }

Lexer::Lexer(const unordered_map<string,regex>& patternAndTag){
    //string space{" "};
    //decorate(split(text,space), regexTagMap); // split stream passed to decorator
    //split(text,patternAndTag);
    matchingRules = patternAndTag;
}
