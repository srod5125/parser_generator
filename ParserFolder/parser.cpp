
#include <string>
#include <stack>
#include <utility>
#include <vector>
#include <iostream>

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

#define LOG(msg) std::cout << msg << std::endl;


Parser::Parser() : pointer{0}, pT{} {
    //pStack.push({0,""});
}
Parser::Parser(unordered_map<string,symbol>& g) : pStack{}, pointer{0}, pT{} {
    pT = ParserTable(g);
    //pStack.push({0,""});
    //LOG(pT)
}

void Parser::parse(const vector<string>& input){
    //prime
    pStack.push({0,input[pointer]});
    move m = pT.getMove(0,input[pointer]);

    Ast ast;
    stack<shared_ptr<block>> parallelStack;
    parallelStack.push(std::make_unique<block>(input[pointer]));
    //LOG("first")
    //LOG(m.state<<" "<<getStepChar(m.s))

    while(!(m.s == step::error || m.s == step::accept)){
        
            if(m.s == step::shift) {
                pStack.push({m.state,input[pointer]});
                parallelStack.push(std::make_unique<block>(input[pointer]));
                ++pointer;
                //LOG("hit shift")
                //break;
            }
            else if(m.s == step::reduce){
                string val{m.nonterminal};//node creation   
                //reduce pop off 2*len of production rule
                shared_ptr<block> tmpBPtr = std::make_unique<block>(val);
                if(val == "S"){ ast.head = tmpBPtr; }     
                for(int i=0;i<m.len;i+=1){
                    //LOG("here"<<parallelStack.top()->val);
                    //if(parallelStack.top()){
                        tmpBPtr->connections.emplace_back(std::move(parallelStack.top()));
                    //}
                    parallelStack.pop();//add children
                    pStack.pop();
                }
                int gotoState = pStack.top().first;//get state need for goto
                
                
                m = pT.getMove(gotoState,m.nonterminal);
                //LOG("\tNT:"<<val<<" "<<m.state);
                //LOG(m.state<<" "<<getStepChar(m.s))
                //LOG("\tNT:"<<m.nonterminal)
                //if(m.s == step::error) {break;}//break out early
                
                pStack.push({m.state,val});
                parallelStack.push(std::move(tmpBPtr));
                //break;
            }
            else{ 
                //LOG("hit default")
                m.s = step::error;
                break;
            }
        
        //m = pT.getMove(pStack.top());
        m = pT.getMove(pStack.top().first,input[pointer]);
        
        //LOG("move")
        //LOG(m.state<<" "<<getStepChar(m.s)<<"\tTOP "<<pStack.top().first<<" "<<pStack.top().second)

        // string k = m.s == step::error ? "\tgot error" : "\tnope";
        // LOG(k)
        // LOG(static_cast<int>(m.s))

        if(pointer>=input.size()){
            m.s = step::error;
        }

        // for(int j=0;j<=pointer && j<input.size();j+=1){
        //     std::cout << input[j];
        // }
        // LOG("")
    }

    LOG(ast);

    if(m.s==step::accept){
        LOG("ACCPETED INPUT")
    }
    else{
        LOG("REJECTED INPUT")
        LOG(m.state<<" "<<getStepChar(m.s))
    }
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

