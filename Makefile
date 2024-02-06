CC     = clang++
CFLAGS = -Wall -Wextra

BISON_FLAGS = -dvt
FLEX_FLAGS  = -i
GPP_FLAGS   =

BISON_INPUT = parser.ypp
FLEX_INPUT  = lexer.l

PARSER_CPP = parser.cpp
PARSER_HPP = parser.hpp
LEXER_CPP  = lexer.cpp
MAIN_CPP   = main.cpp

OUTPUT = parser



$(PARSER_CPP) $$(PARSER_HPP): $(BISON_INPUT)
	bison $(BISON_FLAGS) --header=$(PARSER_HPP) -o $(PARSER_CPP) $(BISON_INPUT)


$(LEXER_CPP): $(FLEX_INPUT)
	flex $(FLEX_FLAGS) -o $(LEXER_CPP) $(FLEX_INPUT)

# $(MAIN_CPP): $(PARSER_HPP)
# 	bison $(BISON_FLAGS) --define=$(PARSER_HPP) -o $(PARSER_CPP) $(BISON_INPUT)


$(OUTPUT): $(MAIN_CPP) $(PARSER_CPP) $(LEXER_CPP)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(MAIN_CPP) $(PARSER_CPP) $(LEXER_CPP) $(GPP_FLAGS)

clean:
	rm $(PARSER_HPP)
	rm $(PARSER_CPP)
	rm lexer.hpp 
