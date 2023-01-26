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
    grammer["E"] = symbol("E",vector<string>{"T","E'"});
    grammer["E'"] = symbol("E'",{{"+","T","E'"},{"EMPTY"}});
    grammer["+"] = symbol({"+"});
    grammer["EMPTY"] = symbol();
    grammer["T"] = symbol("T",vector<string>{"F","T'"});
    grammer["T'"] = symbol("T'",vector<vector<string>>{{"*","T'"},{"EMPTY"}});
    grammer["*"] = symbol({"*"});
    grammer["F"] = symbol("F",{{"(","E",")"},{"id"}});
    grammer["("] = symbol({"("});
    grammer[")"] = symbol({")"});
    grammer["id"] = symbol({"id"});

    unordered_map<string,symbol> grammer2;
    grammer2["E"] = symbol("E",vector<string>{"T","X"});
    grammer2["X"] = symbol("X",vector<vector<string>>{{"+","E"},{"EMPTY"}});
    grammer2["EMPTY"] = symbol();
    grammer2["T"] = symbol("T",{{"int","Y"},{"(","E",")"}});
    grammer2["("] = symbol({"("});
    grammer2[")"] = symbol({")"});
    grammer["int"] = symbol({"int"});
    grammer2["Y"] = symbol("Y",vector<vector<string>>{{"*","T"},{"EMPTY"}});
    grammer2["*"] = symbol({"*"});

    Dfa d{grammer2};
    string tmp = "E";
    //set<string> x = d.first(tmp); //make public to test
    //std::cout << "First(" << tmp << "): ";
    //printSet(x);
}

void testDfaClosure(){
    unordered_map<string,symbol> grammer;
    grammer["S"] = symbol("S",vector<string>{"A","A"});
    grammer["A"] = symbol("A",vector<vector<string>>{{"a","A"},{"b"}});
    grammer["S'"] = symbol("S'",{"S"});
    grammer["a"] = symbol({"a"});
    grammer["b"] = symbol({"b"});

    line l = line(0,grammer["S'"],{"$"});
    //line l = line(1,grammer["A"],{"a","b"});
    state s = state(0,l);

    Dfa d{grammer};

    //d.closure(s); //make public to call
}

void runAllTest(){
    //testFirstDfa();
    //testLexer1();
    testDfaClosure();
}