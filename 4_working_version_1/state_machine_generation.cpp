#include <iostream>
#include <map>
#include <set>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>

#include "Lexems.h"
#include "Node.h"

using namespace std;

const string LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const string DIGITS = "0123456789";

int main() {
    vector<Node> nodes(1);
    auto CreateNode = [&]() {
        nodes.emplace_back();
        return int(nodes.size()) - 1;
    };
    int root = CreateNode();
    auto AddString = [&](const string& s, Type_of_lex type) {
        Ptr v = root;
        for (auto c : s) {
            nodes[v].add(c, CreateNode());
            v = nodes[v].get(c).end()[-1];
        }
        nodes[v].add_term_type(type);
    };
    { // Добавить в автомат пробелы и прочие пробельные символы
        nodes[root].add(' ', root);
        nodes[root].add('\n', root);
        nodes[root].add('\r', root);
        nodes[root].add('\t', root);
    }
    {
        // добавить фиксированные лексемы 
        for (auto lex : lexem_lst) {
            AddString(lex.second, lex.first);
        }
        cout << "Fixed lexems added!" << endl;
    }
    {
        // добавить идентификаторы 
        Ptr a = CreateNode();
        for (char c : LETTERS) {
            nodes[root].add(c, a); 
        }
        for (char c : LETTERS + DIGITS) {
            nodes[a].add(c, a);
        }
        nodes[a].add_term_type(LEX_IDENT);

        cout << "Identifiers added!" << endl;
    }
    { //  добавить целочисленные и вещественные литералы
        Ptr a = CreateNode();
        for (char c : DIGITS) {
            nodes[root].add(c, a); 
        }
        for (char c : DIGITS) {
            nodes[a].add(c, a); 
        }
        nodes[a].add_term_type(LEX_INT_LITERAL);

        Ptr b = CreateNode();
        nodes[a].add('.', b);
        for (char ch : DIGITS) {
            nodes[b].add(ch, b); 
        }
        nodes[b].add_term_type(LEX_DOUBLE_LITERAL);

        cout << "Numbers added!" << endl;
    }
    {
        // добавить строковые литералы
        Ptr a = CreateNode();
        nodes[root].add('"', a); 
        // for (char c : ASCII) {
        for (int c = 0; c < (1 << 7); ++c) {
            if (c != '"') {
                nodes[a].add(c, a);
            }
        }
        Ptr b = CreateNode();
        nodes[a].add('"', b);
        nodes[b].add_term_type(LEX_STRING_LITERAL);

        cout << "String literals added!" << endl;
    }

    vector<vector<int>> whom(nodes.size());
    Ptr new_root = CreateNode();
    whom.push_back({root});
    map<vector<int>, int> complex_nodes;
    complex_nodes[whom[new_root]] = new_root;
    { // детерменизация
        vector<int> que(1, new_root);
        for (int i = 0; i < (int)que.size(); ++i) {
            int v = que[i];
            vector<pair<char, Ptr>> all_tos;
            for (int u : whom[v]) {
                auto lst = nodes[u].get_to();
                all_tos.insert(all_tos.end(), lst.begin(), lst.end()); 
            }
            sort(all_tos.begin(), all_tos.end());
            all_tos.erase(unique(all_tos.begin(), all_tos.end()), all_tos.end());
            int beg = 0;
            while (beg < (int)all_tos.size()) {
                int end = beg;
                vector<int> tmp;
                while (end < (int)all_tos.size() && all_tos[end].first == all_tos[beg].first) {
                    tmp.push_back(all_tos[end].second);
                    end += 1;
                }
                sort(tmp.begin(), tmp.end());
                tmp.erase(unique(tmp.begin(), tmp.end()), tmp.end());
                auto it = complex_nodes.find(tmp);
                if (it == complex_nodes.end()) {
                    int u = CreateNode();
                    whom.push_back(tmp);
                    complex_nodes[whom[u]] = u;

                    nodes[v].add(all_tos[beg].first, u);
                    que.push_back(u);
                } 
                else {
                    nodes[v].add(all_tos[beg].first, it->second);
                }
                beg = end;
            }
        }
        for (int v = new_root; v < (int)nodes.size(); ++v) {
            vector<Type_of_lex> types;
            for (int u : whom[v]) {
                auto tmp = nodes[u].get_term_type(); 
                types.insert(types.end(), tmp.begin(), tmp.end());
            }
            sort(types.begin(), types.end());
            types.erase(unique(types.begin(), types.end()), types.end());
            for (auto type : types) {
                nodes[v].add_term_type(type);
            }
        }
    }
    { // Выводит бор в файл, нумерация вершин с 1
        ofstream out("state_machine_to_analyze_lexems.txt");
        out << nodes.size() - new_root << '\n'; 
        for (int i = new_root; i < (int)nodes.size(); ++i) {
            auto tmp = nodes[i].get_term_type();
            out << tmp.size() << ' ';
            for (auto t : tmp) {
                out << (int)t << ' ';
            }
            out << '\n';
        }
        for (int i = new_root; i < (int)nodes.size(); ++i) {
            auto node = nodes[i];
            auto lst = node.get_to();
            for (auto edge : lst) 
                out << i - new_root + 1 << ' ' << (int)edge.first << ' ' << edge.second - new_root + 1 << '\n';
            out << "\n\n"; 
        }
        out.close();

        cout << "State machine generated..." << endl;
    }
    return 0;
}
