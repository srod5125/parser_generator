#include <iostream>
#include <cassert>
#include <string>
#include <unordered_map>
#include <regex>
#include <unordered_set>
#include <vector>
#include <memory>

#include "tests.h"

#include "../LexerFolder/lexer.h"
#include "../ParserFolder/DfaFolder/dfa.h"
#include "../CommonFolder/common.h"
#include "../ParserFolder/ParserTable/parserTable.h"
#include "../ParserFolder/parser.h"
#include "../AstFolder/ast.h"


using std::string;
using std::unordered_map;
using std::regex;
using std::unordered_set;
using std::vector;
using std::shared_ptr;


#define LOG(msg) std::cout << msg << std::endl;
#define CONDLOG(cond,msgTrue,msgFalse) if(cond) {std::cout << msgTrue << std::endl;} else {std::cout << msgFalse << std::endl;}
#define PRINTSET(set) std::cout << "{"; for(const auto& el:set){ std::cout << el << " "; } std::cout << "}" << std::endl;

using vecOfVec = vector<vector<string>>;

//isolated test for lexer to see if it splits string correctly
void testLexer1(){
    //string text{"gg 123 gg"};
    

    unordered_map<string,regex> m;
    m["WORD"] = regex ("gg");
    m["NUM"] = regex ("([0-9]+)");

    
    unordered_map<string,regex> matchingRules;
    matchingRules["L"] = regex("b");
    matchingRules["R"] = regex("a");
    matchingRules["EOF"] = regex("[$]");

    // Lexer l{m};
    // l.split("gg 123 zzz      gg   ");
    Lexer l{matchingRules};
    l.split("aabb$ ");

    std::cout << l;
}

void testFirstDfa(){

    // unordered_map<string,symbol> grammer;
    // grammer["S'"] = symbol("S'",{"S"});
    // grammer["S"] = symbol("S",vector<string>{"A","A"});
    // grammer["A"] = symbol("A",vector<vector<string>>{{"a","A"},{"b"}});
    // grammer["a"] = symbol({"a"});
    // grammer["b"] = symbol({"b"});
    
    // unordered_map<string,symbol> grammer3;
    // grammer3["S'"] = symbol("S'",{"P"});
    // grammer3["P"] = symbol("P",{"E"});
    // grammer3["E"] = symbol("E",vector<vector<string>>{{"E","+","T"},{"T"}});
    // grammer3["T"] = symbol("T",vector<vector<string>>{{"id","(","E",")"},{"id"}});
    // grammer3["+"] = symbol({"+"});
    // grammer3["id"] = symbol({"id"});
    // grammer3["("] = symbol({"("});
    // grammer3[")"] = symbol({")"});

    // unordered_map<string,symbol> grammer4;
    // grammer4["S"] = symbol("S",vector<vector<string>>{{"A","S"},{"b"}});
    // grammer4["A"] = symbol("A",vector<vector<string>>{{"S","A"},{"a"}});
    // grammer4["a"] = symbol({"a"});
    // grammer4["b"] = symbol({"b"});

    // unordered_map<string,symbol> grammer5;
    // grammer5["S"] = symbol("S",vector<vector<string>>{{"a","A","B"},{"B","a"}});
    // grammer5["A"] = symbol("A",{"S"});
    // grammer5["B"] = symbol("S",vector<vector<string>>{{"b"},{"EMPTY"}});
    // grammer5["a"] = symbol({"a"});
    // grammer5["b"] = symbol({"b"});
    // grammer5["EMPTY"] = symbol();

    // unordered_map<string,symbol> grammer6;
    // grammer6["S"] = symbol("S",vector<string>{"a","B","D","h"});
    // grammer6["B"] = symbol("B",vector<string>{"c","C"});
    // grammer6["C"] = symbol("C",vector<vector<string>>{{"b","C"},{"EMPTY"}});
    // grammer6["D"] = symbol("D",vector<string>{"E","F"});
    // grammer6["E"] = symbol("E",vector<vector<string>>{{"g"},{"EMPTY"}});
    // grammer6["F"] = symbol("F",vector<vector<string>>{{"f"},{"EMPTY"}});
    // grammer6["a"] = symbol({"a"});
    // grammer6["h"] = symbol({"h"});
    // grammer6["c"] = symbol({"c"});
    // grammer6["g"] = symbol({"g"});
    // grammer6["f"] = symbol({"f"});
    // grammer6["EMPTY"] = symbol();

    unordered_map<string,symbol> grammer;
    grammer["S"] = symbol("S",vector<string>{"E"});
    grammer["E"] = symbol("E",vecOfVec{{"(","E",")"},{"N","O","E"},{"N"}});
    grammer["N"] = symbol("N",vecOfVec{{"1"},{"2"},{"3"}});
    grammer["O"] = symbol("O",vecOfVec{{"+"},{"-"}});
    grammer["("] = symbol({"("});
    grammer[")"] = symbol({")"});
    grammer["+"] = symbol({"+"});
    grammer["-"] = symbol({"-"});

    unordered_map<string,symbol> grammer4;
    grammer4["S"] = symbol("S",vecOfVec{{"S","S","+"},{"S","S","*"},{"a"},{"EMPTY"}});
    grammer4["+"] = symbol({"+"});
    grammer4["*"] = symbol({"*"});
    grammer4["a"] = symbol({"a"});
    grammer4["EMPTY"] = symbol();

    unordered_map<string,symbol> grammer7;
    grammer7["S"] = symbol("S",{"P"});
    //grammer["E"] = symbol("E",vecOfVec{{"(","P",")"}});
    //grammer["P"] = symbol("P",vecOfVec{{"(",")"},{"(","P",")"},{"EMPTY"}});
    grammer7["P"] = symbol("P",vecOfVec{{"EMPTY"},{"P","x"}});
    // grammer["("] = symbol({"("});
    grammer7["x"] = symbol({"x"});
    grammer7["$"] = symbol({"$"});
    grammer7["EMPTY"] = symbol();

    Dfa d{grammer7};
    set<string> helper{};
    string t = "P";
    //LOG(grammer4["S"])
    set<string> x = d.first(t,helper); //make public to test
    //LOG(t)
    PRINTSET(x);
}

