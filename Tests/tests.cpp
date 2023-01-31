#include <iostream>
#include <cassert>
#include <string>
#include <unordered_map>
#include <regex>
#include <unordered_set>
#include <vector>

#include "tests.h"

#include "../LexerFolder/lexer.h"
#include "../ParserFolder/DfaFolder/dfa.h"
#include "../CommonFolder/common.h"

using std::string;
using std::unordered_map;
using std::regex;
using std::unordered_set;
using std::vector;

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
void printSet(const unordered_set<string>& x){
    for(const auto& el: x){
        std::cout << el << " ";
    }
    std::cout << std::endl;
}
void testFirstDfa(){

}

void testDfaClosure(){
    unordered_map<string,symbol> grammer;
    grammer["S'"] = symbol("S'",{"S"});
    grammer["S"] = symbol("S",vector<string>{"A","A"});
    grammer["A"] = symbol("A",vector<vector<string>>{{"a","A"},{"b"}});
    grammer["a"] = symbol({"a"});
    grammer["b"] = symbol({"b"});
    
    unordered_map<string,symbol> grammer2;
    grammer2["S'"] = symbol("S'",{"E"});
    grammer2["E"] = symbol("E",vector<vector<string>>{{"E","+","T"},{"T"}});
    grammer2["T"] = symbol("T",vector<vector<string>>{{"T","*","F"},{"F"}});
    grammer2["F"] = symbol("F",vector<vector<string>>{{"(","E",")"},{"id"}});
    grammer2["G"] = symbol("G",{"F"});
    grammer2["*"] = symbol({"*"});
    grammer2["+"] = symbol({"+"});
    grammer2["("] = symbol({"("});
    grammer2[")"] = symbol({")"});
    grammer2["id"] = symbol({"id"});

    //line l = line(1,grammer["A"],{"a","b"});
    line l = line(0,grammer2["G"],{"$"});
    unordered_set<line,line::hash> x;
    x.insert(l);
    //state s = state(0,l);
    Dfa d{grammer2};
    //d.closure(x); //make public to call
}

void testGoto1(){
    unordered_map<string,symbol> grammer;
    grammer["S'"] = symbol("S'",{"S"});
    grammer["S"] = symbol("S",vector<string>{"A","A"});
    grammer["A"] = symbol("A",vector<vector<string>>{{"a","A"},{"b"}});
    grammer["a"] = symbol({"a"});
    grammer["b"] = symbol({"b"});

    Dfa d{grammer};

    line l{0,grammer["S'"],{"$"}};
    unordered_set<line,line::hash> x;
    x.insert(l);
    state s{d.closure(x)};
    d.goToState(s);//init
}

void runAllTest(){
    //testFirstDfa();
    //testLexer1();
    //testDfaClosure();
    testGoto1();
}