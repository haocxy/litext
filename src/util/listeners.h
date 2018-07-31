#pragma once

#include <vector>
#include <functional>
#include <cstdint>

typedef uintmax_t ListenerID;

template <typename Fn>
class Listeners
{
public:
    ListenerID add(std::function<Fn> && f)
    {
        m_vec.push_back(std::move(f));
        return reinterpret_cast<ListenerID>(&(m_vec.back()));
    }

    void remove(ListenerID id)
    {
        for (Vec::size_type i = 0; i < m_vec.size(); ++i)
        {
            if (reinterpret_cast<ListenerID>(&(m_vec[i])) == id)
            {
                m_vec[i].swap(m_vec.back());
                m_vec.pop_back();
                return;
            }
        }
    }

    template <typename ...Args>
    void call(const Args & ...args)
    {
        for (auto & f : m_vec)
        {
            f(args...);
        }
    }
private:
    typedef typename std::vector<std::function<Fn>> Vec;
    Vec m_vec;
};

