#include "Parser.h"
#include <tuple>
#include <iostream>
#include <sstream>

#ifdef DEBUG
#include <fstream>
std::ofstream debug("debug.txt");
#else 
#define debug if (false) cerr
#endif
Popoliz prog;
void Parser::push_id() {
    // take the last ident from stack
    tabl_ident& TID = st_TID.back();
    std::string dec_name = st_lex.pop().val;
    debug << "level of " << dec_name << ' ' << st_TID.size() << endl; 
    for (size_t it = 0; it < TID.size(); ++it) {
        debug << TID[it].get_name() << ' ';
    }
    debug << endl;
    size_t i = TID.put(dec_name);
    debug << i << endl;
    if (i == -1) {
        throw make_tuple(scan.get_line_number(), string("redeclaration of '") + dec_name + string("'")); 
    }
    if (st_lex.top().type == LEX_NOTHING) {
        throw make_tuple(scan.get_line_number(), string("declaration of element with type nothing")); 
    }
    TID[i].put_type(st_lex.top().type);
}

void Parser::check_id() {
    // take curr_lex
    size_t it = -1, jt = -1;
    debug << c_val << endl;
    for (size_t i = st_TID.size() - 1; i != (size_t)-1; --i) {
        size_t j = st_TID[i].find(c_val);
        if (j != (size_t)-1) {
            it = i;
            jt = j;
            break;
        }
        debug << i << ": ";
        for (size_t j = 0; j < st_TID[i].size(); ++j) {
            debug << st_TID[i][j].get_name() << ' ';
        }
        debug << endl;
    }


    if (it != (size_t)-1) {
        st_lex.push(Elem(st_TID[it][jt].get_type(), st_TID[it][jt].get_name(), true));
    }
    else 
        throw make_tuple(scan.get_line_number(), string("'") + c_val + string("' was not declared in this scope")); 
}

void Parser::create_TID() {
    debug << "create_TID" << endl;
    st_TID.emplace_back();
}

void Parser::del_TID() {
    debug << "del_TID" << endl;
    st_TID.pop_back();
}

void Parser::new_func() {
    // take the last ident from stack
    std::string dec_name = st_lex.pop().val;
    {
        size_t i = st_TID[0].find(dec_name);
        if (i != -1 && st_TID[0][i].get_type() != LEX_FUNC) {
            throw make_tuple(scan.get_line_number(), string("redeclaration of '") + dec_name + string("'")); 
        }
    }
    for (auto p : params) {
        if (p == LEX_NOTHING) 
            throw make_tuple(scan.get_line_number(), std::string("parameter with type nothing")); 
    }
    size_t i = TFU.put(dec_name, params);
    size_t j = st_TID[0].put(dec_name);
    if (i == -1) {
        throw make_tuple(scan.get_line_number(), string("redeclaration of '") + dec_name + string("'")); 
    }
    TFU[i].put_type(st_lex.top().type);
    st_TID[0][j].put_type(LEX_FUNC);
    inside_function = i;
}

