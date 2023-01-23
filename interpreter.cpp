#include <iostream>
#include <string>

#include "./ParserFolder/parser.h"
#include "./Tests/tests.h"


int main(){
    runAllTest();

    return 0;
}



//TODO: overload << operator for lexor
//TODO: set base class that raises errors
//TODO: implement testing class

/*
    layer ; lexer : 0, parser : 1
    error message
*/