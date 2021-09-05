#pragma once

#include <ctime>
#include <chrono>
#include <mutex>

#include "basetype.h"


class ElapsedTime {
public:
    ElapsedTime() {}

	ElapsedTime(const ElapsedTime &) = delete;

	ElapsedTime(ElapsedTime &&) = delete;

	ElapsedTime &operator=(const ElapsedTime &) = delete;

	ElapsedTime &operator=(ElapsedTime &&) = delete;

    void start() {
        beg_ = std::chrono::steady_clock::now();
    }

	i64 ms() const {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - beg_).count();
	}

private:
	std::chrono::time_point<std::chrono::steady_clock> beg_;
};

class AtomicElapsedTime {
public:
    AtomicElapsedTime() {}

    AtomicElapsedTime(const AtomicElapsedTime &) = delete;

    AtomicElapsedTime(AtomicElapsedTime &&) = delete;

    AtomicElapsedTime &operator=(const AtomicElapsedTime &) = delete;

    AtomicElapsedTime &operator=(AtomicElapsedTime &&) = delete;

    void start() {
        std::unique_lock<std::mutex> lock(mtx_);
        beg_ = std::chrono::steady_clock::now();
    }

    i64 ms() const {
        std::unique_lock<std::mutex> lock(mtx_);
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - beg_).count();
    }

private:
    mutable std::mutex mtx_;
    std::chrono::time_point<std::chrono::steady_clock> beg_;
};

namespace TimeUtil
{

std::chrono::system_clock::time_point compileTime(const char *dateByMacro, const char *timeByMacro);

}
