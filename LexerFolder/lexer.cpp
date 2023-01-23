#include <vector>
#include <string>
#include <unordered_map>
#include <regex>

using std::vector;
using std::string;
using std::unordered_map;
using std::regex_match;
using std::regex;

#include "lexer.h"

//split string
//https://java2blog.com/split-string-space-cpp/
const vector<string> Lexer::split(const string& t, const string& deliminator){
    string::size_type start = 0;
    string::size_type end = t.find(deliminator);

    vector<string> splitTokens;

    while(end != string::npos){
        splitTokens.emplace_back(t.substr(start,end-start));
        start = end + deliminator.size();
        end = t.find(deliminator,start);
    }
    splitTokens.emplace_back(t.substr(start,end-start)); //get final token

    return splitTokens;
}

//decorate and set to tokens
void Lexer::decorate(const vector<string>& splitTokens,const unordered_map<regex,string>& regexTagMap){
    // regexTagMap = pattern + tag name
    
    //loop through each token, loop through regular expression find match
    for(const auto& tok : splitTokens){
        for(const auto& map: regexTagMap){
            //if valid terminal
            if(regex_match(tok,map.first)){

                token t;
                t.tag = map.second; // tag assigned to regex
                t.terminal = tok;

                tokens.emplace_back(t);
            }
        }
    }
}


Lexer::Lexer():tokens{} { } // TODO: add default token
Lexer::~Lexer(){ }

Lexer::Lexer(const string& text, const unordered_map<regex,string>& regexTagMap){
    string space{" "};
    decorate(split(text,space), regexTagMap); // split stream passed to decorator
}
