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


void Lexer::split(const string& s){
    // regex + tag name ; ex : /[0-9]+/, (NUMBER, '123')
    map<int,token> sortedMap;

    auto end = sregex_iterator();
    for(auto& pattern : matchingRules){
        //iterater returns pointer to matched substrings
        // we supply current regex to new iterator
        for(auto matchIter = sregex_iterator(s.begin(),s.end(),pattern.second); matchIter != end ; ++matchIter){
            //map inserts by sort automatically, we instert by postion
            //std::cout << pattern.first << " " << matchIter->str() << std::endl;
            //token t;
            token t(pattern.first, matchIter->str());
            //t.tag = pattern.first;
            //t.terminal = matchIter->str();
            sortedMap.insert({matchIter->position(),t});
        }
        //LOG("hit 4")//!HERE
    }
    //unpack contents of sorted map into tokens array
    regex whiteSpace("(\\s+)");
    // int gap=0;
    // for(const auto& tok: sortedMap){
    //     //here stratgey use substring size, and gaps to emplace back non matching string
    //     //gap = tok.first + tok.second.terminal.size() - 1;
    //     //LOG("pos:"<<tok.first<<" end:"<<tok.first+tok.second.terminal.size())
    //     LOG(tok.second)
    //     //tokens.emplace_back(tok.second);
    // }
    auto prevEl = sortedMap.begin();
    auto nextEl = ++sortedMap.begin();
    tokens.emplace_back(prevEl->second);
    //LOG("hit 5")
    while(nextEl!=sortedMap.end()){
        int afterPrevEl = prevEl->first+prevEl->second.terminal.size();
        //?LOG(afterPrevEl<<" "<<nextEl->first)
        //?LOG(s.substr(afterPrevEl,nextEl->first-afterPrevEl))
        //*ignoring white space
        
        if(nextEl->first > afterPrevEl){
            if(!regex_match(s.substr(afterPrevEl,nextEl->first-afterPrevEl),whiteSpace)){
                //TODO: throw error with unmatched
                tokens.emplace_back( token( "UNMATCHED",s.substr(afterPrevEl,nextEl->first-afterPrevEl) ) );
            }
        }
        tokens.emplace_back(nextEl->second);
        ++prevEl;
        ++nextEl;

        //LOG("hit 3")
    }
    //get last potenial unmatched tokens
    if(s.size() > prevEl->first+prevEl->second.terminal.size()){
        regex nonWhiteSpace("(\\S+)");
        string finalText{s.substr(prevEl->first+prevEl->second.terminal.size())};
        for(auto finalIter = sregex_iterator(finalText.begin(), finalText.end(), nonWhiteSpace);finalIter!=end;++finalIter){
            tokens.emplace_back( token("UNMATCHED",finalIter->str()));//get all non whitespace string and push as unmatched
        }
    }
}
void Lexer::split(string&& s){
    // regex + tag name ; ex : /[0-9]+/, (NUMBER, '123')
    map<int,token> sortedMap;

    auto end = sregex_iterator();
    for(auto& pattern : matchingRules){
        //iterater returns pointer to matched substrings
        // we supply current regex to new iterator
        for(auto matchIter = sregex_iterator(s.begin(),s.end(),pattern.second); matchIter != end ; ++matchIter){
            //map inserts by sort automatically, we instert by postion
            //std::cout << pattern.first << " " << matchIter->str() << std::endl;
            //token t;
            token t(pattern.first, matchIter->str());
            //t.tag = pattern.first;
            //t.terminal = matchIter->str();
            sortedMap.insert({matchIter->position(),t});
        }
        //LOG("hit 4")//!HERE
    }
    //unpack contents of sorted map into tokens array
    regex whiteSpace("(\\s+)");
    // int gap=0;
    // for(const auto& tok: sortedMap){
    //     //here stratgey use substring size, and gaps to emplace back non matching string
    //     //gap = tok.first + tok.second.terminal.size() - 1;
    //     //LOG("pos:"<<tok.first<<" end:"<<tok.first+tok.second.terminal.size())
    //     LOG(tok.second)
    //     //tokens.emplace_back(tok.second);
    // }
    auto prevEl = sortedMap.begin();
    auto nextEl = ++sortedMap.begin();
    tokens.emplace_back(prevEl->second);
    //LOG("hit 5")
    while(nextEl!=sortedMap.end()){
        int afterPrevEl = prevEl->first+prevEl->second.terminal.size();
        //?LOG(afterPrevEl<<" "<<nextEl->first)
        //?LOG(s.substr(afterPrevEl,nextEl->first-afterPrevEl))
        //*ignoring white space
        if(nextEl->first > afterPrevEl){
            if(!regex_match(s.substr(afterPrevEl,nextEl->first-afterPrevEl),whiteSpace)){
                //TODO: throw error with unmatched
                tokens.emplace_back( token( "UNMATCHED",s.substr(afterPrevEl,nextEl->first-afterPrevEl) ) );
            }
        }
        tokens.emplace_back(nextEl->second);
        ++prevEl;
        ++nextEl;

        //LOG("hit 3")
    }
    //get last potenial unmatched tokens
    if(s.size() > prevEl->first+prevEl->second.terminal.size()){
        regex nonWhiteSpace("(\\S+)");
        string finalText{s.substr(prevEl->first+prevEl->second.terminal.size())};
        for(auto finalIter = sregex_iterator(finalText.begin(), finalText.end(), nonWhiteSpace);finalIter!=end;++finalIter){
            tokens.emplace_back( token("UNMATCHED",finalIter->str()));//get all non whitespace string and push as unmatched
        }
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

bool Lexer::isNotSet(){
    return matchingRules.empty();
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
