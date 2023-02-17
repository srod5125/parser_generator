#include "../ParserFolder/parser.h"
#include "../CommonFolder/common.h"

#include "generator.h"

#include <string>
#include <unordered_map>
#include <regex>

using std::string;
using std::unordered_map;
using std::regex;

using vecOfVec = vector<vector<string>>;


Generator::Generator() {
    init();
}

void Generator::init() {
    // Parser ebnfParser;

    //tokens for ebnf grammar
    ebnfMatchingRules["TERMINAL"] = regex("[A-Z]+");
    ebnfMatchingRules["NONTERMINAL"] = regex("[a-z][a-z|\\d]+");
    ebnfMatchingRules["LITERAL"] = regex("('\\w+')");
    ebnfMatchingRules["REGEX"] = regex("(/\\w+/)");
    ebnfMatchingRules["MOD"] = regex("[?|*|+]");
    ebnfMatchingRules["("] = regex("(");
    ebnfMatchingRules[")"] = regex(")");
    ebnfMatchingRules["["] = regex("[");
    ebnfMatchingRules["]"] = regex("]");
    ebnfMatchingRules["|"] = regex("|");
    ebnfMatchingRules["="] = regex("=");
    ebnfMatchingRules[";"] = regex(";");

    //grammar rules
    ebnfGrammar["S"]        = symbol("S",vecOfVec{{"RULE_SET"}});
    ebnfGrammar["RULE_SET"] = symbol("RULE_SET",vecOfVec{{"RULE","RULE_SET"},{"EMPTY"}});
    ebnfGrammar["RULE"]     = symbol("RULE",vecOfVec{{"NT_DEC",";"},{"T_DEC",";"}});
    //     terminal decloration
    ebnfGrammar["T_DEC"]    = symbol("T_DEC",vecOfVec{{"TERMINAL","=","REGEX"}});
    //     non terminal decloration
    ebnfGrammar["NT_DEC"]   = symbol("NT_DEC",vecOfVec{{"NONTERMINAL","=","EXPR"}});
    ebnfGrammar["EXPR"]     = symbol("EXPR",vecOfVec{
                                                {"TERMINAL"},
                                                {"LITERAL"},
                                                {"NONTERMINAL"},
                                                {"EXPR", "FOLLOW"}
                                            });
    ebnfGrammar["FOLLOW"]   = symbol("FOLLOW",vecOfVec{
                                                {"EXPR"},
                                                {"|", "EXPR"},
                                                {"[","EXPR","]"},
                                                {"(","EXPR",")"},
                                                {"EMPTY"}
                                            });
    ebnfGrammar["EMPTY"]    = symbol();
    //  terminals
    ebnfGrammar["NONTERMINAL"] = symbol({"NONTERMINAL"});
    ebnfGrammar["TERMINAL"] = symbol({"TERMINAL"});
    ebnfGrammar["REGEX"] = symbol({"REGEX"});
    ebnfGrammar["LITERAL"] = symbol({"LITERAL"});
    ebnfGrammar["MOD"] = symbol({"MOD"});
    ebnfGrammar["|"] = symbol({"|"});
    ebnfGrammar["["] = symbol({"["});
    ebnfGrammar["]"] = symbol({"]"});
    ebnfGrammar["("] = symbol({"("});
    ebnfGrammar[")"] = symbol({")"});
    ebnfGrammar["|"] = symbol({"|"});
    ebnfGrammar["="] = symbol({"="});
    ebnfGrammar[";"] = symbol({";"});

}

Generator::~Generator() {}