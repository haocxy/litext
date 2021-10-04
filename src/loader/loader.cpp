#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <optional>

#include <boost/asio.hpp>

#include "core/logger.h"
#include "core/filelock.h"
#include "core/system.h"
#include "core/endian.h"
#include "global/msg.h"
#include "global/constants.h"
#include "global/server_info.h"
#include "global/work_dir_manager.h"

#include "litext_delegate.h"
#include "cmdopt.h"
#include "notifier.h"


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

enum class StartInSingletonModeResult {
    StartAsServer,
    ServerRunningAndNotified,
    ServerRunningButCannotNotify,
    NonSingletonByCmdOpt,
};

static StartInSingletonModeResult tryStartInSingletonMode(const CmdOpt &opt)
{
    if (opt.hasNonSingle()) {
        return StartInSingletonModeResult::NonSingletonByCmdOpt;
    }

    if (isServerRunning()) {
        if (notifyServer(opt)) {
            return StartInSingletonModeResult::ServerRunningAndNotified;
        } else {
            return StartInSingletonModeResult::ServerRunningButCannotNotify;
        }
    } else {
        return StartInSingletonModeResult::StartAsServer;
    }
}

static int load(int argc, char *argv[])
{
    CmdOpt opt(argc, argv);
    if (opt.needHelp()) {
        opt.help(std::cout);
        return 0;
    }

    ScopedFileLock loadLock(dirMgr.loadLogicLockFile());

    const StartInSingletonModeResult tryResult = tryStartInSingletonMode(opt);
    if (tryResult == StartInSingletonModeResult::ServerRunningAndNotified) {
        return 0;
    }

    LitextDelegate::Factory factory;
    LitextDelegate litext(factory);

    if (tryResult == StartInSingletonModeResult::StartAsServer) {
        litext.initSetShouldStartAsServer(true);
    } else {
        litext.initSetShouldStartAsServer(false);
    }

    litext.initSetLogLevel(opt.logLevel());
    
    for (const auto &pair : opt.files()) {
        litext.initAddOpenInfo(pair.first, pair.second);
    }

    const std::optional<fs::path> styleSheetFile = opt.styleSheetFile();
    if (styleSheetFile) {
        litext.initSetStyleSheetFile(*styleSheetFile);
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
