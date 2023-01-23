#include <iostream>
#include <string>

#include "./ParserFolder/parser.h"

int main(){
    std::cout << "ass" << std::endl;
    std::string a{"sdf"};
    
    std::string b;

    std::cin >> b;
    std::cout << b << std::endl;

    Parser p{a};
    return 0;
}



//TODO: overload << operator for lexor
//TODO: set base class that raises errors
//TODO: put classes in subfolders

/*
    layer ; lexer : 0, parser : 1
    error message
*/