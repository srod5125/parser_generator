#include <iostream>
#include <cassert>
#include <string>
#include <unordered_map>
#include <regex>
#include <iostream>
#include <set>

#include "tests.h"

#include "../LexerFolder/lexer.h"
#include "../ParserFolder/DfaFolder/dfa.h"
#include "../CommonFolder/common.h"

using std::string;
using std::unordered_map;
using std::regex;
using std::set;

//isolated test for lexer to see if it splits string correctly
void testLexer1(){
    string text{"gg 123 gg"};
    
    regex r1("gg");
    regex r2("[0-9]+");

    unordered_map<string,regex> m;
    m["WORD"] = r1;
    m["NUM"] = r2;

    Lexer l{text,m};

    std::cout << l;
}
void printSet(const set<string>& x){
    for(const auto& el: x){
        std::cout << el << " ";
    }
    std::cout << std::endl;
}
void testFirstDfa(){
    unordered_map<string,symbol> grammer;
    grammer["E"] = symbol("E",{{"T","E'"}});
    grammer["E'"] = symbol("E'",{{"+","T","E'"},{"EMPTY"}});
    grammer["+"] = symbol({"+"});
    grammer["EMPTY"] = symbol();
    grammer["T"] = symbol("T",{{"F","T'"}});
    grammer["T'"] = symbol("T'",{{"*","T'"},{"EMPTY"}});
    grammer["*"] = symbol({"*"});
    grammer["F"] = symbol("F",{{"(","E",")"},{"id"}});
    grammer["("] = symbol({"("});
    grammer[")"] = symbol({")"});
    grammer["id"] = symbol({"id"});

    unordered_map<string,symbol> grammer2;
    grammer2["E"] = symbol("E",{{"T","X"}});
    grammer2["X"] = symbol("X",{{"+","E"},{"EMPTY"}});
    grammer2["EMPTY"] = symbol();
    grammer2["T"] = symbol("T",{{"int","Y"},{"(","E",")"}});
    grammer2["("] = symbol({"("});
    grammer2[")"] = symbol({")"});
    grammer["int"] = symbol({"int"});
    grammer2["Y"] = symbol("Y",{{"*","T"},{"EMPTY"}});
    grammer2["*"] = symbol({"*"});

    Dfa d{grammer2};
    string tmp = "E";
    //set<string> x = d.first(tmp); //make public to test
    //std::cout << "First(" << tmp << "): ";
    //printSet(x);
}

void runAllTest(){
    //testFirstDfa();
    testLexer1();
}