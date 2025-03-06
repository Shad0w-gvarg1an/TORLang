#ifndef PARSER_H
#define PARSER_H

#include "Scanner.h"
#include "Stack.h"
#include "Poliz.h"


class Ident {
public:
    Ident() { 
    }
    const std::string& get_name() { return name; }
    Type_of_lex get_type() { return type; } 

    void put_name(const std::string& s) { name = s; }
    void put_type(Type_of_lex t) { type = t; }
private:
    std::string name;
    Type_of_lex type;
};

//typedef std::vector<Ident> tabl_ident;
class tabl_ident {
public:
    Ident& operator[](size_t k) { return idents[k]; }
    size_t find(const string& key) {
        for (size_t i = 0; i < idents.size(); ++i) {
            if (idents[i].get_name() == key)
                return i;
        }
        return -1;
    }
    size_t put(const string& name) {
        if (find(name) != -1) return -1;
        idents.emplace_back();
        idents.back().put_name(name);
        return idents.size() - 1;
    }
    size_t size() { return idents.size(); }
private:
    std::vector<Ident> idents;
};

class Func {
public:
    void put_params(const std::vector<Type_of_lex>& seq) {
        params = seq;
    }
    const std::vector<Type_of_lex> get_params() {
        return params;
    }
    const std::string& get_name() { return name; }
    Type_of_lex get_type() { return type; } 

    void put_name(const std::string& s) { name = s; }
    void put_type(Type_of_lex t) { type = t; }
private:
    std::string name;
    Type_of_lex type;
    std::vector<Type_of_lex> params;
};

class tabl_func {
public:
    Func& operator[](size_t k) { return funcs[k]; }
    size_t find(const string& key) {
        for (size_t i = 0; i < funcs.size(); ++i) {
            if (funcs[i].get_name() == key)
                return i;
        }
        return -1;
    }
    size_t put(const string& name, const std::vector<Type_of_lex>& params) {
        if (find(name) != -1) return -1;
        funcs.emplace_back();
        funcs.back().put_name(name);
        funcs.back().put_params(params);
        return funcs.size() - 1;
    }
private:
    std::vector<Func> funcs;
};

struct Elem {
    Type_of_lex type;
    std::string val;
    bool is_lvalue;
    Elem(Type_of_lex t, std::string s, bool isl) {
        type = t;
        val = s;
        is_lvalue = isl;
    }
    Elem(Type_of_lex t, std::string s) :
        Elem(t, s, false) {}
};

class Parser {
public:
    Parser(const string& program) : scan (program) {}
    void analyze();

private:
    std::pair<Type_of_lex, std::string> curr_lex; //*  тип и сама лексема
    Type_of_lex c_type; //* тип текущей лексемы
    std::string c_val; //* сама лексема
    Scanner scan; //? что это? 
    int last_ind = 0; //* индекс рассатриваемой функции

    Stack<Elem> st_lex;
    std::vector<tabl_ident> st_TID;
    tabl_func TFU;
    std::vector<Type_of_lex> params;
    int inside_cycles;
    size_t inside_function;

    void P(); void D(); void D1(std::pair<Type_of_lex, std::string> & lex_type, std::pair<Type_of_lex, std::string> & lex); void TYPE(); void IDENTSEQ();
    void PARAMSEQ(); void S(); 
    void BLOCK(); void IF(); void WITCH(); void ES(int & ind1, int & ind2); void DO(); void DELIM(); void RETURN(); void FOR(); 
    void E(); void ESEQ(int & col_param);
    void E1();  void E2(); void E3(); void E4(); void E5(); void E6(); void E7(); void E8(); void E9(); void E10(); void E11(); void E12(); void E13(); void E14(); void E15(); 

    // semantics 
    void push_id();
    void check_id();
    void create_TID();
    void del_TID();

    void new_func();

    void check_bin();
    void check_uno();
    void check_square_braces();
    void check_braces();
    void check_postfix();

    void check_return();
    void check_condition();
    void check_jump();

    void gl() {
        curr_lex = scan.get_lex();
        c_type = curr_lex.first;
        //printLex(c_type);
        c_val = curr_lex.second;
    }
};

#endif