void testDfaClosure(){
    unordered_map<string,symbol> grammer;
    grammer["S'"] = symbol("S'",vector<string>{"S"});
    grammer["S"] = symbol("S",vector<string>{"A","A"});
    grammer["A"] = symbol("A",vector<vector<string>>{{"a","A"},{"b"}});
    grammer["a"] = symbol({"a"});
    grammer["b"] = symbol({"b"});
    
    // unordered_map<string,symbol> grammer2;
    // grammer2["S'"] = symbol("S'",{"E"});
    // grammer2["E"] = symbol("E",vector<vector<string>>{{"E","+","T"},{"T"}});
    // grammer2["T"] = symbol("T",vector<vector<string>>{{"T","*","F"},{"F"}});
    // grammer2["F"] = symbol("F",vector<vector<string>>{{"(","E",")"},{"id"}});
    // grammer2["G"] = symbol("G",{"F"});
    // grammer2["*"] = symbol({"*"});
    // grammer2["+"] = symbol({"+"});
    // grammer2["("] = symbol({"("});
    // grammer2[")"] = symbol({")"});
    // grammer2["id"] = symbol({"id"});
    
    // unordered_map<string,symbol> grammer3;
    // grammer3["S'"] = symbol("S'",{"P"});
    // grammer3["P"] = symbol("P",{"E"});
    // grammer3["E"] = symbol("E",vector<vector<string>>{{"E","+","T"},{"T"}});
    // grammer3["T"] = symbol("T",vector<vector<string>>{{"id","(","E",")"},{"id"}});
    // grammer3["+"] = symbol({"+"});
    // grammer3["id"] = symbol({"id"});
    // grammer3["("] = symbol({"("});
    // grammer3[")"] = symbol({")"});

    
    unordered_map<string,symbol> grammer4;
    grammer4["S'"] = symbol("S'",{"S"});
    grammer4["S"] = symbol("S",vecOfVec{{"S","S","+"},{"S","S","*"},{"a"}});
    grammer4["+"] = symbol({"+"});
    grammer4["*"] = symbol({"*"});
    grammer4["a"] = symbol({"a"});

    unordered_map<string,symbol> grammer7;
    grammer7["S"] = symbol("S",{"E"});
    grammer7["E"] = symbol("E",vecOfVec{{"(","P",")"}});
    grammer7["P"] = symbol("P",vecOfVec{{"(",")"},{"(","P",")"},{"EMPTY"}});
    grammer7["("] = symbol({"("});
    grammer7[")"] = symbol({")"});
    grammer7["$"] = symbol({"$"});
    grammer7["EMPTY"] = symbol();

    
    unordered_map<string,symbol> grammer8;
    grammer8["S"] = symbol("S",{"P"});
    //grammer["E"] = symbol("E",vecOfVec{{"(","P",")"}});
    //grammer["P"] = symbol("P",vecOfVec{{"(",")"},{"(","P",")"},{"EMPTY"}});
    grammer8["P"] = symbol("P",vecOfVec{{"EMPTY"},{"P","x"}});
    // grammer["("] = symbol({"("});
    grammer8["x"] = symbol({"x"});
    grammer8["$"] = symbol({"$"});
    grammer8["EMPTY"] = symbol();


    //line l{1,grammer["A"],{"a","b"}};
    //{[E -> (.P ), $]}
    line l{0,grammer8["S"],{"$"}};
    unordered_set<line,line::hash,line::equal> x;
    x.insert(l);
    //state s = state(0,l);
    Dfa d{grammer8};
    shared_ptr<state> s{d.closure(x)}; //make public to call
    LOG(*s)
}