void Parser::check_bin() {
    auto t2 = st_lex.pop();
    auto op = st_lex.pop();
    auto t1 = st_lex.pop();   

    switch (op.type) {
        case LEX_COMMA:
            st_lex.push(t2);         
            break;
        case LEX_EQUAL:
            if (!(t1.type == t2.type && t1.type != LEX_COMEIN && t1.type != LEX_GOOUT))
                throw make_tuple(scan.get_line_number(), string("wrong types are in operation '") + op.val + string("' (") + t1.val + string(", ") + t2.val + string(")")); 
            if (!t1.is_lvalue)
                throw make_tuple(scan.get_line_number(), string("in operation '") + op.val + string("' first operand must be lvalue")); 
            st_lex.push(t1);
            break;
        case LEX_PLUS_EQUAL:
        case LEX_MINUS_EQUAL:
        case LEX_MUL_EQUAL:
        case LEX_DIV_EQUAL:
        case LEX_MOD_EQUAL:
            if (!(t1.type == t2.type && (t1.type == LEX_INT || t1.type == LEX_DOUBLE)))
                throw make_tuple(scan.get_line_number(), string("wrong types are in operation '") + op.val + string("' (") + t1.val + string(", ") + t2.val + string(")")); 
            if (!t1.is_lvalue)
                throw make_tuple(scan.get_line_number(), string("in operation '") + op.val + string("' first operand must be lvalue")); 
            st_lex.push(t1);
            break;
        case LEX_AND_EQUAL:
        case LEX_OR_EQUAL:
        case LEX_LEFT_SHIFT_EQUAL:
        case LEX_RIGHT_SHIFT_EQUAL:
            if (!(t1.type == t2.type && (t1.type == LEX_INT)))
                throw make_tuple(scan.get_line_number(), string("wrong types are in operation '") + op.val + string("' (") + t1.val + string(", ") + t2.val + string(")")); 
            if (!t1.is_lvalue)
                throw make_tuple(scan.get_line_number(), string("in operation '") + op.val + string("' first operand must be lvalue")); 
            st_lex.push(t1);
            break;
        case LEX_OR:
        case LEX_AND:
            if (!(t1.type == t2.type && (t1.type == LEX_BOOL)))
                throw make_tuple(scan.get_line_number(), string("wrong types are in operation '") + op.val + string("' (") + t1.val + string(", ") + t2.val + string(")")); 
            st_lex.push(Elem(t1.type, string("result of ") + op.val));
            break;
        case LEX_BITWISE_OR:
        case LEX_BITWISE_XOR:
        case LEX_BITWISE_AND:
            if (!(t1.type == t2.type && (t1.type == LEX_INT)))
                throw make_tuple(scan.get_line_number(), string("wrong types are in operation '") + op.val + string("' (") + t1.val + string(", ") + t2.val + string(")")); 
            st_lex.push(Elem(t1.type, string("result of ") + op.val));
            break;
        case LEX_EQUALS:
        case LEX_NOT_EQUALS:
            if (!(t1.type == t2.type && t1.type != LEX_COMEIN && t1.type != LEX_GOOUT))
                throw make_tuple(scan.get_line_number(), string("wrong types are in operation '") + op.val + string("' (") + t1.val + string(", ") + t2.val + string(")")); 
            st_lex.push(Elem(LEX_BOOL, string("result of ") + op.val));
            break;
        case LEX_GREATER: 
        case LEX_LESS: 
        case LEX_GREATER_OR_EQUALS: 
        case LEX_LESS_OR_EQUALS: 
            if (!(t1.type == t2.type && (t1.type == LEX_INT || t1.type == LEX_DOUBLE)))
                throw make_tuple(scan.get_line_number(), string("wrong types are in operation '") + op.val + string("' (") + t1.val + string(", ") + t2.val + string(")")); 
            st_lex.push(Elem(LEX_BOOL, string("result of ") + op.val));
            break;
        case LEX_LEFT_SHIFT:
            if (!((t1.type == LEX_INT && t2.type == LEX_INT) || 
                (t1.type == LEX_GOOUT && (t2.type == LEX_INT || t2.type == LEX_STRING || t2.type == LEX_DOUBLE || t2.type == LEX_BOOL))))
                throw make_tuple(scan.get_line_number(), string("wrong types are in operation '") + op.val + string("' (") + t1.val + string(", ") + t2.val + string(")")); 
            st_lex.push(Elem(t1.type, string("result of ") + op.val));
            break;
        case LEX_RIGHT_SHIFT:
            if (!((t1.type == LEX_INT && t2.type == LEX_INT) || 
                (t1.type == LEX_COMEIN && (t2.type == LEX_INT || t2.type == LEX_STRING || t2.type == LEX_DOUBLE || t2.type == LEX_BOOL))))
                throw make_tuple(scan.get_line_number(), string("wrong types are in operation '") + op.val + string("' (") + t1.val + string(", ") + t2.val + string(")")); 
            st_lex.push(Elem(t1.type, string("result of ") + op.val));
            break;
        case LEX_PLUS:
        case LEX_MINUS:
        case LEX_MUL:
        case LEX_DIV:
            if (!(t1.type == t2.type && (t1.type == LEX_INT || t1.type == LEX_DOUBLE)))
                throw make_tuple(scan.get_line_number(), string("wrong types are in operation '") + op.val + string("' (") + t1.val + string(", ") + t2.val + string(")")); 
            st_lex.push(Elem(t1.type, string("result of ") + op.val));
            break; 
        case LEX_MOD:
            if (!(t1.type == t2.type && (t1.type == LEX_INT)))
                throw make_tuple(scan.get_line_number(), string("wrong types are in operation '") + op.val + string("' (") + t1.val + string(", ") + t2.val + string(")")); 
            st_lex.push(Elem(t1.type, string("result of ") + op.val));
            break;
        default: 
            throw make_tuple(scan.get_line_number(), string("unexpected operator '") + op.val + string("', need binary (operands: ") + t1.val + string(", ") + t2.val + string(")")); 
    }
}
void Parser::check_uno() {
    auto t = st_lex.pop();
    auto op = st_lex.pop();
    switch (op.type) {
        case LEX_INCR:
        case LEX_DECR:
            if (t.type != LEX_INT)
                throw make_tuple(scan.get_line_number(), string("wrong types are in operation '") + op.val + string("'")); 
            if (!t.is_lvalue)
                throw make_tuple(scan.get_line_number(), string("in operation '") + op.val + string("' operand must be lvalue")); 
             st_lex.push(t);
             break; 
        case LEX_PLUS:
        case LEX_MINUS:
            if (t.type != LEX_INT && t.type != LEX_DOUBLE)
                throw make_tuple(scan.get_line_number(), string("wrong types are in operation '") + op.val + string("'")); 
            st_lex.push(Elem(t.type, string("result of unary") + op.val));
            break;
        case LEX_NOT:
            if (t.type != LEX_BOOL)
                throw make_tuple(scan.get_line_number(), string("wrong types are in operation '") + op.val + string("'")); 
            st_lex.push(Elem(t.type, string("result of ") + op.val));
            break;
        case LEX_BITWISE_NOT:
            if (t.type != LEX_INT)
                throw make_tuple(scan.get_line_number(), string("wrong types are in operation '") + op.val + string("'")); 
            st_lex.push(Elem(t.type, string("result of ") + op.val));
            break;
        case LEX_SIZEOF:
            if (t.type == LEX_COMEIN || t.type == LEX_GOOUT)
                throw make_tuple(scan.get_line_number(), string("wrong types are in operation '") + op.val + string("'")); 
            st_lex.push(Elem(LEX_INT, string("result of ") + op.val));
            break;
        default:
            throw make_tuple(scan.get_line_number(), string("unexpected operator '") + op.val + string("' need uno")); 
    }
}
void Parser::check_braces() {
    auto r = st_lex.pop();
    if (r.type == LEX_RIGHT_SQUARE_BRACE) {
        auto t2 = st_lex.pop();
        auto l = st_lex.pop();
        auto t1 = st_lex.pop();
        if (t1.type != LEX_STRING || t2.type != LEX_INT)
            throw make_tuple(scan.get_line_number(), string("wrong types are in operation '[]'")); 
        st_lex.push(Elem(LEX_STRING, "result of []", t1.is_lvalue)); 
    }
    else if (r.type == LEX_RIGHT_BRACE) {
        std::vector<Type_of_lex> pars;
        std::vector<Elem> elem_pars;
        auto t = st_lex.pop();
        while (t.type != LEX_LEFT_BRACE) {
            pars.push_back(t.type);
            elem_pars.push_back(t);
            t = st_lex.pop();
        }
        t = st_lex.pop();
        reverse(pars.begin(), pars.end());
        reverse(elem_pars.begin(), elem_pars.end());
        if (t.type != LEX_FUNC) {
            throw make_tuple(t.type, scan.get_line_number(), string("not a function")); 
        }
        size_t i = TFU.find(t.val);
        if (i == -1) {
            throw make_tuple(scan.get_line_number(), string("undefined function '") + t.val + string("'")); 
        }
        if (TFU[i].get_params() != pars) {
            std::stringstream pars_str;
            for (size_t it = 0; it < elem_pars.size(); ++it) {
                pars_str << elem_pars[it].val;
                if (it + 1 < pars.size()) pars_str << ", ";
            }
            throw make_tuple(scan.get_line_number(), string("wrong parameters in function '") + t.val + string("' (") + pars_str.str() + string(")")); 
        }
        st_lex.push(Elem(TFU[i].get_type(), "result of function call", true));
    }
    else {
        throw make_tuple(scan.get_line_number(), string("unexpected operator '") + r.val + string("' need braces")); 
    }
}
void Parser::check_postfix() {
    auto op = st_lex.pop(); // ++ or --
    auto t = st_lex.pop();
    switch(op.type) { 
        case LEX_INCR:
        case LEX_DECR:
            if (t.type != LEX_INT)
                throw make_tuple(scan.get_line_number(), string("wrong types are in operation '") + op.val + string("'")); 
            if (!t.is_lvalue)
                throw make_tuple(scan.get_line_number(), string("in operation '") + op.val + string("' operand must be lvalue")); 
             st_lex.push(t);
             break; 
        default:
            throw make_tuple(scan.get_line_number(), string("unexpected operator '") + op.val + string("' need postfix")); 
    }
}

