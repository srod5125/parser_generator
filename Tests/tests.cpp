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

}

void testDfaClosure(){
    unordered_map<string,symbol> grammer;
    grammer["S"] = symbol("S",vector<string>{"A","A"});
    grammer["A"] = symbol("A",vector<vector<string>>{{"a","A"},{"b"}});
    grammer["S'"] = symbol("S'",{"S"});
    grammer["a"] = symbol({"a"});
    grammer["b"] = symbol({"b"});

    line l = line(0,grammer["S'"],{"$"});
    //set<line> x;
    //x.insert(l);
    //line l = line(1,grammer["A"],{"a","b"});
    //state s = state(0,l);

    Dfa d{grammer};

    //d.closure(s); //make public to call
}

void runAllTest(){
    //testFirstDfa();
    //testLexer1();
    testDfaClosure();
}