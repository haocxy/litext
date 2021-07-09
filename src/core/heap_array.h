#pragma once


class HeapArray {
public:
	HeapArray(size_t len)
		: ptr_(new char[len])
		, len_(len) {}

	~HeapArray() {
		delete ptr_;
		ptr_ = nullptr;
	}

	HeapArray(const HeapArray &) = delete;

	HeapArray(HeapArray &&) = delete;

	HeapArray &operator=(const HeapArray &) = delete;

	HeapArray &operator=(HeapArray &&) = delete;

	char *data() const {
		return ptr_;
	}

	size_t size() const {
		return len_;
	}

private:
	char *ptr_ = nullptr;
	const size_t len_;
};
