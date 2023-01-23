#include <iostream>
#include <cassert>
#include <string>
#include <unordered_map>
#include <regex>
#include <iostream>

#include "tests.h"

#include "../LexerFolder/lexer.h"

using std::string;
using std::unordered_map;
using std::regex;

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

void runAllTest(){
    testLexer1();
}