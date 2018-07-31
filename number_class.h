#pragma once

template <typename T>
class NumberClass
{
public:
    NumberClass() : m_val(T()) {}
    NumberClass(T val) : m_val(val) {}
private:
    T m_val;
};
