#include "document.h"

#include <future>



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
	async([](AsyncComponents &comps) {
		
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
