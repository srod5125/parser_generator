#include <iostream>
#include <string>

#include "./CommonFolder/common.h"
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

//g++ -g -std=c++17 $(find . -type f -iregex ".*\.cpp") -o interpreter




//LINKS
//https://norvig.com/lispy.html
//http://www.righto.com/2008/07/maxwells-equations-of-software-examined.html
//https://www.microsoft.com/en-us/research/wp-content/uploads/2017/10/program_synthesis_now.pdf
//https://lark-parser.readthedocs.io/en/latest/grammar.html
//https://www.unison-lang.org/community/
//https://www.youtube.com/watch?v=1ie8v0AZ8do
//https://www.shadaj.me/papers/katara.pdf
//https://www.cppstories.com/2021/filter-cpp-containers/
//https://cp-algorithms.com/algebra/module-inverse.html#finding-the-modular-inverse-using-euclidean-division

//https://www.youtube.com/watch?v=VSkfnRfNuwI
//https://www.youtube.com/watch?v=WfTNV2UEj2M&list=PLrjkTql3jnm-wW5XdvumCa1u9LjczipjA&index=14
//https://www.wikiwand.com/en/Extended_Backus%E2%80%93Naur_form
//https://www.youtube.com/@verificationandsynthesis5375/videos
//https://cs.stackexchange.com/questions/152523/how-is-the-lookahead-for-an-lr1-automaton-computed
//https://www.geeksforgeeks.org/lalr-parser-with-examples/
//https://lambda.uta.edu/cse5317/notes/node18.html


//general outline

//write parsing procedure
//  integrate lexer with parser
//  vector of tokens
//get ast from procedure
//hardcode ebnf gramamr
//using ast to construct second grammar