void Parser::check_return() {
    auto t = st_lex.pop();
    if (inside_function == -1) {
        throw make_tuple(scan.get_line_number(), string("jump '") + t.val + string("' must be inside a cycle")); 
    }
    Type_of_lex needs_type = TFU[inside_function].get_type(); 
    if (t.type == LEX_RETURN) {
        if (needs_type != LEX_NOTHING)
            throw make_tuple(scan.get_line_number(), string("function returns non nothing")); 
    }
    else {
        if (t.type != needs_type)
            throw make_tuple(scan.get_line_number(), string("returns expression of wrong type (") + t.val + string(")")); 
        st_lex.pop();
    }
}

void Parser::check_condition() {
    auto c = st_lex.pop();
    if (c.type != LEX_BOOL) {
        throw make_tuple(scan.get_line_number(), string("condition must be of boolean type (") + c.val + string(" found)")); 
    }
}

void Parser::check_jump() {
    auto j = st_lex.pop();
    if (inside_cycles == 0) {
        throw make_tuple(scan.get_line_number(), string("jump '") + j.val + string("' must be inside a cycle")); 
    }
}

void Parser::analyze() {
    inside_cycles = 0;
    inside_function = -1;

    gl();
    P();
    prog.print_popoliz();
}

void Parser::P () {
    prog.new_popoliz_of_func();
    create_TID();
    D();
    while (c_type != LEX_FIN) {
    // cout << "from D: ";
    // printLex(c_type);
    // cout << endl;
        D();
    }
    del_TID();
}

