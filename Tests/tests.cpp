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

#define LOG(msg) std::cout << msg << std::endl;

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
    std::cout << "{ ";
    for(const auto& el: x){
        std::cout << el << " ";
    }
    std::cout << " }" << std::endl;
}
void testFirstDfa(){

    unordered_map<string,symbol> grammer;
    grammer["S'"] = symbol("S'",{"S"});
    grammer["S"] = symbol("S",vector<string>{"A","A"});
    grammer["A"] = symbol("A",vector<vector<string>>{{"a","A"},{"b"}});
    grammer["a"] = symbol({"a"});
    grammer["b"] = symbol({"b"});
    
    unordered_map<string,symbol> grammer3;
    grammer3["S'"] = symbol("S'",{"P"});
    grammer3["P"] = symbol("P",{"E"});
    grammer3["E"] = symbol("E",vector<vector<string>>{{"E","+","T"},{"T"}});
    grammer3["T"] = symbol("T",vector<vector<string>>{{"id","(","E",")"},{"id"}});
    grammer3["+"] = symbol({"+"});
    grammer3["id"] = symbol({"id"});
    grammer3["("] = symbol({"("});
    grammer3[")"] = symbol({")"});

    unordered_map<string,symbol> grammer4;
    grammer4["S"] = symbol("S",vector<vector<string>>{{"A","S"},{"b"}});
    grammer4["A"] = symbol("A",vector<vector<string>>{{"S","A"},{"a"}});
    grammer4["a"] = symbol({"a"});
    grammer4["b"] = symbol({"b"});

    unordered_map<string,symbol> grammer5;
    grammer5["S"] = symbol("S",vector<vector<string>>{{"a","A","B"},{"B","a"}});
    grammer5["A"] = symbol("A",{"S"});
    grammer5["B"] = symbol("S",vector<vector<string>>{{"b"},{"EMPTY"}});
    grammer5["a"] = symbol({"a"});
    grammer5["b"] = symbol({"b"});
    grammer5["EMPTY"] = symbol();

    unordered_map<string,symbol> grammer6;
    grammer6["S"] = symbol("S",vector<string>{"a","B","D","h"});
    grammer6["B"] = symbol("B",vector<string>{"c","C"});
    grammer6["C"] = symbol("C",vector<vector<string>>{{"b","C"},{"EMPTY"}});
    grammer6["D"] = symbol("D",vector<string>{"E","F"});
    grammer6["E"] = symbol("E",vector<vector<string>>{{"g"},{"EMPTY"}});
    grammer6["F"] = symbol("F",vector<vector<string>>{{"f"},{"EMPTY"}});
    grammer6["a"] = symbol({"a"});
    grammer6["h"] = symbol({"h"});
    grammer6["c"] = symbol({"c"});
    grammer6["g"] = symbol({"g"});
    grammer6["f"] = symbol({"f"});
    grammer6["EMPTY"] = symbol();

    Dfa d{grammer5};
    unordered_set<string> helper{};
    string t = "B";
    //unordered_set<string> x = d.first(t,helper); //make public to test
    LOG(t)
    //printSet(x);
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

    unordered_map<string,symbol> grammer3;
    grammer3["S'"] = symbol("S'",{"P"});
    grammer3["P"] = symbol("P",{"E"});
    grammer3["E"] = symbol("E",vector<vector<string>>{{"E","+","T"},{"T"}});
    grammer3["T"] = symbol("T",vector<vector<string>>{{"id","(","E",")"},{"id"}});
    grammer3["+"] = symbol({"+"});
    grammer3["id"] = symbol({"id"});
    grammer3["("] = symbol({"("});
    grammer3[")"] = symbol({")"});

    Dfa d{grammer3};

    line l{0,grammer3["S'"],{"$"}};
    unordered_set<line,line::hash> x;
    x.insert(l);
    state s{d.closure(x)};
    d.goToState(s);//init
}

void runAllTest(){
    testFirstDfa();
    //testLexer1();
    //testDfaClosure();
    //testGoto1();
}