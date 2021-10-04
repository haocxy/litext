#include "singleton_server.win32.h"


namespace gui
{



SingletonServerWin32::SingletonServerWin32()
{
}

SingletonServerWin32::~SingletonServerWin32()
{
}

Signal<void()> &SingletonServerWin32::sigActivateUI()
{
    return sigActivateUI_;
}

Signal<void(const SingletonServerWin32::OpenInfos &)> &SingletonServerWin32::sigRecvOpenInfos()
{
    return sigRecvOpenInfos_;
}

void SingletonServerWin32::start()
{
}

}
