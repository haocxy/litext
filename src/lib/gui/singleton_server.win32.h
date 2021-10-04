#pragma once

#include <thread>

#include "core/fs.h"
#include "core/filelock.h"

#include "singleton_server.h"


namespace gui
{

class SingletonServerWin32 : public SingletonServer {
public:

    SingletonServerWin32();

    virtual ~SingletonServerWin32();

    virtual Signal<void()> &sigActivateUI() override;

    virtual Signal<void(const OpenInfos &)> &sigRecvOpenInfos() override;

    virtual void start(const StartInfo &info) override;

private:
    void pipeServerLoop();

private:
    Signal<void()> sigActivateUI_;
    Signal<void(const OpenInfos &)> sigRecvOpenInfos_;
    std::thread pipeServerThread_;
    bool started_ = false;

    FileLock serverRunningLock_;
    FileLockGuard serverRunningLockGuard_;
};

}