void Parser::D() {
    std::pair<Type_of_lex, std::string> lex_type = curr_lex;
    std::pair<Type_of_lex, std::string> lex;
    TYPE();
    if (c_type == LEX_IDENT) {
        lex = curr_lex;
        st_lex.push(Elem(c_type, c_val));

        gl();    
    }
    else 
        throw make_tuple(curr_lex, scan.get_line_number(), string("D"));
    D1(lex_type, lex);
}

void Parser::D1(std::pair<Type_of_lex, std::string> & lex_type, std::pair<Type_of_lex, std::string> & lex) {
    if (c_type == LEX_LEFT_BRACE) {
        prog.new_popoliz_of_func();
        last_ind++;
        prog.put_lex(lex_type, last_ind);
        prog.put_lex(lex, last_ind);
        gl();

        create_TID();

        params.clear();
        prog.put_lex({LEX_PARAMS, "LEX_PARAMS"}, last_ind);
        PARAMSEQ();
        prog.put_lex({LEX_START_FUNC, "START_FUNC"}, last_ind);
        if (c_type == LEX_RIGHT_BRACE) {
            new_func();
            st_lex.pop(); // remove type lexem

            gl();
            S();
            prog.put_lex({LEX_END_FUNC, "END_FUNC"}, last_ind);
            inside_function = -1;
            del_TID();
        }
        else 
            throw make_tuple(curr_lex, scan.get_line_number(), string("D1_1"));
    }
    else {
        push_id();
        prog.put_lex(lex_type, 0);
        prog.put_lex(lex, 0);
        while (c_type == LEX_COMMA) {
            gl();
            if (c_type == LEX_IDENT) {
                st_lex.push(Elem(c_type, c_val)); 
                prog.put_lex(curr_lex, 0);
                push_id();

                gl();
            } 
            else 
                throw make_tuple(curr_lex, scan.get_line_number(), string("D1_2"));
        }
        if (c_type == LEX_SEMICOLON) {
            st_lex.pop(); // remove type lexem

            gl();
        }
        else 
            throw make_tuple(curr_lex, scan.get_line_number(), string("D1_3"));
    }
}

void Parser::TYPE() {
    if (c_type == LEX_NOTHING ||
        c_type == LEX_INT ||
        c_type == LEX_BOOL ||
        c_type == LEX_STRING ||
        c_type == LEX_DOUBLE) {
        st_lex.push(Elem(c_type, c_val));

        gl();
    }
    else 
        throw make_tuple(curr_lex, scan.get_line_number(), string("TYPE"));
}

void Parser::PARAMSEQ() {
    if (c_type == LEX_NOTHING ||
        c_type == LEX_INT ||
        c_type == LEX_BOOL ||
        c_type == LEX_STRING ||
        c_type == LEX_DOUBLE) {
        params.push_back(c_type);
        st_lex.push(Elem(c_type, c_val));
        prog.put_lex(curr_lex, last_ind);

        gl();
        if (c_type == LEX_IDENT) {
            st_lex.push(Elem(c_type, c_val));
            prog.put_lex(curr_lex, last_ind);
            push_id();
            st_lex.pop();

            gl();  
            while (c_type == LEX_COMMA) {
                gl();
                if (c_type == LEX_NOTHING ||
                    c_type == LEX_INT ||
                    c_type == LEX_BOOL ||
                    c_type == LEX_STRING ||
                    c_type == LEX_DOUBLE) {
                    params.push_back(c_type);
                    st_lex.push(Elem(c_type, c_val));
                    prog.put_lex(curr_lex, last_ind);

                    gl();
                    if (c_type == LEX_IDENT) {
                        st_lex.push(Elem(c_type, c_val));
                        prog.put_lex(curr_lex, last_ind);
                        push_id();
                        st_lex.pop();

                        gl();
                    }
                    else 
                        throw make_tuple(curr_lex, scan.get_line_number(), string("PARAMSEQ"));
                }
                else 
                    throw make_tuple(curr_lex, scan.get_line_number(), string("PARAMSEQ"));
            }
        }
        else 
            throw make_tuple(curr_lex, scan.get_line_number(), string("PARAMSEQ"));
    }
}


