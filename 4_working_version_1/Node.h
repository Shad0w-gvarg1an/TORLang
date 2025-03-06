#ifndef NODE_H
#define NODE_H 

#include <vector>
#include <utility>
#include "Lexems.h"
using namespace std;

typedef int Ptr;

class Node {
public:
    const vector<Type_of_lex>& get_term_type() { return term_type; }
    const vector<pair<char, int>>& get_to() { return go; }
    void add_term_type(Type_of_lex type) { term_type.push_back(type); }
    void add(char ch, Ptr to) { go.push_back({ch, to}); }
    vector<Ptr> get(char ch) { 
        vector<Ptr> res;
        for (auto elem : go) {
            if (elem.first == ch) 
                res.push_back(elem.second);
        }
        return res;
    }
private:
  vector<pair<char, Ptr>> go;
  vector<Type_of_lex> term_type;
};

#endif
