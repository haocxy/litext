#include "charset_detect_util.h"

#include <cassert>
#include <fstream>
#include <algorithm>
#include <future>
#include <list>
#include <vector>

#include "core/system_util.h"
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
	namespace sys = SystemUtil;

	const uintmax_t fileSize = fs::file_size(path);

	// 每次检测文件片段的字节数
	const uintmax_t blockSize = sys::pageSize() * 1024;
	if (fileSize <= blockSize) {
		return detectCharsetOfFile(path);
	}

	const uintmax_t blockCount = fileSize / blockSize;

	// 如果页的数量超过了处理器数量，则限制检测的片段数为处理器数量
	const uintmax_t partCount = std::min(blockCount, static_cast<uintmax_t>(sys::processorCount()));

	// 确定好线程数threadCount后，把文件分为threadCount个大小partSize的部分
	// 异步处理每个partSize部分的前page个字节
	const uintmax_t partSize = fileSize / partCount;

	// 异步地检测每个片段
	std::vector<std::future<std::string>> futures;

	// 开始部分特殊处理，这是因为开始部分不需要考虑边界不确定的情况
	const std::string firstPartCharset = detectCharsetOfFile(path, 0, blockSize);
	if (firstPartCharset != "ASCII") {
		return firstPartCharset;
	}

	constexpr int maxEncodedByteCount = 6;
	assert(maxEncodedByteCount < blockSize);

	// 文档非开始的部分，需要考虑边界不确定的情况
	for (uintmax_t i = 1; i < partCount; ++i) {
		const uintmax_t partBegin = i * partSize;
		// 因为不知道具体编码，所以需要取不同的起点尝试，假设未知的编码可能最多用maxEncodedByteCount个字节
		for (int off = 0; off < maxEncodedByteCount; ++off) {
			futures.push_back(std::async(std::launch::async, [path, partBegin, off, blockSize]()->std::string {
				return detectCharsetOfFile(path, partBegin + off, blockSize);
			}));
		}
	}

	std::list<std::string> charsets;
	for (std::future<std::string> &future : futures) {
		charsets.push_back(future.get());
	}

	// 没有检测到任何编码，认为是最简单的ASCII编码
	if (charsets.empty()) {
		return "ASCII";
	}

	// 如果检测的结果只有一个，直接将其返回
	if (charsets.size() == 1) {
		return *(charsets.begin());
	}

	// 如果有多个检测结果，则尝试去掉其中的ASCII编码结果
	for (auto it = charsets.begin(); it != charsets.end(); ) {
		if ((*it) == "ASCII") {
			it = charsets.erase(it);
		} else {
			++it;
		}
	}

	// 在上一步去掉所有的ASCII编码结果后，
	// 如果有非ASCII编码的结果，则简单地取第一个将其返回，因为正常文档的编码应该是一致的，
	// 如果所有结果都是ASCII编码，则认为整个文档是ASCII编码的
	if (!charsets.empty()) {
		return charsets.front();
	} else {
		return "ASCII";
	}
}


}
