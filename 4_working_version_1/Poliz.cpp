#include "Poliz.h"

void Popoliz::put_lex(const std::pair<Type_of_lex, string> & lex, const int &ind){
    func_poliz[ind].emplace_back(lex);
}

int Popoliz::get_size(const int & ind){
    return func_poliz[ind].size();
}

void Popoliz::replase_lex(const std::pair<Type_of_lex, string> & lex, const int &ind_pol, const int & ind_el){
    func_poliz[ind_pol][ind_el] = lex;
}

void Popoliz::put_blank(const int & ind) {
    func_poliz[ind].emplace_back(Type_of_lex::BLANK, "BLANK");
}

void Popoliz::new_popoliz_of_func(){
    func_poliz.emplace_back(vector<std::pair<Type_of_lex, string>>());
}

void Popoliz::print_popoliz(){
    for(int i = 0; i < func_poliz.size(); i++){
        for(int j = 0; j < func_poliz[i].size(); j++){
            cout << "{ " /*<< func_poliz[i][j].first << " "*/ << func_poliz[i][j].second << " } ";
        }
        cout << ";" << endl;
    }
}
