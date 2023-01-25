#ifndef _PARSER_HEADER_
#define _PARSER_HEADER_

#include <string>
#include <unordered_map>
#include <vector>

#include "../AstFolder/ast.h"
#include "../LexerFolder/lexer.h"
#include "./DfaFolder/dfa.h"
#include "../CommonFolder/common.h"


using std::string;
using std::unordered_map;
using std::vector;


class Parser{
    
    private:
        string dsl_string{};
        
        unordered_map<string,symbol> grammar;
        //parser table -> class
        //dfa structure
        //tokens for ebnf grammar
        //lexer
        //closure, goto, first, follow

    public:
        Parser();
        ~Parser();
        Parser(const string& dsl_rules);

        Ast getTree(const string& dsl_text);
};

#endif