#include "document_impl.h"

#include <iostream>
#include <future>

#include "core/logger.h"
#include "core/time_util.h"
#include "core/system_util.h"


namespace doc::detail
{

DocumentImpl::DocumentImpl(const fs::path &path, Worker &ownerThread)
	: path_(path)
	, asyncComponents_(new AsyncComponents(path))
	, db_(path.generic_string() + ".notesharp.db")
	, ownerThread_(ownerThread)
{
	LOGD << "DocumentImpl::DocumentImpl(...) for [" << path << "]";
}

static uintmax_t partSize() {
	return SystemUtil::pageSize() * 1024;
}

static void skipRowEndForAscii(std::ifstream &ifs, std::vector<unsigned char> &skipBuff) {
	constexpr char eof = std::ifstream::traits_type::eof();
	char ch = 0;
	while ((ch = ifs.get()) != eof) {
		skipBuff.push_back(ch);
		if (ch == '\r') {
			char next = 0;
			if ((next = ifs.get()) != eof) {
				if (next == '\n') {
					skipBuff.push_back(next);
					return;
				} else {
					ifs.unget();
					continue;
				}
			} else {
				return;
			}
		} else if (ch == '\n') {
			return;
		} else {
			continue;
		}
	}
}

static void skipRowEndForUnknown(std::ifstream &ifs, std::vector<unsigned char> &skipBuff) {
	skipRowEndForAscii(ifs, skipBuff);
}

// 把文件流ifs定位到换行字节组之后的下一个字节
static void skipRowEnd(std::ifstream &ifs, Charset charset, std::vector<unsigned char> &skipBuff) {
	switch (charset) {
	case Charset::Ascii:
	case Charset::UTF_8:
	case Charset::GB18030:
		skipRowEndForAscii(ifs, skipBuff);
		return;
	default:
		skipRowEndForUnknown(ifs, skipBuff);
		return;
	}
}

static void moveFileStreamPosToAfterNewLine(Charset charset, std::ifstream &ifs, MemBuff &partBuff)
{
	std::vector<unsigned char> skipBuff;
	skipRowEnd(ifs, charset, skipBuff);
	if (!skipBuff.empty()) {
		partBuff.append(skipBuff.data(), skipBuff.size());
	}
}

void DocumentImpl::loadDocument(AsyncComponents &comps)
{
	static const char *title = "DocumentImpl::loadDocument() ";

	ElapsedTime elapsedTime;

	LOGD << title << "start";

	std::ifstream &ifs = comps.ifs();
	MemBuff &buff = comps.buff();
	CharsetDetector &charsetDetector = comps.charsetDetector();

	const uintmax_t partLen = partSize();

	uintmax_t partLenSum = 0;
	
	for (uintmax_t partIndex = 0; ifs; ++partIndex) {

		buff.reverse(partLen);
		ifs.read(reinterpret_cast<char *>(buff.data()), partLen);
		const uintmax_t gcount = ifs.gcount();
		buff.resize(gcount);

		charsetDetector.feed(buff.data(), gcount);
		charsetDetector.end();

		const std::string scharset = charsetDetector.charset();
		const Charset charset = CharsetUtil::strToCharset(scharset);

		moveFileStreamPosToAfterNewLine(charset, ifs, buff);

		LOGD << title << " part(" << partIndex << ") gcount [" << gcount << "], part len: [" << buff.size() << "]";

		partLenSum += buff.size();

		buff.clear();
	}

	LOGD << title << "end, part len sum: [" << partLenSum << "], time usage: " << elapsedTime.milliSec() << "ms";
}

void DocumentImpl::asyncLoadDocument()
{
	auto self(shared_from_this());
	std::async(std::launch::async, [this, self, comps = asyncComponents_] {
		loadDocument(*comps);
		ownerThread_.post([this, self, comps] {
			asyncComponents_ = comps;
		});
	});
}

DocumentImpl::~DocumentImpl()
{

}

void DocumentImpl::start()
{
	asyncLoadDocument();
}

void DocumentImpl::bind(DocumentListener &listener)
{
	listener_ = &listener;
}

void DocumentImpl::unbind()
{
	listener_ = nullptr;
}

bool DocumentImpl::loaded() const
{
	return loaded_;
}

RowN DocumentImpl::loadedRowCount() const
{
	return loadedRowCount_;
}

Charset DocumentImpl::charset() const
{
	return charset_;
}

}