void Parser::IDENTSEQ() {
    if (c_type == LEX_IDENT) {
        st_lex.push(Elem(c_type, c_val));
        prog.put_lex(curr_lex, last_ind);
        push_id();

        gl();
        while (c_type == LEX_COMMA) {
            gl();
            if (c_type == LEX_IDENT) {
                prog.put_lex(curr_lex, last_ind);
                st_lex.push(Elem(c_type, c_val));
                push_id();

                gl();
            } 
            else 
                throw make_tuple(curr_lex, scan.get_line_number(), string("IDENTSEQ"));
        }
    }
    else 
        throw make_tuple(curr_lex, scan.get_line_number(), string("IDENTSEQ"));
}


void Parser::S() {
    if (c_type == LEX_LEFT_CURLY) {
        gl();
        BLOCK();
    }
    else if (c_type == LEX_IF) {
        gl();
        IF();
    }
    else if (c_type == LEX_WITCH) {
        gl();
        WITCH();
    }
    else if (c_type == LEX_WHILE) {
        gl();
        inside_cycles += 1;
        int ind1, ind2;

        ES(ind1, ind2);
        prog.replase_lex({LEX_INT, to_string(prog.get_size(last_ind))}, last_ind, ind1);
        prog.replase_lex({LEX_INT, to_string(ind2 + 2)}, last_ind, ind2);

        inside_cycles -= 1;
    }
    else if (c_type == LEX_FOR) {
        gl();
        FOR();
    }
    else if (c_type == LEX_DO) {
        gl();
        DO();
    }
    else if (c_type == LEX_BREAK) {
        st_lex.push(Elem(c_type, c_val));
        check_jump();

        prog.put_lex(curr_lex, last_ind);
        gl();
        DELIM();
    }
    else if (c_type == LEX_CONTINUE) {
        st_lex.push(Elem(c_type, c_val));
        check_jump();

        prog.put_lex(curr_lex, last_ind);
        gl();
        DELIM();
    }
    else if (c_type == LEX_RETURN) {
        st_lex.push(Elem(c_type, c_val));

        gl();
        RETURN();
    }
    else if (c_type == LEX_NOTHING ||
        c_type == LEX_INT ||
        c_type == LEX_BOOL ||
        c_type == LEX_STRING ||
        c_type == LEX_DOUBLE) {
        st_lex.push(Elem(c_type, c_val));

        prog.put_lex(curr_lex, last_ind);
        gl();
        IDENTSEQ();

        st_lex.pop();
        DELIM();
    }
    else if (c_type == LEX_SEMICOLON) {
        gl();
    }
    else {
        E();

        st_lex.pop();

        DELIM();
    }
}

void Parser::BLOCK() {
    create_TID();

    while (c_type != LEX_RIGHT_CURLY) {
        S();
    }

    del_TID();
    if (c_type == LEX_RIGHT_CURLY) {
        gl();
    }
    else 
        throw make_tuple(curr_lex, scan.get_line_number(), string("BLOCK"));
}

void Parser::IF() {
    int ind1, ind2;
    std::vector<int> vec_ind_to_end;
    ES(ind1, ind2);
    vec_ind_to_end.emplace_back(ind2);

    while (c_type == LEX_ELF) {
        prog.replase_lex({LEX_INT, to_string(prog.get_size(last_ind))}, last_ind, ind1);
        gl();
        ES(ind1, ind2);
        vec_ind_to_end.emplace_back(ind2);
    }
    prog.replase_lex({LEX_INT, to_string(prog.get_size(last_ind))}, last_ind, ind1);
    if (c_type == LEX_ELSE) {
        gl();
        create_TID();

        S();

        del_TID();
    }
    for(int i = 0; i < vec_ind_to_end.size(); i++){
        prog.replase_lex({LEX_INT, to_string(prog.get_size(last_ind))}, last_ind, vec_ind_to_end[i]);
    }
}

void Parser::DELIM() {
    if (c_type == LEX_SEMICOLON) {
        prog.put_lex({LEX_SEMICOLON, ";"}, last_ind);
        gl();
    }
    else
        throw make_tuple(curr_lex, scan.get_line_number(), string("DELIM"));
}

