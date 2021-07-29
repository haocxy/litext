#pragma once

#include <ctime>
#include <chrono>


class ElapsedTime {
public:
	using MilliSec = std::chrono::milliseconds::rep;

	ElapsedTime() : beg_(std::chrono::steady_clock::now()) {}

	ElapsedTime(const ElapsedTime &) = delete;

	ElapsedTime(ElapsedTime &&) = delete;

	ElapsedTime &operator=(const ElapsedTime &) = delete;

	ElapsedTime &operator=(ElapsedTime &&) = delete;

	MilliSec milliSec() const {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - beg_).count();
	}

private:
	const std::chrono::time_point<std::chrono::steady_clock> beg_;
};


namespace TimeUtil
{

std::chrono::system_clock::time_point compileTime(const char *dateByMacro, const char *timeByMacro);

}
