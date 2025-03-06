#ifndef STACK_H
#define STACK_H

#include <vector>
#include <stdexcept>

template<typename T>
class Stack {
public:
    const T& top() {
        if (st.empty()) {
            throw std::logic_error("Top on empty stack");
        }
        return st.back();
    }
    void pop() {
        if (st.empty()) {
            throw std::logic_error("Pop on empty stack");
        }
        st.pop_back();
    }
    void push(const T& elem) {
        st.push_back(elem);
    }
private:
    std::vector<T> st;
};

#endif