void testGoto1(){
    unordered_map<string,symbol> grammer;
    grammer["S'"] = symbol("S'",vector<string>{"S"});
    grammer["S"] = symbol("S",vector<string>{"A","A"});
    grammer["A"] = symbol("A",vector<vector<string>>{{"a","A"},{"b"}});
    grammer["a"] = symbol({"a"});
    grammer["b"] = symbol({"b"});
    grammer["$"] = symbol({"$"});

    // unordered_map<string,symbol> grammer2;
    // grammer2["S'"] = symbol("S'",{"E"});
    // grammer2["E"] = symbol("E",vector<vector<string>>{{"E","+","T"},{"T"}});
    // grammer2["T"] = symbol("T",vector<vector<string>>{{"T","*","F"},{"F"}});
    // grammer2["F"] = symbol("F",vector<vector<string>>{{"(","E",")"},{"id"}});
    // grammer2["G"] = symbol("G",{"F"});
    // grammer2["*"] = symbol({"*"});
    // grammer2["+"] = symbol({"+"});
    // grammer2["("] = symbol({"("});
    // grammer2[")"] = symbol({")"});
    // grammer2["id"] = symbol({"id"});

    unordered_map<string,symbol> grammer3;
    //grammer3["S'"] = symbol("S'",{"P"});
    grammer3["S"] = symbol("S",{"E"});
    grammer3["E"] = symbol("E",vecOfVec{{"E","+","T"},{"T"}});
    grammer3["T"] = symbol("T",vecOfVec{{"id","(","E",")"},{"id"}});
    grammer3["+"] = symbol({"+"});
    grammer3["id"] = symbol({"id"});
    grammer3["("] = symbol({"("});
    grammer3[")"] = symbol({")"});


    //line l{0,grammer3["S'"],{"$"}};
    //unordered_set<line,line::hash,line::equal> x;
    //x.insert(l);

    
    unordered_map<string,symbol> grammer4;
    grammer4["S"] = symbol("S",vecOfVec{{"S","S","+"},{"S","S","*"},{"a"}});
    grammer4["+"] = symbol({"+"});
    grammer4["*"] = symbol({"*"});
    grammer4["a"] = symbol({"a"});
    

    Dfa d{grammer};
    //shared_ptr<state> s{d.closure(x)};
    //LOG(*s)
    //s->rank = status::start;
    //s->stateNum = -1;
    //d.goToState(*s);//init
}

