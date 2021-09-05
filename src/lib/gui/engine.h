#pragma once

#include "config.h"
#include "propdb.h"
#include "singleton_server.h"


namespace gui
{

// 所有平台无关逻辑的根
// 所有平台无关的模块都直接或间接地由这个类的实例管理
class Engine {
public:
    Engine() {}

    const Config &config() const {
        return config_;
    }

    Config &config() {
        return config_;
    }

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
    Config config_;
    SingletonServer singletonServer_;
    PropDb propDb_;
};

}
