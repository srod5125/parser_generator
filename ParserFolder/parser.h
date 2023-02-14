#ifndef _PARSER_HEADER_
#define _PARSER_HEADER_

#include <string>
#include <stack>
#include <utility>
#include <vector>

#include "../LexerFolder/lexer.h"
#include "../CommonFolder/common.h"
#include "ParserTable/parserTable.h"
#include "../AstFolder/ast.h"


using std::string;
using std::stack;
using std::pair;
using std::vector;


class Parser{
    
    private:
        stack< pair<int,string> > pStack;
        ParserTable pT;
        //vector of tokens
        // input buffer + dollar sign
        int pointer;
        

    public:
        Parser();
        Parser(unordered_map<string,symbol>&);
        ~Parser();

        Ast parse(const vector<string>&);
        //void parse(vector<string>&&);
};

#endif