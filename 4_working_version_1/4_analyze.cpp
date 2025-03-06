#include <iostream>
#include <cassert>
#include <set>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>

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

void print(const vector<Type_of_lex>& vec) {
    for (auto elem : vec) {
        cout << (int)elem << ' ';
    }
    cout << endl;
}

void printFirst(const vector<Type_of_lex>& vec) {
    assert(!vec.empty());
    auto id = vec[0];
    auto it = find_if(lexem_lst.begin(), lexem_lst.end(), [&](auto p) {
        return p.first == id; 
    }) - lexem_lst.begin();
    if (it < (int)lexem_lst.size()) {
        cout << id << " wich is " << lexem_lst[it].second << endl;
    } else {
        cout << id << endl;
    }
}


class LexicsAnalyzer {
public:
    LexicsAnalyzer();
    pair<vector<Type_of_lex>, string> GiveLexema(bool& succeed, int& iter, const string& prog);
    void Check(const string& s);
private:
    Ptr go(Ptr v, const string& s); 
    Ptr root;
    vector<Node> nodes;
};

LexicsAnalyzer::LexicsAnalyzer() : nodes(1) {
    auto CreateNode = [&]() {
        nodes.emplace_back();
        return int(nodes.size()) - 1;
    };
    root = 0;
    {
        root = 1;
        ifstream in("state_machine_to_analyze_lexems.txt");
        int V;
        in >> V;
        for (int i = 0; i < V; ++i) {
            CreateNode();
        }
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
    }
}


pair<vector<Type_of_lex>, string> LexicsAnalyzer::GiveLexema(bool& succeed, int& iter, const string& prog) {
    string SKIP_SYMBOLS = string(" \n") + char(32) + char(0) + char(9);
    while (iter < (int)prog.size() && SKIP_SYMBOLS.find(prog[iter]) != (size_t)-1) {
        iter += 1;
    }
    if (iter == (int)prog.size()) {
        succeed = false;
        return {{}, ""};
    }
    Ptr v = root;
    int beg = iter;
    char ch;
    do {
        ch = prog[iter]; 
        if (!nodes[v].get(ch).empty()) {
            assert(nodes[v].get(ch).size() == 1);
            v = nodes[v].get(ch)[0];
            iter += 1;
        }
        else break;
    } while (iter < (int)prog.size());
    if (nodes[v].get_term_type().empty()) {
        // exception
        throw string("lexem not found: [") + prog.substr(beg, iter - beg) + string("]\nSymbol code: ") + to_string(int(ch));
    }
    succeed = true;
    return {nodes[v].get_term_type(), prog.substr(beg, iter - beg)}; 
}

void LexicsAnalyzer::Check(const string& s) {
    int v = go(root, s);
    if (v) {
        cout << "String " << s << " found!\ntype: ";
        print(nodes[v].get_term_type());
        cout << endl;
    } else {
        cout << "String " << s << " not found." << endl;
    }
}

Ptr LexicsAnalyzer::go(Ptr v, const string& s) {
    for (char c : s) {
        if (!nodes[v].get(c).empty()) {
            assert(nodes[v].get(c).size() == 1);
            v = nodes[v].get(c)[0];
        } else { v = 0; break; }
    }
    return v;
}






int main() {
    string prog;
    { // read program
        ifstream f("in.txt");
         
        string fileName = "in.txt";
        f.seekg(0, std::ios::end);
        size_t size = f.tellg();
        prog.resize(size);
        f.seekg(0);
        f.read(&prog[0], size); // по стандарту можно в C++11, по факту работает и на старых компиляторах
        f.close(); 
    }
    LexicsAnalyzer an;
    int iter = 0;
    bool succeed;
    try {
        while (true) {
            auto lexem = an.GiveLexema(succeed, iter, prog); 
            if (!succeed) {
                break;
            }
            cout << lexem.second << " - ";
            printFirst(lexem.first);
        }
    } catch (string s) {
        cout << s << endl;
    }
    return 0;
}
