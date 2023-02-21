
#include <string>
#include <stack>
#include <utility>
#include <vector>
#include <iostream>
#include <regex>
#include <memory>

#include "../AstFolder/ast.h"
#include "../LexerFolder/lexer.h"
#include "../CommonFolder/common.h"
#include "ParserTable/parserTable.h"

#include "parser.h"

using std::string;
using std::stack;
using std::pair;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;
using std::regex;

#define LOG(msg) std::cout << msg << std::endl;


Parser::Parser() : pointer{0}, pT{} {
    //pStack.push({0,""});
}
Parser::Parser(unordered_map<string,symbol>& g) : pStack{}, pointer{0}, pT{ParserTable(g)} {
    // unordered_map<string,regex> matchingRules;
    
    // matchingRules["LITERAL"] = regex("'\\w+'");
}

void Parser::setLexer(const unordered_map<string,regex>& matchingRules){
    lex = Lexer(matchingRules);
}

//TODO: add expected list of tokens when err occurs
Ast Parser::parse(string& input){
    if(lex.isNotSet()){
        LOG("UNSET LEXER EARLY EXIT")
        return Ast();
    }
    input+="$";//append EOF
    //lex
    lex.split(input);
    LOG(lex)
    auto inputTokens{lex.getTokens()};

    LOG(pT)
    //prime
    pStack.push({0,""});
    move m = pT.getMove(0,inputTokens[pointer].tag);

    Ast ast;
    stack<shared_ptr<block>> parallelStack;
    parallelStack.push(std::make_unique<block>(inputTokens[pointer].terminal));
    LOG("first")
    LOG(m.state<<" "<<getStepChar(m.s))

    bool hitErrMove = false;

    while(!(m.s == step::error || m.s == step::accept)){
    
        if(m.s == step::shift) {
            pStack.push({m.state,inputTokens[pointer].tag});
            parallelStack.push(std::make_unique<block>(inputTokens[pointer].terminal));
            ++pointer;
            //LOG("hit shift")
            //break;
        }
        else if(m.s == step::reduce){
            string val{m.nonterminal};//node creation   
            //reduce pop off 2*len of production rule
            shared_ptr<block> tmpBPtr = std::make_unique<block>(val);
            //LOG(val)
            if(val == "S"){ ast.head = tmpBPtr; }// set start of tree
            for(int i=0;i<m.len;i+=1){
                //LOG("here"<<parallelStack.top()->val);
                //if(parallelStack.top()){
                    tmpBPtr->connections.emplace_back(std::move(parallelStack.top()));
                //}
                if(pStack.size()>1){//prevent empty stack
                    parallelStack.pop();
                    pStack.pop();
                    //?LOG("\t\t\t"<<pStack.size())
                }
            }
            int gotoState = pStack.top().first;//get state need for goto
            
            
            m = pT.getMove(gotoState,val);
            LOG("\tNT: "<<val<<" "<<m.state);
            LOG("\tNT: "<<m.state<<" "<<getStepChar(m.s)<<" "<<m.nonterminal)
            
            hitErrMove = m.s == step::error;//break out early
            
            pStack.push({m.state,val});
            parallelStack.push(std::move(tmpBPtr));
            //break;
        }
        else{ 
            //LOG("hit default")
            hitErrMove = true;
            break;
        }
    
        //m = pT.getMove(pStack.top());
        m = hitErrMove ? move() : pT.getMove(pStack.top().first,inputTokens[pointer].tag);
        
        LOG("move")
        LOG(m.state<<" "<<getStepChar(m.s)<<"\tTOP "<<pStack.top().first<<" ON: "<<input[pointer])

        // string k = m.s == step::error ? "\tgot error" : "\tnope";
        // LOG(k)
        // LOG(static_cast<int>(m.s))

        if(pointer>=input.size()){
            m.s = step::error;
        }

        for(int j=0;j<=pointer && j<input.size();j+=1){
            std::cout << input[j];
        }
        LOG("")
    }
    //LOG(ast.head->val)
    

    if(m.s==step::accept){
        LOG("ACCPETED INPUT")
        //LOG(ast);
    }
    else{
        LOG("REJECTED INPUT")
        //TODO: LOG STACK TRACE WHERE ERROR OCCURED
        //ask user how o resolve
        LOG(m.state<<" "<<getStepChar(m.s))
        for(int j=0;j<=pointer && j<input.size();j+=1){
            std::cout << input[j];
        }
        LOG("")
        LOG("TOP "<<pStack.top().first<<" "<<pStack.top().second)
    }

    return ast;
}

