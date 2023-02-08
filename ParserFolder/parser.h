#ifndef _PARSER_HEADER_
#define _PARSER_HEADER_

#include <string>
#include <stack>
#include <utility>

#include "../AstFolder/ast.h"
#include "../LexerFolder/lexer.h"
#include "../CommonFolder/common.h"


using std::string;
using std::stack;
using std::pair;


class Parser{
    
    private:
        stack< pair<int,string> > pStack;
        //vector of tokens
        //parse table


    public:
        Parser();
        ~Parser();

        Ast getTree(const string& dsl_text);
};

#endif