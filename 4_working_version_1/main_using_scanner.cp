#include "Scanner.h"

int main() {
    try {
        Scanner scan("input.txt");
        while (true)
        printLex(scan.get_lex());
    } catch (const char* s) {
        cout << s << endl;
    }
}