void testParserTable(){
    unordered_map<string,symbol> grammer;
    grammer["S"] = symbol("S",vector<string>{"A","A"});
    grammer["A"] = symbol("A",vector<vector<string>>{{"a","A"},{"b"}});
    grammer["a"] = symbol({"a"});
    grammer["b"] = symbol({"b"});
    grammer["$"] = symbol({"$"});

    ParserTable pT{grammer};
    LOG(pT);

    //test fillin
    //test merge
    //write getMove(int state,string val)
}

void testParser(){
    unordered_map<string,symbol> grammer;
    grammer["S"] = symbol("S",vector<string>{"A","A"});
    grammer["A"] = symbol("A",vector<vector<string>>{{"a","A"},{"b"}});
    grammer["a"] = symbol({"a"});
    grammer["b"] = symbol({"b"});
    grammer["$"] = symbol({"$"});
    //void parse(vector<string>&&);

    Parser p{grammer};
    //vector<string> input = {"a","a","b","b","$"}; //append end
    p.parse("aabb$");
}

void testAST(){
    unordered_map<string,symbol> grammer;
    grammer["S"] = symbol("S",vector<string>{"A","A"});
    grammer["A"] = symbol("A",vector<vector<string>>{{"l","A"},{"r"}});
    grammer["l"] = symbol({"l"});
    grammer["r"] = symbol({"r"});
    grammer["$"] = symbol({"$"});
    //void parse(vector<string>&&);'
    unordered_map<string,regex> matchingRules;
    matchingRules["l"] = regex("a");
    matchingRules["r"] = regex("b");
    matchingRules["$"] = regex("[$]");

    // matchingRules["L"] = regex("a");
    // matchingRules["R"] = regex("b");
    // matchingRules["EOF"] = regex("[$]");

    Parser p{grammer};
    p.setLexer(matchingRules);
    //string input = {"a","a","b","b","$"}; //append end //accepted grammar
    Ast ast =  p.parse("aabb$");
    LOG(ast)
}

