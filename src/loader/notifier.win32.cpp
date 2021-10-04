#include "notifier.win32.h"

#include <optional>

#include "core/win32/win32_named_pipe.win32.h"

#include "global/msg.h"
#include "global/constants.h"


namespace w = win32;
using Client = w::NamedPipeClient;

static bool send(Client &client, const std::string &data)
{
    const i64 dataLen = data.size();
    client.write(&dataLen, sizeof(dataLen));
    if (!client.good()) {
        return false;
    }

    client.write(data.data(), data.size());
    if (!client.good()) {
        return false;
    }

    return true;
}

static bool send(Client &client, const msg::Pack &pack)
{
    return send(client, pack.serialize());
}

static std::optional<msg::Pack> recv(Client &client)
{
    i64 dataLen = 0;
    client.read(&dataLen, sizeof(dataLen));
    if (!client.good()) {
        return std::nullopt;
    }

    std::string data;
    data.resize(dataLen);
    client.read(data.data(), data.size());
    if (!client.good()) {
        return std::nullopt;
    }

    return msg::Pack::deserialize(data);
}

bool notifyServerForWin32(const CmdOpt &opt)
{
    Client client;
    client.start(constants::SingletonPipeNameWin32);

    if (opt.files().empty()) {
        send(client, msg::ActivateUI());
    } else {
        msg::OpenFiles openFilesMsg;
        for (const auto &[file, row] : opt.files()) {
            openFilesMsg.files.push_back({ u8str(file.generic_u8string()), row });
        }
        send(client, openFilesMsg);
    }

    std::optional<msg::Pack> result = recv(client);
    if (result && result->is<msg::Ok>()) {
        return true;
    } else {
        return false;
    }
}
