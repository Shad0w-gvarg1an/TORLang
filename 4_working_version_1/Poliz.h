
#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <chrono>
#include <random>
#include "Lexems.h"

struct Type {
	//* Standard expression type
	Type_of_lex expr_type;
	//* Whether the expression is constant (in case it is a variable)
	bool is_const;
	//* Whether the expression is an array
	bool is_array;
	//* The size of the array or -1 if the type is not an array
	int array_size;
	Type(Type_of_lex expr_type_, bool is_const_, bool is_array_, int array_size_ = -1) {
		expr_type = expr_type_;
		is_const = is_const_;
		is_array = is_array_;
		array_size = array_size_;
	}
	Type() {
		expr_type = Type_of_lex::UNKNOWN;
		is_const = is_array = false;
		array_size = -1;
	}
	bool operator==(const Type& other) const {
		return expr_type == other.expr_type && is_const == other.is_const && is_array == other.is_array && array_size == other.array_size;
	}
	bool operator!=(const Type& other) const {
		return !(*this == other);
	}
};

struct Identifier {
	//* Variable name
	std::string name;
	//* Standart expression type
	Type type;
	//* Pointer to a value stored in the executor memory
	void* value;
	Identifier(std::string name_, Type type_, void* value_ = nullptr) {
		name = name_;
		type = type_;
		value = value_;
	}
	bool operator==(const Identifier& other) const {
		return type == other.type;
	}
};

class Popoliz{
public:
    void put_lex(const std::pair<Type_of_lex, string> & lex, const int & ind);
    int get_size(const int & ind);
    void replase_lex(const std::pair<Type_of_lex, string> & lex, const int & ind_pol, const int & ind_el);
    void put_blank(const int & ind);
    void new_popoliz_of_func();
    void print_popoliz();
private:
    std::map<string, int> func_ind;
    std::vector<std::vector<std::pair<Type_of_lex, string>>> func_poliz;
    std::stack<std::vector<std::pair<Identifier, string>>> tid_vals;
    Type_of_lex string_to_type(const std::string &str);
};