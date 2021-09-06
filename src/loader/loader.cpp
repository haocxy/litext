#include <iostream>
#include <fstream>
#include <string>

#include "core/logger.h"
#include "core/filelock.h"
#include "global/work_dir_manager.h"

#include "litext_delegate.h"
#include "cmdopt.h"


static const DirManager dirMgr;

static bool isServerRunning()
{
    FileLock serverRunningLock(dirMgr.singletonServerRunningLockFile());
    // 锁定成功表示没有服务在运行中
    if (serverRunningLock.tryLock()) {
        return false;
    } else {
        return true;
    }
}

static u16 readServerPort()
{
    ScopedFileLock serverInfoLock(dirMgr.singletonServerInfoFile());


}

static void notifyServer(const CmdOpt &opt)
{

}

static int load(int argc, char *argv[])
{
    CmdOpt opt(argc, argv);
    if (opt.needHelp()) {
        opt.help(std::cout);
        return 0;
    }

    ScopedFileLock loadLock(dirMgr.loadLogicLockFile());

    if (isServerRunning()) {
        notifyServer(opt);
        return 0;
    }

    LitextDelegate::Factory factory;
    LitextDelegate litext(factory);

    litext.initSetLogLevel(opt.logLevel());
    litext.initSetShouldStartAsServer(true);
    for (const auto &pair : opt.files()) {
        litext.initAddOpenInfo(pair.first, pair.second);
    }

    // 实际执行Application的初始化逻辑,
    // 其它的以init开头的函数只是设置了初始化信息,
    // 只有init()才是把这些信息实际用于Application初始化
    litext.init();

    // init之后解锁,其它loader可以执行load逻辑
    // ScopedFileLock会在退出作用域时自动解锁,
    // 但此处不会退出作用域,所以一定要调用unlock
    loadLock.unlock();

    // 进入消息循环
    return litext.exec();
}

int loader_entry(int argc, char *argv[])
{
    try {
        return load(argc, argv);
    } catch (const std::exception &e) {
        LOGE << "load() throws exception, what(): " << e.what();
        return 1;
    } catch (...) {
        LOGE << "load() throws unknown exception";
        return 1;
    }
}
