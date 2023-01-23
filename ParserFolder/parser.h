#ifndef _PARSER_HEADER_
#define _PARSER_HEADER_

#include <string>
#include <unordered_map>
#include "../AstFolder/ast.h"


using std::string;
using std::unordered_map;

enum class Status: char { Error, Success };

class Parser{
    
    private:
        string dsl_string{};
        unordered_map<string, int> ruleToRuleMap;

    public:
        Parser();
        ~Parser();
        Parser(const string& dsl_rules);

        Ast getTree(const string& dsl_text);
};

#endif