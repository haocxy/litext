#include "win32_named_pipe.win32.h"


namespace win32
{

void NamedPipeServer::Info::setOpenMode(DataDirection dir, bool async) {
    dwOpenMode_ = 0;
    switch (dir) {
    case DataDirection::ToServer:
        dwOpenMode_ = PIPE_ACCESS_INBOUND;
        break;
    case DataDirection::ToClient:
        dwOpenMode_ = PIPE_ACCESS_OUTBOUND;
        break;
    case DataDirection::Both:
        dwOpenMode_ = PIPE_ACCESS_DUPLEX;
        break;
    default:
        break;
    }
    if (async) {
        dwOpenMode_ |= FILE_FLAG_OVERLAPPED;
    }
}

void NamedPipeServer::Info::setDataUnit(DataUnit unit) {
    switch (unit) {
    case DataUnit::Byte:
        dwPipeMode_ = PIPE_TYPE_BYTE;
        break;
    case DataUnit::Message:
        dwPipeMode_ = PIPE_TYPE_MESSAGE;
        break;
    default:
        break;
    }
}

void NamedPipeServer::start(const Info &info) {
    handle_ = ::CreateNamedPipeA(
        info.name_.c_str(),
        info.dwOpenMode_,
        info.dwPipeMode_,
        info.nMaxInstances_,
        info.nOutBufferSize_,
        info.nInBufferSize_,
        info.nDefaultTimeOut_,
        info.lpSecurityAttributes_);

    if (!handle_.isValid()) {
        throw Win32LogicError(ErrorCode::last());
    }
}

NamedPipeServer::ConnectResult NamedPipeServer::waitConnect() {
    const ::BOOL result = ::ConnectNamedPipe(handle_.get(), nullptr);
    if (result != 0) {
        return ConnectResult::Connected;
    }

    const DWORD ec = ::GetLastError();
    if (ec == ERROR_PIPE_CONNECTED) {
        return ConnectResult::Connected;
    }

    if (ec == ERROR_NO_DATA) {
        disconnected_ = true;
        return ConnectResult::ClientDisconnected;
    }

    throw Win32LogicError(ErrorCode::last());
}

void NamedPipe::read(void *dest, i64 nbytes) {
    if (!good()) {
        return;
    }

    unsigned char *output = reinterpret_cast<unsigned char *>(dest);
    unsigned char *end = output + nbytes;
    while (output < end) {
        const ::DWORD remain = static_cast<::DWORD>(end - output);
        ::DWORD got = 0;
        const ::BOOL result = ::ReadFile(handle_.get(), output, remain, &got, nullptr);
        if (result != 0 || result == ERROR_MORE_DATA) {
            if (got == remain) {
                return;
            } else if (got == 0) {
                disconnected_ = true;
                return;
            } else if (got < remain) {
                output += got;
            } else {
                throw std::runtime_error("win32::NamedPipe::read() bad logic(1)");
            }
        } else {
            const ::DWORD ec = ::GetLastError();
            if (ec == ERROR_BROKEN_PIPE) {
                disconnected_ = true;
                return;
            }
            throw Win32LogicError(ErrorCode::last());
        }
    }
}

void NamedPipe::write(const void *data, i64 nbytes)
{
    if (!good()) {
        return;
    }

    const unsigned char *input = reinterpret_cast<const unsigned char *>(data);
    const unsigned char *end = input + nbytes;
    while (input < end) {
        const ::DWORD remain = static_cast<::DWORD>(end - input);
        ::DWORD put = 0;
        const ::BOOL result = ::WriteFile(handle_.get(), input, remain, &put, nullptr);
        if (result != 0) {
            if (put == remain) {
                return;
            } else if (put == 0) {
                disconnected_ = true;
                return;
            } else if (put < remain) {
                input += put;
            } else {
                throw std::runtime_error("win32::NamedPipe::write() bad logic(1)");
            }
        } else {
            const ::DWORD ec = ::GetLastError();
            if (ec == ERROR_BROKEN_PIPE || ec == ERROR_NO_DATA) {
                disconnected_ = true;
                return;
            }
            throw Win32LogicError(ErrorCode::last());
        }
    }
}

void NamedPipe::flush()
{
    const ::BOOL result = ::FlushFileBuffers(handle_.get());
    if (result != 0) {
        return;
    } else {
        throw Win32LogicError(ErrorCode::last());
    }
}


void NamedPipeClient::doStart(const std::string &name, ::DWORD timeoutMs)
{
    while (true) {

        handle_ = ::CreateFileA(
            name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
            OPEN_EXISTING, 0, nullptr);

        if (handle_.isValid()) {
            return;
        }

        const ErrorCode ec = ErrorCode::last();
        if (ec.raw() != ERROR_PIPE_BUSY) {
            throw Win32LogicError(ec);
        }

        if (!::WaitNamedPipeA(name.c_str(), timeoutMs)) {
            throw Win32RuntimeError("NamedPipeClient::start() timeout");
        }
    }
}

}
