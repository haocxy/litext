#pragma once

#include <chrono>
#include <boost/interprocess/sync/file_lock.hpp>

#include "fs.h"


class FileLock {
public:
    FileLock() {}

#ifdef WIN32
    explicit FileLock(const fs::path &path)
        : fileLock_(path.generic_wstring().c_str()) {}
#else
    explicit FileLock(const fs::path &path)
        : fileLock(path.generic_u8string().c_str()) {}
#endif

    FileLock(const FileLock &) = delete;

    FileLock(FileLock &&b) noexcept
        : fileLock_(std::move(b.fileLock_)) {}

    FileLock &operator=(const FileLock &) = delete;

    FileLock &operator=(FileLock &&b) noexcept {
        fileLock_ = std::move(b.fileLock_);
        return *this;
    }

    void lock() {
        fileLock_.lock();
    }

    void unlock() {
        fileLock_.unlock();
    }

private:
    boost::interprocess::file_lock fileLock_;
};


class FileLockGuard {
public:
    FileLockGuard() {}

    explicit FileLockGuard(FileLock &fileLock)
        : fileLock_(&fileLock) {

        fileLock.lock();
    }

    FileLockGuard(const FileLockGuard &) = delete;

    FileLockGuard(FileLockGuard &&b) noexcept
        : fileLock_(b.fileLock_) {
        b.fileLock_ = nullptr;
    }

    FileLockGuard &operator=(const FileLockGuard &) = delete;

    FileLockGuard &operator=(FileLockGuard &&b) noexcept {
        if (fileLock_) {
            fileLock_->unlock();
            fileLock_ = nullptr;
        }
        fileLock_ = b.fileLock_;
        b.fileLock_ = nullptr;
        return *this;
    }

    ~FileLockGuard() {
        if (fileLock_) {
            fileLock_->unlock();
        }
    }

private:
    FileLock *fileLock_ = nullptr;
};
