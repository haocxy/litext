#pragma once

#include "win32_core.h"


namespace win32
{

class NamedPipeServer {
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

    ~NamedPipeServer() {
    }

    void start(const Info &info);

    enum class ConnectResult {
        Connected, ClientDisconnected,
    };

    ConnectResult waitConnect();

    void close() {
        handle_.close();
    }

    void read(void *dest, i64 nbytes);

    ::HANDLE getHANDLE() const {
        return handle_.get();
    }

    ::HANDLE takeHANDLE() {
        return handle_.take();
    }

private:
    ObjHandle handle_;
};

}
