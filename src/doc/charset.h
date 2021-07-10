#pragma once

#include <string>


namespace doc
{


enum class Charset {
	Unknown = 0,
	Ascii,
	UTF_8,
	GB18030,
};

namespace CharsetUtil
{

Charset strToCharset(const std::string &s);

std::string charsetToStr(Charset charset);

}

}
