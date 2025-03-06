#include <fstream>
#include <algorithm>
#include "Scanner.h"

Scanner::Scanner (const string& fn) {
    { // read the state machine
        root = 1;
        ifstream in("state_machine_to_analyze_lexems.txt");
        int V;
        in >> V;
        nodes.assign(V + 1, {});
        for (int i = root; i <= V; ++i) {
            int k;
            in >> k;
            vector<Type_of_lex> tmp; 
            for (int j = 0; j < k; ++j) {
                int t;
                in >> t;
                tmp.push_back(Type_of_lex(t));
            }
            sort(tmp.begin(), tmp.end());
            for (auto t : tmp) {
                nodes[i].add_term_type(t);
            }
        }
        int v, u;
        int c;
        while (in >> v >> c >> u) {
            nodes[v].add(c, u);
        }
        in.close();

        CS = root;
    }
    { // read the input file
        ifstream f(fn, ios::in|ios::binary);
        f.seekg(0, std::ios::end);
        size_t size = f.tellg();
        //input.resize(size);
        char* str = new char[size+1];
        f.seekg(0);
        f.read(str, size); // по стандарту можно в C++11, по факту работает и на старых компиляторах
        str[size] = 0;
        input = str;
        f.close(); 

        cout << input << endl;

        pos = 0;
        line_number = 0;
    }
    gc();
}

void Scanner::gc() {
    if (pos == input.size()) {
        c = EOF;
        return;
    }
    c = input[pos++]; 
}

std::pair<Type_of_lex, std::string> Scanner::get_lex() {
    CS = root;
    std::string val;
    do {
        if (CS == root) {
            val.clear();
        }
        const auto& moves = nodes[CS].get(c);
        if (!moves.empty()) {
            if (c == '\n') {
                line_number += 1;
            }
            val.push_back(c);
            CS = moves[0];
            gc();
        }
        else {
            const auto& lexems = nodes[CS].get_term_type();
            if (lexems.empty()) {
                throw make_tuple(std::string("Error UndefinedLexem"), line_number, c);
            }
            return {lexems[0], val};
        }
    } while (true);
}

size_t Scanner::get_line_number() {
    return line_number;
}
