#include "document_impl.h"

#include <iostream>
#include <future>

#include "core/logger.h"
#include "core/heap_array.h"
#include "core/system_util.h"


namespace doc::detail
{

DocumentImpl::DocumentImpl(const fs::path &path, Worker &ownerThread)
	: path_(path)
	, asyncComponents_(new AsyncComponents(path))
	, db_(path.generic_string() + ".notesharp.db")
	, ownerThread_(ownerThread)
{
}

static uintmax_t partSize() {
	return SystemUtil::pageSize() * 1024;
}

static void moveFileStreamPosToAfterNewLine(Charset charset, std::ifstream &ifs, std::vector<unsigned char> &buff) {
	if (buff.size() != partSize()) {
		throw std::logic_error("buff.size() should equal to partSize() when call moveFileStreamPosToAfterNewLine()");
	}

}

void DocumentImpl::loadDocument(AsyncComponents &comps)
{
	LOGD << "loadDocument()";

	std::ifstream &ifs = comps.ifs();
	std::vector<unsigned char> &buff = comps.buff();
	CharsetDetector &charsetDetector = comps.charsetDetector();

	if (!buff.empty()) {
		throw std::logic_error("buff should be empty before handle file part");
	}

	buff.resize(partSize());
	ifs.read(reinterpret_cast<char *>(buff.data()), buff.size());
	const uintmax_t gcount = ifs.gcount();
	if (gcount == 0) {
		return;
	}

	charsetDetector.feed(buff.data(), gcount);
	charsetDetector.end();

	const std::string scharset = charsetDetector.charset();
	const Charset charset = CharsetUtil::strToCharset(scharset);

	moveFileStreamPosToAfterNewLine(charset, ifs, buff);

	buff.clear();
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
