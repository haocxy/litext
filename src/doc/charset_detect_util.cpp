#include "charset_detect_util.h"

#include <fstream>
#include <algorithm>
#include <future>
#include <list>
#include <vector>

#include "core/heap_array.h"
#include "charset_detector.h"


namespace doc::CharsetDetectUtil
{


std::string detectCharsetOfFile(const fs::path &path, size_t offset, size_t len)
{
	CharsetDetector detector;

	const size_t fileSize = fs::file_size(path);
	if (offset >= fileSize) {
		return "";
	}

	const size_t remain = fileSize - offset;
	const size_t realLen = len > 0 ? std::min(remain, len) : remain;

	HeapArray buff(realLen);
	std::ifstream ifs(path, std::ios::binary);
	ifs.seekg(offset);
	ifs.read(buff.data(), realLen);

	if (!detector.feed(buff.data(), buff.size())) {
		return "";
	}

	detector.end();

	return detector.charset();
}

std::string quickDetectCharset(const fs::path &path)
{
	const uintmax_t fileSize = fs::file_size(path);
	constexpr uintmax_t page = 10 * 1024 * 1024;
	if (fileSize <= page) {
		return detectCharsetOfFile(path);
	}

	const uintmax_t pageCount = fileSize / page;
	const uintmax_t threadCount = std::min(pageCount, static_cast<uintmax_t>(4));

	// 确定好线程数threadCount后，把文件分为threadCount个大小partSize的部分
	// 异步处理每个partSize部分的前page个字节
	const uintmax_t partSize = fileSize / threadCount;

	std::vector<std::future<std::string>> futures;
	for (uintmax_t i = 0; i < threadCount; ++i) {
		const uintmax_t partBegin = i * partSize;
		// 因为不知道具体编码，所以需要取不同的起点尝试，假设未知的编码可能最多用maxEncodedByteCount个字节
		constexpr int maxEncodedByteCount = 6;
		for (int off = 0; off < maxEncodedByteCount; ++off) {
			futures.push_back(std::async(std::launch::async, [path, partBegin, off, page]()->std::string {
				return detectCharsetOfFile(path, partBegin + off, page);
			}));
		}
	}

	std::list<std::string> charsets;
	for (std::future<std::string> &future : futures) {
		charsets.push_back(future.get());
	}

	if (charsets.empty()) {
		return std::string();
	}

	if (charsets.size() == 1) {
		return *(charsets.begin());
	}

	for (auto it = charsets.begin(); it != charsets.end(); ) {
		if ((*it) == "ASCII") {
			it = charsets.erase(it);
		} else {
			++it;
		}
	}

	if (!charsets.empty()) {
		return charsets.front();
	} else {
		return "ASCII";
	}
}


}
