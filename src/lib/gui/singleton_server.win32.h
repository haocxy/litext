#pragma once

#include <thread>

#include "singleton_server.h"


namespace gui
{

class SingletonServerWin32 : public SingletonServer {
public:
    SingletonServerWin32();

    virtual ~SingletonServerWin32();

    virtual Signal<void()> &sigActivateUI() override;

    virtual Signal<void(const OpenInfos &)> &sigRecvOpenInfos() override;

    void start();

private:
    Signal<void()> sigActivateUI_;
    Signal<void(const OpenInfos &)> sigRecvOpenInfos_;
};

}
