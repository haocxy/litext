#include "document.h"

#include <future>

#include "core/heap_array.h"
#include "core/system_util.h"
#include "charset_detect_util.h"


namespace doc::detail
{

DocumentImpl::DocumentImpl(const fs::path &path, Worker &ownerThread)
	: path_(path)
	, asyncComponents_(new AsyncComponents(path, path / ".notesharp.db"))
	, ownerThread_(ownerThread)
{
	asyncLoadOnePart();
}

void DocumentImpl::asyncLoadOnePart()
{
	auto self(shared_from_this());
	std::async(std::launch::async, [this, self, comps = asyncComponents_]{
		std::ifstream &ifs = comps->ifs();
		const uintmax_t remain = fs::file_size(path_) - ifs.tellg();
		const uintmax_t readn = std::min(remain, SystemUtil::pageSize() * 1024);
		HeapArray buff(readn);
		if (!ifs.read(buff.data(), buff.size())) {
			throw std::logic_error("TODO: ifs.read() failed"); // TODO
		}
		const std::string scharset = CharsetDetectUtil::detectCharset(buff.data(), buff.size());
		const Charset charset = CharsetUtil::strToCharset(scharset);
		
	});
}

void DocumentImpl::async(std::function<void(AsyncComponents &comps)> &&action, std::function<void()> &&done) {
	if (!asyncComponents_) {
		throw std::logic_error("bad logic, [asyncComponents_] is null");
	}
	if (!action) {
		throw std::logic_error("bad logic, [action] is null");
	}
	if (!done) {
		throw std::logic_error("bad logic, [done] is null");
	}

	auto self(shared_from_this());
	std::async(std::launch::async, [this, self, done = std::move(done), action = std::move(action), comps = asyncComponents_]() mutable {
		if (comps) {
			action(*comps);
			ownerThread_.post([this, self, done = std::move(done), comps]{
				asyncComponents_ = comps;
				done();
			});
		}
	});
}

DocumentImpl::~DocumentImpl()
{

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
