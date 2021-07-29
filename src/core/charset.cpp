#include "charset.h"

#include <stdexcept>

#include <third/uchardet-0.0.7/src/uchardet.h>


namespace CharsetUtil
{

Charset strToCharset(const std::string &s)
{
	if (s == "ASCII") {
		return Charset::Ascii;
	}

	if (s == "UTF-8") {
		return Charset::UTF_8;
	}

    if (s == "UTF-16") {
        return Charset::UTF_16;
    }

	if (s == "GB18030") {
		return Charset::GB18030;
	}

	return Charset::Unknown;
}

const char *charsetToStr(Charset charset)
{
	switch (charset) {
	case Charset::Ascii:
		return "ASCII";
	case Charset::UTF_8:
		return "UTF-8";
    case Charset::UTF_16:
        return "UTF-16";
	case Charset::GB18030:
		return "GB18030";
	default:
		return "";
	}
}

}

CharsetDetector::CharsetDetector() {
    handle_ = uchardet_new();
    if (!handle_) {
        throw std::runtime_error("uchardet_new() failed");
    }
}

CharsetDetector::~CharsetDetector() {
    if (handle_) {
        uchardet_delete(handle_);
        handle_ = nullptr;
    }
}

bool CharsetDetector::feed(const void *data, size_t len) {
    int result = uchardet_handle_data(handle_, reinterpret_cast<const char *>(data), len);
    return result == 0;
}

void CharsetDetector::end() {
    uchardet_data_end(handle_);
}

void CharsetDetector::reset() {
    uchardet_reset(handle_);
}

const char *CharsetDetector::charset() const {
    return uchardet_get_charset(handle_);
}
