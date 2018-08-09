#pragma once

#include <vector>
#include <functional>
#include <memory>

class RemovableListeners
{
public:
    virtual ~RemovableListeners() {}
    virtual void remove(void * id) = 0;
};

// 监听器句柄
// 由监听事件的模块(通常是添加监听器的模块)负责维护生命周期
// 句柄析构时会自动移除监听器,以保证当监听者消失时监听器被正确移除
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
        listenerHandler.m_listenerId = nullptr;
    }

    ListenerHandle & operator=(ListenerHandle && h)
    {
        m_listeners = h.m_listeners;
        h.m_listeners = nullptr;

        m_listenerId = h.m_listenerId;
        h.m_listenerId = nullptr;

        return *this;
    }

    ~ListenerHandle()
    {
        if (m_listeners && m_listenerId)
        {
            m_listeners->remove(m_listenerId);
            m_listeners = nullptr;
            m_listenerId = nullptr;
        }
    }

private:
    std::shared_ptr<RemovableListeners> m_listeners;
    void * m_listenerId;

    ListenerHandle(std::shared_ptr<RemovableListeners> listeners, void * id) : m_listeners(listeners), m_listenerId(id) {}

    template <typename Fn>
    friend class Listeners;
};

template <typename Fn>
class ListenersInner : public RemovableListeners
{
public:
    virtual ~ListenersInner() {}

    void * add(std::function<Fn> && f)
    {
        m_vec.push_back(std::move(f));
        return &(m_vec.back());
    }

    virtual void remove(void * id) override
    {
        for (Vec::size_type i = 0; i < m_vec.size(); ++i)
        {
            if (&(m_vec[i]) == id)
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
        void * id = m_ptr->add(std::move(f));
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
