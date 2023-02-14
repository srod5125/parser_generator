#ifndef _GENERATOR_HEADER_
#define _GENERATOR_HEADER_

#include "../ParserFolder/parser.h"
#include "../CommonFolder/common.h"

#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

using Grammar = unordered_map<string,symbol>;


class Generator{
    private:
        Parser ebnfParser;
        Parser userParser;

        Grammar ebnfGrammar;

        void disambiguateUserGrammar(); //TODO: preprocess grammar before feeding to parser to disambiguate

    public:
        Generator();
        ~Generator();

        void rules(const string&);
};

#endif