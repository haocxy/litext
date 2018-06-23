#pragma once

template <typename T>
class PtrDeleter
{
public:
    typedef T *Ptr;
    PtrDeleter(Ptr & ptr) : m_ptr(ptr) {}
    ~PtrDeleter() { delete m_ptr; m_ptr = nullptr; }
private:
    Ptr & m_ptr;
};


