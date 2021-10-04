#pragma once

#include <vector>

#include "core/signal.h"
#include "doc/open_info.h"


namespace gui
{

class SingletonServer {
public:

    using OpenInfos = std::vector<doc::OpenInfo>;

    struct StartInfo {
        fs::path serverRunningLock;
        fs::path infoFile;
        fs::path infoFileLock;
    };

    static SingletonServer *newObj();

    virtual ~SingletonServer() {}

    virtual Signal<void()> &sigActivateUI() = 0;

    virtual Signal<void(const OpenInfos &)> &sigRecvOpenInfos() = 0;

    virtual void start(const StartInfo &info) = 0;
};

}
