#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "parser.hpp"

extern FILE* yyin;
extern int yylineno;


void yyerror(const char *msg) {
   std::cerr << "Err: "<< msg << "@" << yylineno << std::endl;
   yylineno = 1;
}

int main(int argc, char **argv) {
   if (argc > 1) {
      //yy_scan_string(argv[1]);
      yyin = fopen(argv[1], "r");
      if (!yyin){
         printf("syntax: %s filename\n", argv[0]);
         exit(1);
      }
   }
   yyparse(); // Calls yylex() for tokens.
   return 0;
}
