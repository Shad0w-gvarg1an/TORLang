#include "Lexems.h"

void printLex(Type_of_lex lex) {
    auto it = find_if(lexem_lst.begin(), lexem_lst.end(), [&](auto p) {
        return p.first == lex; 
    }) - lexem_lst.begin();
    if (it < (int)lexem_lst.size()) {
        cout << lex << " wich is " << lexem_lst[it].second << endl;
    } 
    else if (lex == LEX_IDENT) {
        cout << lex << " wich is identifier" << endl;
    }
    else {
        cout << lex << endl;
    }
}

void printFirst(const vector<Type_of_lex>& vec) {
    assert(!vec.empty());
    printLex(vec[0]);
}

