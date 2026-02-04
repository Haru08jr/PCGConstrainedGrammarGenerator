#include <iostream>
#include "patternmatcher.hpp"
#include "stringbuffer.hpp"
using namespace std;


enum Symbol {
    NUMBER, IDENTIFIER, 
    VAR, DEF, PRINTLN, WHILE, IF, ELSE, RETURN
};

struct Token {
    Symbol type;
    string lexeme;
    Token(Symbol s, string st) : type(s), lexeme(st) { } 
};


int main(int argc, char* argv[]) {
    string pattern = argv[1];
    bool looping = true;
    while (looping) {
        cout<<"> ";
        string input;
        getline(cin, input);
        if (input == "quit") {
            looping = false;
        } else if (input.substr(0, 5) == ".pat ") {
            pattern = input.substr(5);
            cout<<"Pattern changed to '"<<pattern<<"'"<<endl;
        } else {
            StringBuffer sb; sb.init(input);
            cout<<MatchRE()(sb, pattern, true)<<endl;
        }
    }
}