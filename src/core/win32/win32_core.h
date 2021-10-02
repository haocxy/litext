#pragma once

#include <Windows.h>

#include <mutex>
#include <string>
#include <stdexcept>


namespace win32
{

using RawHandle = ::HANDLE;


class ErrorCode {
public:
    ErrorCode() {}

    ErrorCode(::DWORD n) : n_(n) {}

    static ErrorCode last() {
        return ErrorCode(::GetLastError());
    }

    std::string message() const;

private:
    ::DWORD n_ = 0;
};

class Win32LogicError : public std::logic_error {
public:
    Win32LogicError(const std::string &msg) : std::logic_error(msg) {}

    Win32LogicError(const ErrorCode &ec) : std::logic_error(ec.message()) {}
};

class Win32RuntimeError : public std::runtime_error {
public:
    Win32RuntimeError(const std::string &msg) : std::runtime_error(msg) {}

    Win32RuntimeError(const ErrorCode &ec) : std::runtime_error(ec.message()) {}
};

class ObjHandle {
public:
    ObjHandle() {}

    ObjHandle(RawHandle handle)
        : handle_(handle) {}

    ObjHandle(const ObjHandle &) = delete;

    ObjHandle(ObjHandle &&other) noexcept {
        Lock lockOther(other.mtx_);
        _move(*this, other);
    }

    ObjHandle &operator=(RawHandle handle) {
        Lock lockThis(mtx_);
        _close();
        handle_ = handle;
        return *this;
    }

    ObjHandle &operator=(const ObjHandle &) = delete;

    ObjHandle &operator=(ObjHandle &&other) noexcept {
        Lock lockThis(mtx_);
        Lock lockOther(other.mtx_);
        _move(*this, other);
        return *this;
    }

    ~ObjHandle() {
        _close();
    }

    void close() {
        Lock lock(mtx_);
        _close();
    }

    bool isValid() const {
        Lock lock(mtx_);
        return handle_ != INVALID_HANDLE_VALUE;
    }

    RawHandle get() const {
        Lock lock(mtx_);
        return handle_;
    }

    RawHandle take() {
        Lock lock(mtx_);
        ::HANDLE handle = handle_;
        handle_ = INVALID_HANDLE_VALUE;
        return handle;
    }

private:
    void _close() {
        if (handle_ != INVALID_HANDLE_VALUE) {
            ::CloseHandle(handle_);
            handle_ = INVALID_HANDLE_VALUE;
        }
    }

private:
    static void _move(ObjHandle &to, ObjHandle &from) {
        if (&to != &from) {
            to._close();
            to.handle_ = from.handle_;
            from.handle_ = INVALID_HANDLE_VALUE;
        }
    }

private:
    using Mtx = std::mutex;
    using Lock = std::lock_guard<Mtx>;

    mutable Mtx mtx_;
    RawHandle handle_ = INVALID_HANDLE_VALUE;
};


}
