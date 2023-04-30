#include "../ParserFolder/parser.h"
#include "../CommonFolder/common.h"
#include "../AstFolder/ast.h"

#include "generator.h"

#include <string>
#include <unordered_map>
#include <regex>

using std::string;
using std::unordered_map;
using std::regex;

using vecOfVec = vector<vector<string>>;

#define LOG(msg) std::cout << msg << std::endl;

Generator::Generator() {
    init();
}

void Generator::init() {
    // Parser ebnfParser;

    //--------------tokens for ebnf grammar
    ebnfMatchingRules["TERMINAL"] = regex(R"([A-Z\d]+)");
    ebnfMatchingRules["NT"] = regex(R"([\w]+(?![^\/']))");//!HERE working on regex not recognizing new line // ^([^'/\\]([a-z][a-z\\d]*)+$)
    ebnfMatchingRules["LITERAL"] = regex(R"('[\w]+')");
    ebnfMatchingRules["REGEX"] = regex(R"(\/[\w]+\/)");
    ebnfMatchingRules["MOD"] = regex(R"([?*+])");
    ebnfMatchingRules["("] = regex("[(]");
    ebnfMatchingRules[")"] = regex("[)]");
    ebnfMatchingRules["["] = regex("[[]");
    ebnfMatchingRules["]"] = regex("[]]");
    ebnfMatchingRules["|"] = regex("[|]");
    ebnfMatchingRules["="] = regex("[=]");
    ebnfMatchingRules[";"] = regex("[;]");
    ebnfMatchingRules["$"] = regex("[$]");

    //-----------------grammar rules
    // rst is rule set
    ebnfGrammar["S"]        = symbol("S",vecOfVec{{"RST"}});
    ebnfGrammar["RST"] = symbol("RST",vecOfVec{{"RULE","RST"},{"EMPTY"}});
    ebnfGrammar["RULE"]     = symbol("RULE",vecOfVec{{"NT_DEC",";"},{"T_DEC",";"}});//
    //     terminal decloration
    ebnfGrammar["T_DEC"]    = symbol("T_DEC",vecOfVec{{"TERMINAL","=","REGEX"}});
    //     non terminal decloration
    ebnfGrammar["NT_DEC"]   = symbol("NT_DEC",vecOfVec{{"NT","=","EXPR"}});
    ebnfGrammar["EXPR"]     = symbol("EXPR",vecOfVec{
                                                {"TERMINAL"},
                                                {"LITERAL"},
                                                {"NT"},
                                                {"EXPR", "FOLLOW"}
                                            });
                //POSSIBLE ERR ON FOLLOW
    ebnfGrammar["FOLLOW"]   = symbol("FOLLOW",vecOfVec{
                                                {"EXPR"},
                                                {"|", "EXPR"},
                                                {"[","EXPR","]"},
                                                {"(","EXPR",")"},
                                                {"MOD"}
                                            });
    ebnfGrammar["EMPTY"]    = symbol();
    //  terminals
    ebnfGrammar["NT"] = symbol({"NT"});
    ebnfGrammar["TERMINAL"] = symbol({"TERMINAL"});
    ebnfGrammar["REGEX"] = symbol({"REGEX"});
    ebnfGrammar["LITERAL"] = symbol({"LITERAL"});
    ebnfGrammar["MOD"] = symbol({"MOD"});
    ebnfGrammar["|"] = symbol({"|"});
    ebnfGrammar["["] = symbol({"["});
    ebnfGrammar["]"] = symbol({"]"});
    ebnfGrammar["("] = symbol({"("});
    ebnfGrammar[")"] = symbol({")"});
    ebnfGrammar["="] = symbol({"="});
    ebnfGrammar[";"] = symbol({";"});
    ebnfGrammar["$"] = symbol({"$"});

    ebnfParser = Parser(ebnfGrammar);
    ebnfParser.setLexer(ebnfMatchingRules);



}

void Generator::rules(const string& ruleText){

}
void Generator::rules(string&& ruleText){
    Ast ast = ebnfParser.parse(ruleText);
    LOG(ast)
}

Generator::~Generator() {}