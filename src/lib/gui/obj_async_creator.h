#pragma once

#include <atomic>
#include <thread>
#include <tuple>
#include <memory>
#include <functional>

#include "core/thread.h"


namespace gui
{

// 异步地构造对象
//
// 某些对象的构造逻辑本身就是复杂的,可能涉及到一些诸如打开数据库之类的慢速操作
// 这些构造逻辑不能放在GUI线程中,否则当这类对象被大量构造时会使GUI线程卡顿
// 例如本项目中的Editor对象就是这种构造较慢的对象
//
// 构造线程不应该太多,只需要保证异步化即可
// 相反地,用于释放对象的线程则应该多一些以保证对象的释放被尽快地执行
class ObjAsyncCreator {
private:

    class Creatable {
    public:
        virtual ~Creatable() {}

        virtual void create() = 0;
    };

    template <typename T, typename ...Args>
    class CreatableImpl : public Creatable {
    public:
        CreatableImpl(const Args &... args, std::function<void(T*)> &&cb)
            : args_(std::make_tuple(args...))
            , callback_(std::move(cb)) {}

        virtual ~CreatableImpl() {}

        virtual void create() {
            T *obj = std::apply(doCreate, std::move(args_));
            callback_(obj);
        }

    private:
        static T *doCreate(Args &&...args) {
            return new T(std::forward<Args>(args)...);
        }

    private:
        std::tuple<Args...> args_;
        std::function<void(T *)> callback_;
    };

public:
    ObjAsyncCreator();

    ~ObjAsyncCreator();

    // 异步地构造一个类型为T地对象
    // 构造参数列表地类型为Args
    // 构造完成后会调用回调函数cb
    template <typename T, typename ...Args>
    void asyncCreate(const Args &...args, std::function<void(T *)> &&cb) {

        std::unique_ptr<Creatable> creatable = std::make_unique<CreatableImpl<T, Args...>>(
            args...,
            std::move(cb));

        creatables_.push(std::move(creatable));
    }

private:
    void loop();

private:
    std::atomic_bool stopping_{ false };
    BlockQueue<std::unique_ptr<Creatable>> creatables_;
    std::thread thread_;
};

}
