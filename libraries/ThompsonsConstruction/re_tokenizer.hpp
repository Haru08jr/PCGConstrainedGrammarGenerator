#ifndef tokenize_hpp
#define tokenize_hpp
#include <iostream>
#include <vector>
using namespace std;


enum RegExSymbol {
    RE_CHAR, RE_LPAREN, RE_RPAREN, RE_LSQUARE, RE_RSQUARE, 
    RE_STAR, RE_PLUS, RE_QUESTION, RE_CONCAT, RE_OR, RE_SPECIFIEDSET, 
    RE_SPECIFIEDRANGE, RE_QUANTIFIER, RE_NONE
};

inline vector<string> reSymStr = { 
    "RE_CHAR", "RE_LPAREN", "RE_RPAREN", "RE_LSQUARE", "RE_RSQUARE", 
    "RE_STAR", "RE_PLUS", "RE_QUESTION", "RE_CONCAT", "RE_OR", 
    "RE_SPECIFIEDSET", "RE_SPECIFIEDRANGE", "RE_QUANTIFIER", "TK_NONE"
};

struct RegExToken {
    RegExSymbol symbol;
    string characters;
    RegExToken(RegExSymbol s = RE_NONE, string ch = "nil") : symbol(s), characters(ch) { }
};

class Tokenizer {
    private:
        bool isChar(char c) {
            return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '.';
        }
        void setToken(RegExToken& nt, char ch, RegExSymbol sym) {
            string buff;
            buff.push_back(ch);
            nt.symbol = sym;
            nt.characters = buff;
        }
        void setSpecified(RegExToken& nt, string re, int& idx) {
            string buff;
            idx++;
            bool isRange = false;
            while (idx < re.length() && re[idx] != ']') {
                if (re[idx] == '-') 
                    isRange = true;
                buff.push_back(re[idx++]);
            }
            nt.characters = buff;
            nt.symbol = isRange ? RE_SPECIFIEDRANGE:RE_SPECIFIEDSET;
        }
        void setQuantifier(RegExToken& nt, string re, int& idx) {
            string buff;
            while (idx < re.length() && isdigit(re[idx])) {
                buff.push_back(re[idx++]);
            }
            nt.characters = buff;
            nt.symbol = RE_QUANTIFIER;
        }
    public:
        vector<RegExToken> tokenize(string re) {
            vector<RegExToken> tokens;
            for (int i = 0; i < re.size(); i++) {
                RegExToken nextToken;
                if (isdigit(re[i]) || isChar(re[i])) {
                    setToken(nextToken, re[i], RE_CHAR);
                } else {
                    switch (re[i]) {
                        case '@': { setToken(nextToken, re[i], RE_CONCAT); } break;
                        case '|': { setToken(nextToken, re[i], RE_OR); } break;
                        case '+': { setToken(nextToken, re[i], RE_PLUS); } break;
                        case '*': { setToken(nextToken, re[i], RE_STAR); } break;
                        case '?': { setToken(nextToken, re[i], RE_QUESTION); } break;
                        case '(': { setToken(nextToken, re[i], RE_LPAREN); } break;
                        case ')': { setToken(nextToken, re[i], RE_RPAREN); } break;
                        case '[': { setSpecified(nextToken, re, i); } break;
                        case ']': break;
                        case '{': { i++; setQuantifier(nextToken, re, i); } break;
                        case '}': break;
                        default:
                            break;
                    }
                }
                tokens.push_back(nextToken);
            }
            return tokens;
        }
};

#endif