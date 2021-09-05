#pragma once

#include "singleton_server.h"
#include "propdb.h"


namespace gui
{

// 所有平台无关逻辑的根
// 所有平台无关的模块都直接或间接地由这个类的实例管理
class Engine {
public:
    Engine() {}

    const SingletonServer &singletonServer() const {
        return singletonServer_;
    }

    SingletonServer &singletonServer() {
        return singletonServer_;
    }

    const PropDb &propDb() const {
        return propDb_;
    }

    PropDb &propDb() {
        return propDb_;
    }

private:
    SingletonServer singletonServer_;
    PropDb propDb_;
};

}
