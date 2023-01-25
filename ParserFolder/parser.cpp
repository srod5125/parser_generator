#include <string>

#include "parser.h"
#include "../AstFolder/ast.h"
#include "../CommonFolder/common.h"

#include <iostream>


Parser::Parser() :dsl_string{""} { }
Parser::~Parser() { }

Parser::Parser(const std::string& dsl_rules) {
    dsl_string = dsl_rules;
}

Ast Parser::getTree(const string& dsl_text){
    Ast a;
    return a; // REMOVE later
}