void Parser::ES (int & ind1, int & ind2) {
    if (c_type == LEX_LEFT_BRACE) {
        gl();
        E();

        prog.put_lex({BLANK, "BLANK"}, last_ind);
        ind1 = prog.get_size(last_ind) - 1;
        prog.put_lex({LEX_FGO, "LEX_FGO"}, last_ind);
        check_condition();

        if (c_type == LEX_RIGHT_BRACE) {
            gl();
            create_TID();

            S();

            prog.put_lex({BLANK, "BLANK"}, last_ind);
            ind2 = prog.get_size(last_ind) - 1;
            prog.put_lex({LEX_GO, "LEX_GO"}, last_ind);
            del_TID();
        }
        else 
            throw make_tuple(curr_lex, scan.get_line_number(), string("ES_1"));
    }
    else 
        throw make_tuple(curr_lex, scan.get_line_number(), string("ES_2"));
}

void Parser::WITCH() {
    int ind1, ind2;
    std::vector<int> vec_ind_to_end;
    if (c_type == LEX_LEFT_CURLY) {
        gl();
        while (c_type != LEX_RIGHT_CURLY) {
            E();

            prog.put_lex({BLANK, "BLANK"}, last_ind);
            ind1 = prog.get_size(last_ind) - 1;
            prog.put_lex({LEX_FGO, "LEX_FGO"}, last_ind);
            check_condition();

            if (c_type == LEX_COLON) {
                gl();
                create_TID();

                S();

                prog.put_lex({BLANK, "BLANK"}, last_ind);
                ind2 = prog.get_size(last_ind) - 1;
                prog.put_lex({LEX_GO, "LEX_GO"}, last_ind);
                vec_ind_to_end.emplace_back(ind2);
                del_TID();
            }
            else 
                throw make_tuple(curr_lex, scan.get_line_number(), string("WITCH"));
            prog.replase_lex({LEX_INT, to_string(prog.get_size(last_ind))}, last_ind, ind1);
        }
        gl();
    }
    else 
        throw make_tuple(curr_lex, scan.get_line_number(), string("WITCH"));
    for(int i = 0; i < vec_ind_to_end.size(); i++){
        prog.replase_lex({LEX_INT, to_string(prog.get_size(last_ind))}, last_ind, vec_ind_to_end[i]);
    }
}

void Parser::FOR() {
    int ind_to_check_for, ind1, ind2, ind3;
    if (c_type == LEX_LEFT_BRACE) {
        create_TID();

        gl();
        if (c_type == LEX_SEMICOLON) {
            gl();
        }
        else {
            E();
            
            st_lex.pop();

            if (c_type == LEX_SEMICOLON) {
                gl();
            }
            else 
                throw make_tuple(curr_lex, scan.get_line_number(), string("FOR"));
        }
        ind_to_check_for = prog.get_size(last_ind);
        E();

        prog.put_lex({BLANK, "BLANK"}, last_ind);
        ind1 = prog.get_size(last_ind) - 1;
        prog.put_lex({LEX_FGO, "LEX_FGO"}, last_ind);
        prog.put_lex({BLANK, "BLANK"}, last_ind);
        ind2 = prog.get_size(last_ind) - 1;
        prog.put_lex({LEX_GO, "LEX_GO"}, last_ind);
        check_condition();

        if (c_type == LEX_SEMICOLON) {
            gl();
        }
        else
            throw make_tuple(curr_lex, scan.get_line_number(), string("FOR"));
        if (c_type == LEX_RIGHT_BRACE) {
            gl();
        }
        else {
            ind3 = prog.get_size(last_ind);
            E();

            prog.put_lex({LEX_INT, to_string(ind_to_check_for)}, last_ind);
            prog.put_lex({LEX_GO, "LEX_GO"}, last_ind);
            st_lex.pop();

            if (c_type == LEX_RIGHT_BRACE) {
                gl();
            }
            else
                throw make_tuple(curr_lex, scan.get_line_number(), string("FOR"));
        }

        inside_cycles += 1;

        prog.replase_lex({LEX_INT, to_string(prog.get_size(last_ind))}, last_ind, ind2);
        S();

        prog.put_lex({LEX_INT, to_string(ind3)}, last_ind);
        prog.put_lex({LEX_GO, "LEX_GO"}, last_ind);
        inside_cycles -= 1;
        del_TID();
    }
    else 
        throw make_tuple(curr_lex, scan.get_line_number(), string("FOR"));
    prog.replase_lex({LEX_INT, to_string(prog.get_size(last_ind))}, last_ind, ind1);
}

void Parser::DO() {
    int ind1;
    create_TID();
    inside_cycles += 1;

    ind1 = prog.get_size(last_ind);
    S();

    inside_cycles -= 1;
    del_TID();
    if (c_type == LEX_WHILE) {
        gl();
        if (c_type == LEX_LEFT_BRACE) {
            gl();
            E();

            prog.put_lex({LEX_INT, to_string(ind1)}, last_ind);
            prog.put_lex({LEX_TGO, "LEX_TGO"} , last_ind);
            check_condition();

            if (c_type == LEX_RIGHT_BRACE) {
                gl();
                DELIM();
            }
            else 
                throw make_tuple(curr_lex, scan.get_line_number(), string("DO"));
        }
        else 
            throw make_tuple(curr_lex, scan.get_line_number(), string("DO"));
    }
    else 
        throw make_tuple(curr_lex, scan.get_line_number(), string("DO"));
}

