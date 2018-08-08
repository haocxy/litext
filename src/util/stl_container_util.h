#pragma once

#include <vector>
#include <map>
#include <deque>
#include <stdint.h>

namespace StlContainerUtil
{
    template <typename Container>
    auto grow(Container &c) -> decltype(c.back())
    {
        c.resize(c.size() + 1);
        return c.back();
    }

    template <typename E>
    bool contains(const std::vector<E> &c, const E &e)
    {
        for (const auto &x : c)
        {
            if (x == e)
            {
                return true;
            }
        }
        return false;
    }


    template <typename Container>
    void pop_front(Container &c, size_t cnt)
    {
        const Int size = static_cast<Int>(c.size());
        if (cnt < size)
        {
            for (Int i = 0; i != cnt; ++i)
            {
                c.pop_front();
            }
        }
        else
        {
            c.clear();
        }
    }

    template <typename E>
    void pop_back(std::vector<E> & vec, size_t cnt)
    {
        const int size = static_cast<int>(c.size());
        if (cnt < size)
        {
            vec.resize(size - cnt);
        }
        else
        {
            vec.clear();
        }
    }

    template <typename E>
    void pop_back(std::deque<E> & vec, size_t cnt)
    {
        const int size = static_cast<int>(c.size());
        if (cnt < size)
        {
            vec.resize(size - cnt);
        }
        else
        {
            vec.clear();
        }
    }
}
