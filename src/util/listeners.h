#pragma once

#include <vector>
#include <functional>
#include <cstdint>
#include <memory>

class RemovableListeners
{
public:
    virtual ~RemovableListeners() {}
    virtual void remove(uintmax_t id) = 0;
};

// ¼àÌýÆ÷¾ä±ú
// ¾ä±úÎö¹¹Ê±»á×Ô¶¯ÒÆ³ý¼àÌýÆ÷
class ListenerHandle
{
public:
    ListenerHandle()
        : m_listeners(nullptr)
        , m_listenerId(0)
    {

    }

    ListenerHandle(ListenerHandle && listenerHandler)
        : m_listeners(listenerHandler.m_listeners)
        , m_listenerId(listenerHandler.m_listenerId)
    {
        listenerHandler.m_listeners = nullptr;
        listenerHandler.m_listenerId = 0;
    }

    ListenerHandle & operator=(ListenerHandle && h)
    {
        m_listeners = h.m_listeners;
        h.m_listeners = nullptr;

        m_listenerId = h.m_listenerId;
        h.m_listenerId = 0;

        return *this;
    }

    ~ListenerHandle()
    {
        if (m_listeners)
        {
            m_listeners->remove(m_listenerId);
            m_listeners = nullptr;
            m_listenerId = 0;
        }
    }

private:
    std::shared_ptr<RemovableListeners> m_listeners;
    uintmax_t m_listenerId;

    ListenerHandle(std::shared_ptr<RemovableListeners> listeners, uintmax_t id) : m_listeners(listeners), m_listenerId(id) {}

    template <typename Fn>
    friend class Listeners;
};

template <typename Fn>
class ListenersInner : public RemovableListeners
{
public:
    virtual ~ListenersInner() {}

    uintmax_t add(std::function<Fn> && f)
    {
        m_vec.push_back(std::move(f));
        uintmax_t id = uintmax_t(&(m_vec.back()));
        return id;
    }

    virtual void remove(uintmax_t id) override
    {
        for (Vec::size_type i = 0; i < m_vec.size(); ++i)
        {
            if (uintmax_t(&(m_vec[i])) == id)
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

template <typename Fn>
class Listeners
{
public:
    ListenerHandle add(std::function<Fn> && f)
    {
        uintmax_t id = m_ptr->add(std::move(f));
        return ListenerHandle(m_ptr, id);
    }

    template <typename ...Args>
    void call(const Args & ...args)
    {
        m_ptr->call(args...);
    }

private:
    std::shared_ptr<ListenersInner<Fn>> m_ptr{ new ListenersInner<Fn> };
};