Ast Parser::parse(string&& input){//TODO: handle unmatched case
    if(lex.isNotSet()){
        LOG("UNSET LEXER EARLY EXIT")
        return Ast();
    }
    input+="$";//append EOF
    //lex
    lex.split(input);
    LOG(lex)
    auto inputTokens{lex.getTokens()};

    //LOG(pT)
    //prime
    pStack.push({0,""});
    move m = pT.getMove(0,inputTokens[pointer].tag);

    Ast ast;
    stack<shared_ptr<block>> parallelStack;
    parallelStack.push(std::make_unique<block>(inputTokens[pointer].terminal));
    LOG("first")
    LOG(m.state<<" "<<getStepChar(m.s))

    bool hitErrMove = false;

    while(!(m.s == step::error || m.s == step::accept)){
    
        if(m.s == step::shift) {
            pStack.push({m.state,inputTokens[pointer].tag});
            parallelStack.push(std::make_unique<block>(inputTokens[pointer].terminal));
            ++pointer;
            //LOG("hit shift")
            //break;
        }
        else if(m.s == step::reduce){
            string val{m.nonterminal};//node creation   
            //reduce pop off 2*len of production rule
            shared_ptr<block> tmpBPtr = std::make_unique<block>(val);
            //LOG(val)
            if(val == "S" || val == "start"){ ast.head = tmpBPtr; }// set start of tree
            for(int i=0;i<m.len;i+=1){
                //LOG("here"<<parallelStack.top()->val);
                //if(parallelStack.top()){
                    tmpBPtr->connections.emplace_back(std::move(parallelStack.top()));
                //}
                if(pStack.size()>1){//prevent empty stack
                    parallelStack.pop();
                    pStack.pop();
                    //?LOG("\t\t\t"<<pStack.size())
                }
            }
            int gotoState = pStack.top().first;//get state need for goto
            
            
            m = pT.getMove(gotoState,val);
            LOG("\tNT: "<<val<<" "<<m.state);
            LOG("\tNT: "<<m.state<<" "<<getStepChar(m.s)<<" "<<m.nonterminal)
            
            hitErrMove = m.s == step::error;//break out early
            
            pStack.push({m.state,val});
            parallelStack.push(std::move(tmpBPtr));
            //break;
        }
        else{ 
            //LOG("hit default")
            hitErrMove = true;
            break;
        }
    
        //m = pT.getMove(pStack.top());
        m = hitErrMove ? move() : pT.getMove(pStack.top().first,inputTokens[pointer].tag);
        
        LOG("move")
        LOG(m.state<<" "<<getStepChar(m.s)<<"\tTOP "<<pStack.top().first<<" ON: "<<input[pointer])

        // string k = m.s == step::error ? "\tgot error" : "\tnope";
        // LOG(k)
        // LOG(static_cast<int>(m.s))

        if(pointer>=input.size()){
            m.s = step::error;
        }

        for(int j=0;j<=pointer && j<input.size();j+=1){
            std::cout << input[j];
        }
        LOG("")
    }
    //LOG(ast.head->val)
    

    if(m.s==step::accept){
        LOG("ACCPETED INPUT")
        //LOG(ast);
    }
    else{
        LOG("REJECTED INPUT")
        //TODO: LOG STACK TRACE WHERE ERROR OCCURED
        //ask user how o resolve
        LOG(m.state<<" "<<getStepChar(m.s))
        for(int j=0;j<=pointer && j<input.size();j+=1){
            std::cout << input[j];
        }
        LOG("")
        LOG("TOP "<<pStack.top().first<<" "<<pStack.top().second)
    }

    return ast;
}
/*
void Parser::parse(const vector<string>& input){
    //prime
    pStack.push({0,input[pointer]});
    move m = pT.getMove(0,input[pointer]);
    //LOG("first")
    LOG(m.state<<" "<<getStepChar(m.s))

    while(!(m.s == step::error || m.s == step::accept)){
        
            if(m.s == step::shift) {
                pStack.push({m.state,input[pointer]});
                ++pointer;
                //LOG("hit shift")
                //break;
            }
            else if(m.s == step::reduce){                
                //reduce pop off 2*len of production rule
                for(int i=0;i<m.len;i+=1){
                    pStack.pop();//add children
                }
                int gotoState = pStack.top().first;//get state need for goto
                string val{m.nonterminal};//node creation
                
                m = pT.getMove(gotoState,m.nonterminal);
                LOG("\tNT:"<<val<<" "<<m.state);
                //LOG(m.state<<" "<<getStepChar(m.s))
                //LOG("\tNT:"<<m.nonterminal)
                //if(m.s == step::error) {break;}//break out early
                
                pStack.push({m.state,val});
                //break;
            }
            else{ 
                LOG("hit default")
                m.s = step::error;
                break;
            }
        
        //m = pT.getMove(pStack.top());
        m = pT.getMove(pStack.top().first,input[pointer]);
        
        LOG("move")
        LOG(m.state<<" "<<getStepChar(m.s)<<"\tTOP "<<pStack.top().first<<" "<<pStack.top().second)

        // string k = m.s == step::error ? "\tgot error" : "\tnope";
        // LOG(k)
        // LOG(static_cast<int>(m.s))

        if(pointer>=input.size()){
            m.s = step::error;
        }

        for(int j=0;j<=pointer && j<input.size();j+=1){
            std::cout << input[j];
        }
        LOG("")
    }

    if(m.s==step::accept){
        LOG("ACCPETED INPUT")
    }
    else{
        LOG("REJECTED INPUT")
        LOG(m.state<<" "<<getStepChar(m.s))
    }
}
*/

Parser::~Parser() {}


//         Ast getTree(const string& dsl_text);
//         void parse(string);

