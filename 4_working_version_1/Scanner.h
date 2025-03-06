#ifndef SCANNER_H
#define SCANNER_H

#include <vector>
#include <string>
#include <tuple>

#include "Lexems.h"
#include "Node.h"

class Scanner {
public:
    Scanner(const string& fn);
    std::pair<Type_of_lex, std::string> get_lex();
    size_t get_line_number();

private:
    char c;
    Ptr root, CS;
    std::vector<Node> nodes;
    
    std::string input;
    size_t pos;
    size_t line_number;

    void gc(); 
};



#endif
