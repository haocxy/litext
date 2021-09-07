#pragma once

#include <mutex>
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
        : fileLock_(path.generic_u8string().c_str()) {}
#endif

    FileLock(const FileLock &) = delete;

    FileLock(FileLock &&b) noexcept
        : fileLock_(std::move(b.fileLock_))
        , locked_(b.locked_) {

        b.locked_ = false;
    }

    ~FileLock() {
        unlock();
    }

    FileLock &operator=(const FileLock &) = delete;

    FileLock &operator=(FileLock &&b) noexcept {
        ThreadLock threadLock(threadMtx_);
        if (locked_) {
            fileLock_.unlock();
        }
        locked_ = b.locked_;
        b.locked_ = false;
        fileLock_ = std::move(b.fileLock_);
        return *this;
    }

    void lock() {
        ThreadLock threadLock(threadMtx_);
        if (!locked_) {
            fileLock_.lock();
            locked_ = true;
        }
    }

    bool tryLock() {
        ThreadLock threadLock(threadMtx_);
        if (!locked_) {
            locked_ = fileLock_.try_lock();
        }
        return locked_;
    }

    void unlock() {
        ThreadLock threadLock(threadMtx_);
        if (locked_) {
            fileLock_.unlock();
            locked_ = false;
        }
    }

private:
    using ThreadMtx = std::mutex;
    using ThreadLock = std::lock_guard<ThreadMtx>;
    mutable ThreadMtx threadMtx_;
    boost::interprocess::file_lock fileLock_;
    bool locked_ = false;
};

class ScopedFileLock {
public:
    ScopedFileLock(const fs::path &file)
        : lock_(file) {

        lock_.lock();
    }

    ScopedFileLock(const ScopedFileLock &) = delete;

    ScopedFileLock(ScopedFileLock &&b) noexcept
        : lock_(std::move(b.lock_)) {}

    ~ScopedFileLock() {
        lock_.unlock();
    }

    ScopedFileLock &operator=(const ScopedFileLock &) = delete;

    ScopedFileLock &operator=(ScopedFileLock &&b) noexcept {
        unlock();
        lock_ = std::move(b.lock_);
        return *this;
    }

    void lock() {
        lock_.lock();
    }

    bool tryLock() {
        return lock_.tryLock();
    }

    void unlock() {
        lock_.unlock();
    }

private:
    FileLock lock_;
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
