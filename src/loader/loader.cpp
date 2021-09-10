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

    ScopedFileLock serverInfoLock(dirMgr.singletonServerInfoFileLock());

    const fs::path serverInfoFilePath = dirMgr.singletonServerInfoFile();
    if (!fs::exists(serverInfoFilePath)) {
        LOGE << title << "server info file not exists";
        return 0;
    }

    std::ifstream ifs(serverInfoFilePath);
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

    LOGD << title << "server port [" << serverInfo.port() << "]";
    return serverInfo.port();
}

static void send(boost::asio::ip::tcp::socket &sock, const std::string &data)
{
    const i64 sendDataLen = data.size();
    const i64 sendDataLenNetEndian = endian::nativeToNet(sendDataLen);
    sock.send(boost::asio::const_buffer(&sendDataLenNetEndian, sizeof(sendDataLenNetEndian)));
    sock.send(boost::asio::const_buffer(data.data(), sendDataLen));
}

static void send(boost::asio::ip::tcp::socket &sock, const msg::Pack &pack)
{
    send(sock, pack.serialize());
}

static msg::Pack recv(boost::asio::ip::tcp::socket &sock)
{
    i64 recvDataLenNetEndian = 0;
    sock.receive(boost::asio::mutable_buffer(&recvDataLenNetEndian, sizeof(recvDataLenNetEndian)));
    std::string recvData;
    recvData.resize(endian::netToNative(recvDataLenNetEndian));
    sock.receive(boost::asio::mutable_buffer(recvData.data(), recvData.size()));
    return msg::Pack::deserialize(recvData);
}

static bool notifyServer(const CmdOpt &opt)
{
    const u16 port = readServerPort();
    if (port == 0) {
        return false;
    }

    namespace asio = boost::asio;
    namespace ip = asio::ip;
    using tcp = ip::tcp;

    asio::io_context context;
    tcp::resolver resolver(context);
    tcp::socket sock(context);
    sock.connect(tcp::endpoint(ip::address_v4::from_string("127.0.0.1"), port));

    LOGD << "client conneted to server, server port [" << sock.remote_endpoint().port() << "] client port [" << sock.local_endpoint().port() << "]";

    if (opt.files().empty()) {
        // 如果没有要打开的文件,则通知服务端显示界面
        send(sock, msg::ActivateUI());
    } else {
        msg::OpenFiles openFilesMsg;
        for (const auto &[file, row] : opt.files()) {
            openFilesMsg.files.push_back({u8str(file.generic_u8string()), row});
        }
        send(sock, openFilesMsg);
    }

    msg::Pack result = recv(sock);
    if (result.is<msg::Ok>()) {
        return true;
    } else {
        return false;
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
