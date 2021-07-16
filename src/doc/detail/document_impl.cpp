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

static void moveFileStreamPosToAfterNewLine(Charset charset, std::ifstream &ifs, MemBuff &buff)
{
	
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
	
	for (uintmax_t partIndex = 0; ifs; ++partIndex) {

		buff.resize(partLen);
		ifs.read(reinterpret_cast<char *>(buff.data()), buff.size());
		const uintmax_t gcount = ifs.gcount();

		LOGD << title << " part(" << partIndex << ") gcount [" << gcount << "]";

		charsetDetector.feed(buff.data(), gcount);
		charsetDetector.end();

		const std::string scharset = charsetDetector.charset();
		const Charset charset = CharsetUtil::strToCharset(scharset);

		moveFileStreamPosToAfterNewLine(charset, ifs, buff);

		buff.clear();
	}

	LOGD << title << "end, time usage: " << elapsedTime.milliSec() << "ms";
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
