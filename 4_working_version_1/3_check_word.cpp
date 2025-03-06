#include <iostream>
#include <cassert>
#include <set>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cassert>

#include "Lexems.h"
#include "Node.h"

using namespace std;

vector<Node> nodes(1);

Ptr go(Ptr v, const string& s) {
    for (char c : s) {
        if (!nodes[v].get(c).empty()) {
            assert(nodes[v].get(c).size() == 1);
            v = nodes[v].get(c)[0];
        } else { v = 0; break; }
    }
    return v;
}

int main() {
    auto CreateNode = [&]() {
        nodes.emplace_back();
        return int(nodes.size()) - 1;
    };
    Ptr root = 0;
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
            for (int j = 0; j < k; ++j) {
                int t;
                in >> t;
                nodes[i].add_term_type(Type_of_lex(t));
            }
        }
        int v, u;
        int c;
        while (in >> v >> c >> u) {
            nodes[v].add(c, u);
        }
        in.close();
    }
    auto Check = [&](string s) {
        int v = go(root, s);
        if (v && !nodes[v].get_term_type().empty()) {
            cout << "String " << s << " found!\ntype: ";
            //print(nodes[v].get_term_type());
            printFirst(nodes[v].get_term_type());
            cout << endl;
        } else {
            cout << "String " << s << " not found." << endl;
        }
    };

    Check("\"hello, world");

    return 0;
}
