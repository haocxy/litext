#pragma once

#include <variant>

#include "win32_core.h"

#include "core/basetype.h"


namespace win32
{

class NamedPipe {
public:

    virtual ~NamedPipe() {}

    void close() {
        handle_.close();
    }

    void read(void *dest, i64 nbytes);

    void write(const void *data, i64 nbytes);

    void flush();

    bool good() const {
        return !disconnected_;
    }

protected:
    ObjHandle handle_;
    bool disconnected_ = false;
};

class NamedPipeServer : public NamedPipe {
public:

    enum class DataDirection {
        ToServer, ToClient, Both
    };

    // 管道中的数据以字节为单位还是消息为单位
    // 以字节为单位时需要上层逻辑自己维护消息边界
    // 以消息为单位时操作系统会维护消息边界
    enum class DataUnit { Byte, Message };

    class Info {
    public:
        Info() {}

        void setName(const std::string &name) {
            name_ = name;
        }

        void setOpenMode(DataDirection dir, bool async);

        void setDataUnit(DataUnit unit);

        void setOutBufferSize(DWORD size) {
            nOutBufferSize_ = size;
        }

        void setInBufferSize(DWORD size) {
            nInBufferSize_ = size;
        }

        void setDefaultTimeOut(DWORD ms) {
            nDefaultTimeOut_ = ms;
        }

    private:
        std::string name_;
        DWORD dwOpenMode_ = PIPE_ACCESS_DUPLEX;
        DWORD dwPipeMode_ = PIPE_TYPE_BYTE;
        DWORD nMaxInstances_ = PIPE_UNLIMITED_INSTANCES;
        DWORD nOutBufferSize_ = 1024;
        DWORD nInBufferSize_ = 1024;
        DWORD nDefaultTimeOut_ = 0;
        LPSECURITY_ATTRIBUTES lpSecurityAttributes_ = nullptr;

        friend class NamedPipeServer;
    };

    NamedPipeServer() {}

    NamedPipeServer(const Info &info) {
        start(info);
    }

    virtual ~NamedPipeServer() {
    }

    void start(const Info &info);

    enum class ConnectResult {
        Connected, ClientDisconnected,
    };

    ConnectResult waitConnect();
};

class NamedPipeClient : public NamedPipe {
public:

    enum class StartWaitPolicy { Default, Forever };

    NamedPipeClient() {}

    NamedPipeClient(const std::string &name) {
        start(name);
    }

    NamedPipeClient(const std::string &name, StartWaitPolicy waitPolicy) {
        start(name, waitPolicy);
    }

    NamedPipeClient(const std::string &name, i64 timeoutMs) {
        start(name, timeoutMs);
    }

    virtual ~NamedPipeClient() {}

    void start(const std::string &name) {
        doStart(name, NMPWAIT_USE_DEFAULT_WAIT);
    }

    void start(const std::string &name, StartWaitPolicy waitPolicy) {
        switch (waitPolicy) {
        case StartWaitPolicy::Default:
            doStart(name, NMPWAIT_USE_DEFAULT_WAIT);
            break;
        case StartWaitPolicy::Forever:
            doStart(name, NMPWAIT_WAIT_FOREVER);
            break;
        default:
            break;
        }
    }

    void start(const std::string &name, i64 timeoutMs) {
        if (timeoutMs > NMPWAIT_USE_DEFAULT_WAIT && timeoutMs < NMPWAIT_WAIT_FOREVER) {
            doStart(name, static_cast<::DWORD>(timeoutMs));
        } else {
            throw Win32LogicError("NamedPipeClient::start() bad arg (timeoutMs)");
        }
    }

private:
    void doStart(const std::string &name, ::DWORD timeoutMs);
};

}
