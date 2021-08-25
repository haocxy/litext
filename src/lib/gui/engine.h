#pragma once

#include "propdb.h"


namespace gui
{

// 所有平台无关逻辑的根
// 所有平台无关的模块都直接或间接地由这个类的实例管理
class Engine {
public:
    Engine() {}

    const PropDb &propDb() const {
        return propDb_;
    }

    PropDb &propDb() {
        return propDb_;
    }

private:
    PropDb propDb_;
};

}
