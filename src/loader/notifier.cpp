#include "notifier.h"

#include <boost/asio.hpp>

#include "core/endian.h"
#include "core/filelock.h"
#include "core/logger.h"

#include "global/work_dir_manager.h"
#include "global/server_info.h"
#include "global/constants.h"
#include "global/msg.h"


static const DirManager dirMgr;

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

bool notifyServer(const CmdOpt &opt)
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
            openFilesMsg.files.push_back({ u8str(file.generic_u8string()), row });
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
