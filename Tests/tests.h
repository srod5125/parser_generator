#ifndef _TEST_HEADER_
#define _TEST_HEADER_

#include "../LexerFolder/lexer.h"
#include "../ParserFolder/DfaFolder/dfa.h"
#include "../CommonFolder/common.h"
#include "../ParserFolder/ParserTable/parserTable.h"


void runAllTest();
void testLexer1();
void testFirstDfa();
void testDfaClosure();
void testGoto1();
void testParserTable();

#endif