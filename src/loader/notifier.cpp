#include "notifier.h"

#ifdef WIN32
#include "notifier.win32.h"
#else
#include "notifier.unix.h"
#endif


bool notifyServer(const CmdOpt &opt)
{
#ifdef WIN32
    return notifyServerForWin32(opt);
#else
    return notifyServerForUnix(opt);
#endif
}
