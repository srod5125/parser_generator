#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>


using std::string;
using std::vector;
using std::unordered_set;


#include "common.h"

// token
token::token() : tag("EMPTY"), terminal("") {} // empty token init
token::token(const string& t,const string& ter) : tag(t), terminal(ter) { } //std::cout<<"constructor "<<terminal<<std::endl;}
token::token(string&& t,string&& ter) : tag{t}, terminal{ter} {}
token::token(const string& t,string&& ter) : tag(t), terminal(ter) { }
token::token(string&& t,const string& ter) : tag(t), terminal(ter) { }
token::token(string&& t) : tag{t}, terminal{t} {} // same
token::token(const string& t) : tag{t}, terminal{t} {}

bool token::operator==(const token& rhs) const{
    return this->tag == rhs.tag && this->terminal == rhs.terminal;
}

std::ostream& operator<< (std::ostream& out, const token& t){
    out << t.tag << "\t:\t" << t.terminal << std::endl;
    return out;
}

// symbol
symbol::symbol() : isTerminal{true}, name{"EMPTY"}, t{}, production_rule{} {}
symbol::symbol(const string&& n, const vector<vector<string>>&& rules){
    isTerminal=false;
    name=n;
    production_rule = rules;
}
symbol::symbol(const string&& n,  const vector<string>&& p) : name{n}{
    isTerminal=false;
    production_rule.emplace_back(p);
}
symbol::symbol(const string& n, const vector<string>&& p){
    name=n;
    isTerminal=false;
    production_rule.emplace_back(p);
}
symbol::symbol(token& t){
    isTerminal=true;
    name=t.tag;
    t=t;
}
symbol::symbol(token&& t){
    isTerminal=true;
    name=t.tag;
    t=t;
}
std::ostream& operator<< (std::ostream& out, const symbol& sym){
    if(sym.isTerminal){
        if(sym.t.tag == sym.t.terminal){
            out << sym.name;
        }
        else{
            out << sym.t.tag << "\t:\t" << sym.t.terminal;
        }
    }
    else{
        out << sym.name << "\t->\t";
        out << "{ ";
        for(const auto& seq: sym.production_rule){
            out << "{ ";
            for(const auto& el: seq){
                out << el;
                if(&el != &seq.back()){
                    out << ",";
                }
                out << " ";
            }
            out << "} ";
        }
        out << " }";
    }

    return out;
}

bool symbol::operator==(const symbol& rhs) const{
    if(this->isTerminal != rhs.isTerminal) {return false;}
    if(this->name != rhs.name) {return false;}
    if(this->production_rule != rhs.production_rule) {return false;}
    if(!(this->t == rhs.t)) {return false;}
    return true;
}


// template <typename T>
// void printSet(const unordered_set<T>& x){
//     std::cout << "{ ";
//     for(const auto& el: x){
//         std::cout << el << " ";
//     }
//     std::cout << " }" << std::endl;
// }