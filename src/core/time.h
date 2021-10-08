#pragma once

#include <ctime>
#include <chrono>
#include <mutex>
#include <shared_mutex>

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

template <typename ChronoClock>
class TimeUsage {
public:
    TimeUsage() {}

    void start() {
        WriteLock lock(mtx_);
        startTime_ = ChronoClock::now();
        stopTime_ = TimePoint();
        state_ = State::Started;
    }

    void stop() {
        WriteLock lock(mtx_);
        if (state_ == State::Started) {
            stopTime_ = ChronoClock::now();
            state_ = State::Stoped;
        } else {
            if (state_ == State::NotStarted) {
                throw std::logic_error("not started");
            }
            if (state_ == State::Stoped) {
                throw std::logic_error("already stopped");
            }
            throw std::logic_error("bad state");
        }
    }

    i64 sec() const {
        using Seconds = std::chrono::seconds;
        if (state_ == State::Stoped) {
            return std::chrono::duration_cast<Seconds>(stopTime_ - startTime_).count();
        } else {
            throwBadStateForRead(state_);
        }
    }

    i64 ms() const {
        using Ms = std::chrono::milliseconds;
        if (state_ == State::Stoped) {
            return std::chrono::duration_cast<Ms>(stopTime_ - startTime_).count();
        } else {
            throwBadStateForRead(state_);
        }
    }

private:

    enum class State {
        NotStarted, Started, Stoped
    };

    [[noreturn]]
    static void throwBadStateForRead(State st) {
        if (st == State::NotStarted) {
            throw std::logic_error("not started and not stopped");
        }
        if (st == State::Started) {
            throw std::logic_error("started but not stopped");
        }
        throw std::logic_error("bad state for read");
    }

private:
    using Mtx = std::shared_mutex;
    using ReadLock = std::shared_lock<Mtx>;
    using WriteLock = std::lock_guard<Mtx>;

    mutable Mtx mtx_;
    State state_ = State::NotStarted;

    using TimePoint = typename ChronoClock::time_point;
    TimePoint startTime_{};
    TimePoint stopTime_{};
};

namespace TimeUtil
{

std::chrono::system_clock::time_point compileTime(const char *dateByMacro, const char *timeByMacro);

}