void Parser::RETURN() {
    if (c_type == LEX_SEMICOLON) {
        gl();

        prog.put_lex({LEX_NOTHING, "LEX_NOTHING"}, last_ind);
        prog.put_lex({LEX_RETURN, "return"}, last_ind);
        check_return();
    }
    else {
        E();

        prog.put_lex({LEX_RETURN, "return"}, last_ind);
        check_return();

        if (c_type == LEX_SEMICOLON) {
            gl();
        }
        else 
            throw make_tuple(curr_lex, scan.get_line_number(), string("RETURN"));
    } 
}

void Parser::E() {
    E1();
}

void Parser::ESEQ(int & col_param) {
    E2();
    col_param++;
    while (c_type == LEX_COMMA) {
        prog.put_lex({LEX_COMMA, ","}, last_ind);
        gl();
        E2();
        col_param++;
    }
}

void Parser::E1() {
    E2();
    while (c_type == LEX_COMMA) {
        st_lex.push(Elem(c_type, c_val));

        auto q = curr_lex;
        gl();
        E2();

        prog.put_lex(q, last_ind);
        check_bin();
    }
}

void Parser::E2() {
    E3();
    while (c_type == LEX_EQUAL ||
            c_type == LEX_PLUS_EQUAL ||
            c_type == LEX_MINUS_EQUAL ||
            c_type == LEX_MUL_EQUAL ||
            c_type == LEX_DIV_EQUAL ||
            c_type == LEX_MOD_EQUAL ||
            c_type == LEX_AND_EQUAL ||
            c_type == LEX_OR_EQUAL ||
            c_type == LEX_LEFT_SHIFT_EQUAL ||
            c_type == LEX_RIGHT_SHIFT_EQUAL) {
                
        st_lex.push(Elem(c_type, c_val));

        auto q = curr_lex;
        gl();
        E3();

        prog.put_lex(q, last_ind);
        check_bin();
    }
}

void Parser::E3() {
    E4();
    while (c_type == LEX_OR) {
        st_lex.push(Elem(c_type, c_val));

        gl();
        E4();

        prog.put_lex({LEX_OR, "||"}, last_ind);
        check_bin();
    }
}

void Parser::E4() {
    E5();
    while (c_type == LEX_AND) {
        st_lex.push(Elem(c_type, c_val));

        gl();
        E5();

        prog.put_lex({LEX_AND, "&&"}, last_ind);
        check_bin();
    }
}

void Parser::E5() {
    E6();
    while (c_type == LEX_BITWISE_OR) {
        st_lex.push(Elem(c_type, c_val));

        gl();
        E6();
        
        prog.put_lex({LEX_BITWISE_OR, "|"}, last_ind);
        check_bin();
    }
}

void Parser::E6() {
    E7();
    while (c_type == LEX_BITWISE_XOR) {
        st_lex.push(Elem(c_type, c_val));

        gl();
        E7();

        prog.put_lex({LEX_BITWISE_XOR, "^"}, last_ind);
        check_bin();
    }
}

void Parser::E7() {
    E8();
    while (c_type == LEX_BITWISE_AND) {
        st_lex.push(Elem(c_type, c_val));

        gl();
        E8();

        prog.put_lex({LEX_BITWISE_AND, "&"}, last_ind);
        check_bin();
    }
}

void Parser::E8() {
    E9();
    while (c_type == LEX_EQUALS ||
            c_type == LEX_NOT_EQUALS) {
        st_lex.push(Elem(c_type, c_val));
        auto q = curr_lex;

        gl();
        E9();
        prog.put_lex(q, last_ind);

        check_bin();
    }
}

void Parser::E9() {
    E10();
    while (c_type == LEX_LESS ||
            c_type == LEX_GREATER ||
            c_type == LEX_LESS_OR_EQUALS ||
            c_type == LEX_GREATER_OR_EQUALS) {
        st_lex.push(Elem(c_type, c_val));
        auto q = curr_lex;

        gl();
        E10();
        prog.put_lex(q, last_ind);

        check_bin();
    }
}

void Parser::E10() {
    E11();
    while (c_type == LEX_LEFT_SHIFT ||
            c_type == LEX_RIGHT_SHIFT) {
        st_lex.push(Elem(c_type, c_val));
        auto q = curr_lex;

        gl();
        E11();
        prog.put_lex(q, last_ind);

        check_bin();
    }
}

