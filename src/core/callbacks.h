#pragma once

#include <list>
#include <functional>
#include <memory>

class RemovableCallbacks
{
public:
    virtual ~RemovableCallbacks() {}
    virtual void remove(void * id) = 0;
};

// 监听器句柄
// 由监听事件的模块(通常是添加监听器的模块)负责维护生命周期
// 句柄析构时会自动移除监听器,以保证当监听者消失时监听器被正确移除
class CallbackHandle
{
public:
    CallbackHandle()
        : m_callbacks(nullptr)
        , m_callbackId(nullptr)
    {

    }

    CallbackHandle(CallbackHandle && cbh)
        : m_callbacks(cbh.m_callbacks)
        , m_callbackId(cbh.m_callbackId)
    {
        cbh.m_callbacks = nullptr;
        cbh.m_callbackId = nullptr;
    }

    CallbackHandle & operator=(CallbackHandle && cbh)
    {
        m_callbacks = cbh.m_callbacks;
        cbh.m_callbacks = nullptr;

        m_callbackId = cbh.m_callbackId;
        cbh.m_callbackId = nullptr;

        return *this;
    }

    ~CallbackHandle()
    {
        if (m_callbacks && m_callbackId)
        {
            m_callbacks->remove(m_callbackId);
            m_callbacks = nullptr;
            m_callbackId = nullptr;
        }
    }

private:
    std::shared_ptr<RemovableCallbacks> m_callbacks;
    void *m_callbackId;

    CallbackHandle(std::shared_ptr<RemovableCallbacks> callbacks, void * id) : m_callbacks(callbacks), m_callbackId(id) {}

    template <typename Fn>
    friend class Callbacks;
};

template <typename Fn>
class CallbacksInner : public RemovableCallbacks
{
public:
    virtual ~CallbacksInner() {}

    void *add(std::function<Fn> && f)
    {
        callbacks_.push_back(std::move(f));
        return &(callbacks_.back());
    }

    virtual void remove(void *id) override
    {
        for (auto it = callbacks_.begin(); it != callbacks_.end(); ++it)
        {
            if ((&(*it)) == id)
            {
                callbacks_.erase(it);
                return;
            }
        }
    }

    template <typename ...Args>
    void call(const Args & ...args)
    {
        for (auto &f : callbacks_)
        {
            f(args...);
        }
    }
private:
    std::list<std::function<Fn>> callbacks_;
};

template <typename Fn>
class Callbacks
{
public:
    CallbackHandle add(std::function<Fn> && f)
    {
        void * id = m_ptr->add(std::move(f));
        return CallbackHandle(m_ptr, id);
    }

    template <typename ...Args>
    void call(const Args & ...args)
    {
        m_ptr->call(args...);
    }

private:
    std::shared_ptr<CallbacksInner<Fn>> m_ptr{ new CallbacksInner<Fn> };
};
