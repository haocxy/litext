#include "singleton_server.win32.h"

#include "core/thread.h"
#include "core/win32/win32_named_pipe.win32.h"

#include "global/msg.h"


namespace gui
{

SingletonServerWin32::SingletonServerWin32()
{
    pipeServerThread_ = std::thread([this] {
        pipeServerLoop();
    });
}

SingletonServerWin32::~SingletonServerWin32()
{
    pipeServerThread_.join();
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

static SingletonServerWin32::OpenInfos toOpenInfos(const msg::Pack &pack) {
    SingletonServerWin32::OpenInfos openInfos;
    msg::OpenFiles openFilesMsg = pack.unpack();
    for (const msg::OpenFiles::OpenInfo &info : openFilesMsg.files) {
        u32str u32s = info.u8file;
        openInfos.push_back({ fs::path(static_cast<std::u32string &&>(u32s)), info.row });
    }
    return openInfos;
}

void SingletonServerWin32::pipeServerLoop()
{
    ThreadUtil::setNameForCurrentThread("SingletonServerWin32-pipeServerLoop");
    while (true) {
        using Server = win32::NamedPipeServer;
        using ConnRes = Server::ConnectResult;
        Server::Info info;
        info.setName("\\\\.\\pipe\\LitextNamedPipe");
        Server server;
        server.start(info);
        ConnRes result = server.waitConnect();
        if (result == ConnRes::ClientDisconnected) {
            continue;
        }

        i64 msgLen = 0;
        server.read(&msgLen, sizeof(msgLen));
        if (!server.good()) {
            continue;
        }

        std::string msgData;
        msgData.resize(msgLen);
        server.read(msgData.data(), msgData.size());
        if (!server.good()) {
            continue;
        }

        msg::Pack resp;

        msg::Pack pack = msg::Pack::deserialize(msgData);
        if (pack.is<msg::ActivateUI>()) {
            sigActivateUI_();
            resp = msg::Ok();
        } else if (pack.is<msg::OpenFiles>()) {
            sigRecvOpenInfos_(toOpenInfos(pack));
            resp = msg::Ok();
        } else {
            continue;
        }

        const std::string respData = resp.serialize();
        const i64 respLen = respData.size();
        server.write(&respLen, sizeof(respLen));
        if (!server.good()) {
            continue;
        }
        server.write(respData.data(), respData.size());
        if (!server.good()) {
            continue;
        }

        server.flush();
    }
}

}
