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
    void pipeServerLoop();

private:
    Signal<void()> sigActivateUI_;
    Signal<void(const OpenInfos &)> sigRecvOpenInfos_;
    std::thread pipeServerThread_;
};

}
