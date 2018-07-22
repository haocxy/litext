#pragma once

#include <vector>
#include <map>

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

}
