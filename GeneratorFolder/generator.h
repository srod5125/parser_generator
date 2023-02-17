#ifndef _GENERATOR_HEADER_
#define _GENERATOR_HEADER_

#include "../ParserFolder/parser.h"
#include "../CommonFolder/common.h"

#include <string>
#include <unordered_map>
#include <regex>

using std::string;
using std::unordered_map;
using std::regex;

using Grammar = unordered_map<string,symbol>;
using MatchingRules = unordered_map<string,regex>;


class Generator{
    private:
        MatchingRules ebnfMatchingRules;
        Parser ebnfParser;
        Grammar ebnfGrammar;

        MatchingRules userMatchingRules;
        Parser userParser;
        Grammar userGrammar;

        void disambiguateUserGrammar(); //TODO: preprocess grammar before feeding to parser to disambiguate

        void init();

    public:
        Generator();
        ~Generator();

        void rules(const string&);
};

#endif