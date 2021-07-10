#include "charset.h"


namespace doc::CharsetUtil
{

Charset strToCharset(const std::string &s)
{
	if (s == "ASCII") {
		return Charset::Ascii;
	}

	if (s == "UTF-8") {
		return Charset::UTF_8;
	}

	if (s == "GB18030") {
		return Charset::GB18030;
	}

	return Charset::Unknown;
}

std::string charsetToStr(Charset charset)
{
	switch (charset) {
	case Charset::Ascii:
		return "ASCII";
	case Charset::UTF_8:
		return "UTF-8";
	case Charset::GB18030:
		return "GB18030";
	default:
		return std::string();
	}
}

}
