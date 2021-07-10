#pragma once

#include <cstdint>


namespace doc
{


class OffsetRange {
public:
	OffsetRange() {}

	OffsetRange(uintmax_t offset, uintmax_t length)
		: offset_(offset), length_(length)
	{

	}

	bool empty() const {
		return length_ == 0;
	}

	uintmax_t offset() const {
		return offset_;
	}

	uintmax_t length() const {
		return length_;
	}

private:
	uintmax_t offset_ = 0;
	uintmax_t length_ = 0;
};


}
