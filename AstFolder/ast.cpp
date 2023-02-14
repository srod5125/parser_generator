#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stack>

#include "ast.h"

using std::string;
using std::vector;
using std::shared_ptr;
using std::unique_ptr;
using std::stack;




//block
block::block(): val{""},connections{} {}
block::block(string name): val{name}, connections{} {}

//ast
Ast::Ast() {
    head = std::shared_ptr<block>();
}

Ast::~Ast() {}

std::ostream &operator<<(std::ostream& out , const Ast& ast) { //TODO: pretty print
    if(ast.head){
        stack<block> trace;
        trace.push(*(ast.head));

        block curr_block;
        int tabCount{1};

        while(!trace.empty()){
            curr_block = trace.top();
            trace.pop();
            //tabCount -= 1;
            out << std::string(trace.size(),'\t') << curr_block.val;//trace.size()

            for(const auto& c : curr_block.connections){
                trace.push(*c);
                //tabCount+=1;
            }
            out << std::endl;
            // if(curr_block.connections.size()>0){
            //     out << "\t";
            // }
        }
        out << std::endl;
    }
    else{
        out << "PARSING ERROR";
    }
    
    return out;
}
// S
//   E
//      N 1
//      O +
//      N 1
