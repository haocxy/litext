#include "singleton_server.h"

#ifdef WIN32
#include "singleton_server.win32.h"
#else
#include "singleton_server.unix.h"
#endif


namespace gui
{

SingletonServer *SingletonServer::newObj() {
#ifdef WIN32
    return new SingletonServerWin32;
#else
    return new SingletonServerUnix;
#endif
}

}