void testAST2(){
    unordered_map<string,symbol> grammer;
    grammer["S"] = symbol("S",{"E"});
    grammer["E"] = symbol("E",vecOfVec{{"(","E",")"},{"N","O","E"},{"N"}});
    grammer["N"] = symbol("N",vecOfVec{{"1"},{"2"},{"3"}});
    grammer["O"] = symbol("O",vecOfVec{{"+"},{"-"}});
    grammer["("] = symbol({"("});
    grammer[")"] = symbol({")"});
    grammer["+"] = symbol({"+"});
    grammer["-"] = symbol({"-"});
    //grammer["/"] = symbol({"/"});
    //grammer["*"] = symbol({"*"});
    grammer["1"] = symbol({"1"});
    grammer["2"] = symbol({"2"});
    grammer["3"] = symbol({"3"});
    grammer["$"] = symbol({"$"});

    // S -> E
    // E -> ( E )
    // E -> N O E
    // E -> N
    // N -> 1
    // N -> 2
    // N -> 3
    // O -> +
    // O -> -
    //void parse(vector<string>&&);

    // unordered_map<string,symbol> grammer3;
    // grammer3["S"] = symbol("S",{"E"});
    // grammer3["E"] = symbol("E",vecOfVec{{"E","+","T"},{"T"}});
    // grammer3["T"] = symbol("T",vecOfVec{{"T","*","F"},{"F"}});
    // grammer3["F"] = symbol("F",vecOfVec{{"(","E",")"},{"id"}});
    // grammer3["+"] = symbol({"+"});
    // grammer3["*"] = symbol({"*"});
    // grammer3["id"] = symbol({"id"});
    // grammer3["("] = symbol({"("});
    // grammer3[")"] = symbol({")"});
    // grammer3["$"] = symbol({"$"});

    // unordered_map<string,symbol> grammer4;
    // grammer4["S"] = symbol("S",vecOfVec{{"S","S","+"},{"S","S","*"},{"a"}});
    // grammer4["+"] = symbol({"+"});
    // grammer4["*"] = symbol({"*"});
    // grammer4["a"] = symbol({"a"});
    
    unordered_map<string,regex> matchingRules;
    matchingRules["N"] = regex("a");
    matchingRules["$"] = regex("[$]");

    Parser p{grammer};
    //vector<string> input = {"a","a","+","$"}; //accepted grammar4
    //vector<string> input = {"id","*","id","+","id","$"}; //append end
    //vector<string> input = {"1","+","1","$"};
    
    Ast ast = p.parse("1+1$");
    LOG(ast)
}
void testLexerAndParser(){
    unordered_map<string,symbol> grammer;
    grammer["S"] = symbol("S",{"E"});
    grammer["E"] = symbol("E",vecOfVec{{"(","E",")"},{"N","O","E"},{"N"}});
    grammer["N"] = symbol({"N"});
    grammer["O"] = symbol({"O"});
    grammer["("] = symbol({"("});
    grammer[")"] = symbol({")"});
    grammer["$"] = symbol({"$"});
    
    unordered_map<string,regex> matchingRules;
    matchingRules["N"] = regex("([0-9]+)");
    matchingRules["O"] = regex("[+|-|/|*]");
    matchingRules["$"] = regex("[$]");

    Parser p{grammer};
    //vector<string> input = {"a","a","+","$"}; //accepted grammar4
    //vector<string> input = {"id","*","id","+","id","$"}; //append end
    //vector<string> input = {"1","+","1","$"};
    p.setLexer(matchingRules);
    Ast ast = p.parse("123+456*789");
    LOG(ast)
}
void testEpsilonProductions(){
    //balanced paren
    unordered_map<string,symbol> grammer;
    grammer["S"] = symbol("S",{"P"});
    //grammer["E"] = symbol("E",vecOfVec{{"(","P",")"}});
    //grammer["P"] = symbol("P",vecOfVec{{"(",")"},{"(","P",")"},{"EMPTY"}});
    grammer["P"] = symbol("P",vecOfVec{{"EMPTY"},{"x","P"}});
    // grammer["("] = symbol({"("});
    grammer["x"] = symbol({"x"});
    grammer["$"] = symbol({"$"});
    grammer["EMPTY"] = symbol();

    
    unordered_map<string,regex> matchingRules;
    // matchingRules["("] = regex("[(]");
    matchingRules["x"] = regex("[x]");
    matchingRules["$"] = regex("[$]");
    matchingRules["EMPTY"] = regex("^$");

    Parser p{grammer};
    p.setLexer(matchingRules);
    Ast ast = p.parse("xxxx");
    LOG(ast)
}
void testEpsilonProductions2(){
    //balanced paren, grammar is nont lalr
    unordered_map<string,symbol> grammer;
    grammer["S"] = symbol("S",{"E"});
    grammer["E"] = symbol("E",vecOfVec{{"(","P",")"}});
    grammer["P"] = symbol("P",vecOfVec{{"(",")"},{"(","P",")"},{"EMPTY"}});
    grammer["("] = symbol({"("});
    grammer[")"] = symbol({")"});
    grammer["$"] = symbol({"$"});
    grammer["EMPTY"] = symbol();

    
    unordered_map<string,regex> matchingRules;
    matchingRules["("] = regex("[(]");
    matchingRules[")"] = regex("[)]");
    matchingRules["$"] = regex("[$]");
    matchingRules["EMPTY"] = regex("^$");

    Parser p{grammer};
    p.setLexer(matchingRules);
    Ast ast = p.parse("()");
    LOG(ast)

}
//TODO: write more dfa tests
void runAllTest(){
    //testFirstDfa();
    //testLexer1();//TODO: test tinfinte loop for unmatched regex
    //testDfaClosure();
    //testGoto1();
    //testParserTable();
    //testParser();
    //testAST();
    //testAST2();
    //testLexerAndParser();
    testEpsilonProductions();
    //testEpsilonProductions2();
}