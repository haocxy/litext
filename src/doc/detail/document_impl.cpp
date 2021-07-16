#include "document_impl.h"

#include <future>

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

DocumentImpl::LoadOnePartResult DocumentImpl::doLoadOnePartSync(AsyncComponents &comps)
{
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
		return LoadOnePartResult::FileEnd;
	}

	charsetDetector.feed(buff.data(), gcount);
	charsetDetector.end();

	const std::string scharset = charsetDetector.charset();
	const Charset charset = CharsetUtil::strToCharset(scharset);

	moveFileStreamPosToAfterNewLine(charset, ifs, buff);

	buff.clear();
	return LoadOnePartResult::FileNotEnd;
}

void DocumentImpl::asyncLoadOnePart()
{
	auto self(shared_from_this());
	std::async(std::launch::async, [this, self, comps = asyncComponents_] {
		doLoadOnePartSync(*comps);
		ownerThread_.post([this, self, comps] {
			asyncComponents_ = comps;
			});
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

void DocumentImpl::start()
{
	asyncLoadOnePart();
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
