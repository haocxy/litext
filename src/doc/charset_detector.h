#pragma once

#include <stdexcept>
#include <string>

#include <third/uchardet-0.0.7/src/uchardet.h>


class CharsetDetector {
public:
	CharsetDetector() {
		handle_ = uchardet_new();
		if (!handle_) {
			throw std::runtime_error("uchardet_new() failed");
		}
	}

	~CharsetDetector() {
		if (handle_) {
			uchardet_delete(handle_);
			handle_ = nullptr;
		}
	}

	CharsetDetector(const CharsetDetector &) = delete;

	CharsetDetector(CharsetDetector &&) = delete;

	CharsetDetector &operator=(const CharsetDetector &) = delete;

	CharsetDetector &operator=(CharsetDetector &&) = delete;

	bool feed(const void *data, size_t len) {
		int result = uchardet_handle_data(handle_, reinterpret_cast<const char *>(data), len);
		return result == 0;
	}

	void end() {
		uchardet_data_end(handle_);
	}

	void reset() {
		uchardet_reset(handle_);
	}

	std::string charset() const {
		return uchardet_get_charset(handle_);
	}

private:
	uchardet_t handle_ = nullptr;
};
