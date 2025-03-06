#ifndef STACK_H
#define STACK_H

#include <vector>
#include <stdexcept>

template<typename T>
class Stack {
public:
    T top() {
        if (st.empty()) {
            throw std::logic_error("Top on empty stack");
        }
        return st.back();
    }
    T pop() {
        if (st.empty()) {
            throw std::logic_error("Pop on empty stack");
        }
        auto tmp = top();
        st.pop_back();
        return tmp;
    }
    void push(const T& elem) {
        st.push_back(elem);
    }
private:
    std::vector<T> st;
};

#endif
