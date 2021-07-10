#include "async_doc_server.h"

#include "core/block_door.h"
#include "doc_server.h"


namespace doc
{


AsyncDocServer::AsyncDocServer(Worker &cbWorker, const fs::path &filePath)
	: cbWorker_(cbWorker) {

	docServerThread_.post([this, filePath] {
		docServer_ = new DocServer(filePath);
		detectCharset();
	});
}

AsyncDocServer::~AsyncDocServer()
{
	BlockDoor door;

	docServerThread_.post([this, &door] {
		delete docServer_;
		docServer_ = nullptr;
		door.open();
	});

	door.waitOpened();
}

void AsyncDocServer::loadRows(const doc::RowRange &range, std::function<void(std::vector<UString> &&rows)> &&cb)
{
	docServerThread_.post([this, range, cb = std::move(cb)]() mutable {
		std::vector<UString> rows = docServer_->loadRows(range);
		cbWorker_.post([rows = std::move(rows), cb = std::move(cb)]() mutable {
			cb(std::move(rows));
		});
	});
}

void AsyncDocServer::queryRowCount(std::function<void(RowN rowCount)> &&cb)
{
	docServerThread_.post([this, cb = std::move(cb)]() mutable {
		RowN rowCount = docServer_->rowCount();
		cbWorker_.post([rowCount, cb = std::move(cb)]{
			cb(rowCount);
		});
	});
}

CallbackHandle AsyncDocServer::addCharsetDetectedCallback(std::function<void(Charset charset)> &&action)
{
	return cbsCharsetDetected_.add(std::move(action));
}

void AsyncDocServer::detectCharset()
{
	docServerThread_.post([this] {
		docServer_->detectCharset();
		const Charset charset = docServer_->charset();
		cbWorker_.post([this, charset]() {
			cbsCharsetDetected_.call(charset);
		});
	});
}


}