void Parser::E11() {
    E12();
    while (c_type == LEX_PLUS ||
            c_type == LEX_MINUS) {
        st_lex.push(Elem(c_type, c_val));
        auto q = curr_lex;

        gl();
        E12();
        prog.put_lex(q, last_ind);

        check_bin();
    }
}

void Parser::E12() {
    E13();
    while (c_type == LEX_MUL ||
            c_type == LEX_DIV ||
            c_type == LEX_MOD) {
        st_lex.push(Elem(c_type, c_val));
        auto q = curr_lex;

        gl();
        E13();
        prog.put_lex(q, last_ind);

        check_bin();
    }
}

void Parser::E13() {
    int cnt = 0;
    std::vector<std::pair<Type_of_lex, std::string>> lexs;
    while (c_type == LEX_INCR ||
            c_type == LEX_DECR ||
            c_type == LEX_PLUS ||
            c_type == LEX_MINUS ||
            c_type == LEX_NOT ||
            c_type == LEX_BITWISE_NOT ||
            c_type == LEX_SIZEOF) {
        st_lex.push(Elem(c_type, c_val));
        cnt += 1;

        lexs.emplace_back(curr_lex);
        gl();
    }
    E14();

    for(auto & u : lexs) {
        prog.put_lex(u, last_ind);
    }
    while (cnt --> 0)
        check_uno();
}

void Parser::E14() {
    E15();
    while (true) {
        if (c_type == LEX_LEFT_SQUARE_BRACE) {
            st_lex.push(Elem(c_type, c_val));

            gl();
            E();
            prog.put_lex({LEX_GETINDEX, "LEX_GETINDEX"}, last_ind);
            if (c_type == LEX_RIGHT_SQUARE_BRACE) {
                st_lex.push(Elem(c_type, c_val));
                check_braces();

                gl();
            }
            else 
                throw make_tuple(curr_lex, scan.get_line_number(), string("E14"));
        }
        else if (c_type == LEX_LEFT_BRACE) {
            st_lex.push(Elem(c_type, c_val));
            int col = 1;

            gl();
            if (c_type == LEX_RIGHT_BRACE) {
                st_lex.push(Elem(c_type, c_val));
                check_braces();

                gl();
            }
            else {
                ESEQ(col);

                if (c_type == LEX_RIGHT_BRACE) {
                    st_lex.push(Elem(c_type, c_val));
                    check_braces();

                    gl();
                }
                else 
                    throw make_tuple(curr_lex, scan.get_line_number(), string("E14"));
            }
            prog.put_lex({LEX_INT, to_string(col)}, last_ind);
            prog.put_lex({LEX_CALL, "LEX_CALL"}, last_ind);
        }
        else if (c_type == LEX_INCR) {
            st_lex.push(Elem(c_type, c_val));
            prog.put_lex(curr_lex, last_ind);
            check_postfix();

            gl();
        }
        else if (c_type == LEX_DECR) {
            st_lex.push(Elem(c_type, c_val));
            prog.put_lex(curr_lex, last_ind);
            check_postfix();

            gl();
        }
        else 
            break;
    }
}

void Parser::E15() {
    if (c_type == LEX_LEFT_BRACE) {
        gl();
        E();
        if (c_type == LEX_RIGHT_BRACE) {
            gl();
        }
        else 
            throw make_tuple(curr_lex, scan.get_line_number(), string("E15"));
    }
    else if (c_type == LEX_IDENT) {
        prog.put_lex(curr_lex, last_ind);
        check_id();
        gl();
    }
    else if (c_type == LEX_COMEIN) {
        st_lex.push(Elem(c_type, c_val));
        prog.put_lex(curr_lex, last_ind);
        gl();
    }
    else if (c_type == LEX_GOOUT) {
        st_lex.push(Elem(c_type, c_val));
        prog.put_lex(curr_lex, last_ind);
        gl();   
    }
    else if (c_type == LEX_STRING_LITERAL) {
        st_lex.push(Elem(LEX_STRING, "str literal"));
        prog.put_lex(curr_lex, last_ind);
        gl();
    }
    else if (c_type == LEX_INT_LITERAL) {
        st_lex.push(Elem(LEX_INT, "int literal"));
        prog.put_lex(curr_lex, last_ind);
        gl();
    }
    else if (c_type == LEX_BOOLEAN_LITERAL) {
        st_lex.push(Elem(LEX_BOOL, "bool literal"));
        prog.put_lex(curr_lex, last_ind);
        gl();
    }
    else if (c_type == LEX_DOUBLE_LITERAL) {
        st_lex.push(Elem(LEX_DOUBLE, "double literal"));
        prog.put_lex(curr_lex, last_ind);
        gl();
    }
    else 
        throw make_tuple(curr_lex, scan.get_line_number(), string("E15"));
}

