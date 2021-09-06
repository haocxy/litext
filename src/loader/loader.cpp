#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <optional>

#include "core/logger.h"
#include "core/filelock.h"
#include "core/system.h"
#include "global/constants.h"
#include "global/server_info.h"
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
    const char *title = "readServerPort() ";

    ScopedFileLock serverInfoLock(dirMgr.singletonServerInfoFile());

    const fs::path serverInfoFilePath = dirMgr.singletonServerInfoFile();
    if (!fs::exists(serverInfoFilePath)) {
        LOGE << title << "server info file not exists";
        return 0;
    }

    std::ifstream ifs(dirMgr.singletonServerInfoFile(), std::ios::binary);
    if (!ifs) {
        LOGE << title << "cannot open server info file";
        return 0;
    }

    std::vector<std::string> repeatedLines;

    std::string line;

    while (std::getline(ifs, line)) {
        if (!line.empty()) {
            repeatedLines.push_back(std::move(line));
        }
    }

    const i64 lineCount = repeatedLines.size();

    if (lineCount < 1 || lineCount != constants::ServerInfoRepeatCount) {
        LOGE << title << "bad lineCount";
        return 0;
    }

    for (i64 i = 1; i < lineCount; ++i) {
        if (repeatedLines[i] != repeatedLines[i - 1]) {
            LOGE << title << "lines not same";
            return 0;
        }
    }

    const ServerInfo serverInfo = ServerInfo::fromLine(repeatedLines.front());
    if (serverInfo.port() == 0) {
        LOGE << title << "bad port";
        return 0;
    }

    if (SystemUtil::exePath() != serverInfo.exePath()) {
        LOGE << title << "bad executable file path";
        return 0;
    }

    LOGD << title << "server port [" << serverInfo.port() << "]";
    return serverInfo.port();
}

static bool notifyServer(const CmdOpt &opt)
{
    const u16 port = readServerPort();
    if (port == 0) {
        return false;
    }

    // TODO
    return true;
}

enum class StartInSingletonModeResult {
    NoServerRunning,
    ServerRunningAndNotified,
    ServerRunningButCannotNotify,
};

static StartInSingletonModeResult tryStartInSingletonMode(const CmdOpt &opt)
{
    if (isServerRunning()) {
        if (notifyServer(opt)) {
            return StartInSingletonModeResult::ServerRunningAndNotified;
        } else {
            return StartInSingletonModeResult::ServerRunningButCannotNotify;
        }
    } else {
        return StartInSingletonModeResult::NoServerRunning;
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

    if (tryResult == StartInSingletonModeResult::NoServerRunning) {
        litext.initSetShouldStartAsServer(true);
    } else {
        litext.initSetShouldStartAsServer(false);
    }

    litext.initSetLogLevel(opt.logLevel());
    
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
