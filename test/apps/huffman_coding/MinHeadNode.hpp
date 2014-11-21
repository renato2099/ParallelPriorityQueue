#include <iostream>
#include <sstream>
#include <functional>
#include <vector>
#include <queue>

class Int {
public:
    Int(int val) : m_val(val) {}
    int getVal() { return m_val; }
    bool operator<(const Int &other) const { return m_val < other.m_val; }
private:
    int m_val;
};

template<typename Type, typename Compare = std::less<Type> >
struct pless : public std::binary_function<Type *, Type *, bool> {
    bool operator()(const Type *x, const Type *y) const
        { return Compare()(*x, *y); }
};