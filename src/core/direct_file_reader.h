#pragma once

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <sstream>

#include "fs.h"


class DirectFileReader {
public:
	DirectFileReader(const fs::path &path) {
		std::FILE *filep = std::fopen(path.generic_string().c_str(), "rb");
		if (!filep) {
			std::ostringstream ss;
			ss << "Cannot open file [" << path.generic_string() << "] ";
			ss << "because [" << std::strerror(errno) << "]";
			throw std::runtime_error(ss.str());
		}

		std::setbuf(filep, nullptr);

		filep_ = filep;
	}

	~DirectFileReader() {
		if (filep_) {
			std::fclose(filep_);
			filep_ = nullptr;
		}
	}

	size_t read(void *buffer, size_t len) {
		assertFileOpened();
		if (buffer) {
			return std::fread(buffer, 1, len, filep_);
		} else {
			return 0;
		}
	}

	uintmax_t tell() const {
		assertFileOpened();
		const long n = std::ftell(filep_);
		if (n < 0) {
			std::ostringstream ss;
			ss << "std::ftell() error: [" << std::strerror(errno) << "]";
			throw std::runtime_error(ss.str());
		}
		return static_cast<uintmax_t>(n);
	}

	void seek(uintmax_t offset) {
		assertFileOpened();
		const int n = std::fseek(filep_, static_cast<long>(offset), SEEK_SET);
		if (n != 0) {
			std::ostringstream ss;
			ss << "std::fseek() error: [" << std::strerror(errno) << "]";
		}
	}

private:
	void assertFileOpened() const {
		if (!filep_) {
			throw std::runtime_error("File not opened");
		}
	}

private:
	std::FILE *filep_ = nullptr;
};
