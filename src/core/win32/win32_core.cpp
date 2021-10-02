#include "win32_core.h"


namespace win32
{

std::string ErrorCode::message() const {
    char *msgBuffer = nullptr;

    ::FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, n_, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPSTR)&msgBuffer, 0, nullptr);

    if (!msgBuffer) {
        throw std::runtime_error("win32::ErrorCode::message() failed because ::FormatMessageA() failed");
    }

    std::unique_ptr<void, std::function<void(void *)>> deleter(msgBuffer, [](void *p) {
        ::LocalFree(p);
    });

    std::string msg(msgBuffer);

    while (!msg.empty()) {
        const char ch = msg.back();
        if (ch != '\r' && ch != '\n') {
            msg.pop_back();
        }
    }

    std::ostringstream ss;
    ss << '(' << n_ << ')' << msg;

    return ss.str();
}

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
        return ConnectResult::ClientDisconnected;
    }

    throw Win32LogicError(ErrorCode::last());
}

void NamedPipeServer::read(void *dest, i64 nbytes) {
    unsigned char *output = reinterpret_cast<unsigned char *>(dest);
    unsigned char *end = output + nbytes;
    while (output < end) {
        const ::DWORD remain = static_cast<::DWORD>(end - output);
        ::DWORD got = 0;
        const ::BOOL result = ::ReadFile(handle_.get(), output, remain, &got, nullptr);
        if (result != 0 || result == ERROR_MORE_DATA) {
            if (got == remain) {
                return;
            } else if (got < remain) {
                output += got;
            } else {
                throw std::runtime_error("win32::NamedPipeServer::read() bad logic(1)");
            }
        } else {
            throw Win32LogicError(ErrorCode::last());
        }
    }
}




}
