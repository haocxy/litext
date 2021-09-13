#pragma once

#include <atomic>
#include <thread>
#include <memory>

#include "core/thread.h"


namespace gui
{

class ObjAsyncDeleter {
private:

    class Deleter {
    public:
        virtual ~Deleter() {}

        virtual void doDelete() = 0;
    };

    template <typename T>
    class DeleterImpl : public Deleter {
    public:
        DeleterImpl(T *obj)
            : obj_(obj) {}

        virtual ~DeleterImpl() {
            if (obj_) {
                delete obj_;
                obj_ = nullptr;
            }
        }

        virtual void doDelete() override {
            if (obj_) {
                delete obj_;
                obj_ = nullptr;
            }
        }

    private:
        T *obj_ = nullptr;
    };

public:
    ObjAsyncDeleter();

    ~ObjAsyncDeleter();

    template <typename T>
    void asyncDelete(T *obj) {
        if (!destructing_) {
            if (obj) {
                deleterQueue_.push(std::make_unique<DeleterImpl<T>>(obj));
            }
        } else {
            // destructing_ 由析构函数置为 true, 显然在析构函数后就不应该继续使用了, 这是个BUG
            throw std::logic_error("ObjAsyncDeleter already destructed, logic has bug");
        }
    }

private:
    void loop();

private:

    // 由析构函数置为 true
    std::atomic_bool destructing_{ false };

    // TODO 应该多开几个线程,确保析构逻辑尽快结束
    // 因为在正常逻辑中析构逻辑的结束是必须的
    std::thread worker_;

    BlockQueue<std::unique_ptr<Deleter>> deleterQueue_;
};

}
