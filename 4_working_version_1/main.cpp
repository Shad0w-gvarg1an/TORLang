#include "Parser.h"
#include <tuple>

int main() {
    try {
        Parser pars("input.txt");
        pars.analyze();
    } catch (const std::tuple<std::string, size_t, char>& er) {
        std::string s;
        size_t line;
        char c;
        std::tie(s, line, c) = er;
        cout << s << " at line " << line << " on symbol " << c << " (code: " << int(c) << ")" << endl;
    } catch (const std::tuple<std::pair<Type_of_lex, std::string>, size_t, std::string>& er) {
        std::pair<Type_of_lex, std::string> s;
        size_t line;
        std::string from;
        std::tie(s, line, from) = er;
        cout << "Error at line " << line + 1 << " throwed from " << from << "\nLexem: ";
        printLex(s.first);
        cout << "Value: " << s.second  << endl;
    } catch (const std::tuple<size_t, std::string>& er) {
        size_t line;
        std::string msg;
        std::tie(line, msg) = er;
        cout << "Error at line " << line + 1 << ": " << msg << endl;
    } catch (const std::tuple<Type_of_lex, size_t, std::string>& er) {
        Type_of_lex s;
        size_t line;
        std::string msg;
        std::tie(s, line, msg) = er;
        cout << "Error at line " << line + 1 << ": " << msg << endl << "Lexem: ";
        printLex(s);
    }
}

