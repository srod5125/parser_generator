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

std::ostream &operator<<(std::ostream& out , const Ast& ast) {
    stack<block> trace;
    trace.push(*(ast.head));

    block curr_block;
    int tabCount{1};

    while(!trace.empty()){
        curr_block = trace.top();
        trace.pop();
        //tabCount -= 1;

        out << std::string(trace.size(),'\t') << curr_block.val << std::endl;//trace.size()

        for(const auto& c : curr_block.connections){
            trace.push(*c);
            //tabCount+=1;
        }
    }

    return out;
}
