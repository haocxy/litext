#include "charset_detect_util.h"

#include <fstream>
#include <algorithm>

#include "core/heap_array.h"
#include "charset_detector.h"


namespace doc::CharsetDetectUtil
{


std::string detectCharsetOfFile(const fs::path &path, size_t offset, size_t len)
{
	CharsetDetector detector;

	std::ifstream ifs(path, std::ios::binary);

	ifs.seekg(0, std::ios::end);
	const size_t fileSize = ifs.tellg();

	if (offset >= fileSize) {
		return "";
	}

	const size_t remain = fileSize - offset;
	const size_t realLen = len > 0 ? std::min(remain, len) : remain;

	HeapArray buff(realLen);

	ifs.seekg(offset);
	ifs.read(buff.data(), realLen);

	if (!detector.feed(buff.data(), buff.size())) {
		return "";
	}

	detector.end();

	return detector.charset();
}


}
