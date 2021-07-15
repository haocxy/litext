#include "document.h"

#include <future>

#include "core/heap_array.h"
#include "core/system_util.h"
#include "charset_detect_util.h"


namespace doc::detail
{

DocumentImpl::DocumentImpl(const fs::path &path, Worker &ownerThread)
	: path_(path)
	, asyncComponents_(new AsyncComponents(path))
	, db_(path / ".notesharp.db")
	, ownerThread_(ownerThread)
{
	asyncLoadOnePart();
}

static uintmax_t partSize() {
	return SystemUtil::pageSize() * 1024;
}

void DocumentImpl::asyncLoadOnePart()
{
	auto self(shared_from_this());
	std::async(std::launch::async, [this, self, comps = asyncComponents_]{
		DirectFileReader &reader = comps->reader();
		const uintmax_t remain = fs::file_size(path_) - reader.tell();
		const uintmax_t readn = std::min(remain, partSize());
		HeapArray buff(partSize());
		size_t readCount = reader.read(buff.data(), buff.size());
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
