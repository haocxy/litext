#include "async_doc_server.h"

#include "core/thread_pool.h"


namespace doc
{


AsyncDocServer::AsyncDocServer(Worker &cbWorker, const fs::path &filePath)
	: cbWorker_(cbWorker)
	, docServer_(filePath) {

	docServerWorker_ = new ThreadPool(1);
}

AsyncDocServer::~AsyncDocServer()
{
	delete docServerWorker_;
	docServerWorker_ = nullptr;
}

void AsyncDocServer::loadRows(const doc::RowRange &range, std::function<void(std::vector<UString> &&rows)> &&cb)
{
	docServerWorker_->post([this, range, cb = std::move(cb)]() mutable {
		std::vector<UString> rows = docServer_.loadRows(range);
		cbWorker_.post([rows = std::move(rows), cb = std::move(cb)]() mutable {
			cb(std::move(rows));
		});
	});
}

void AsyncDocServer::queryRowCount(std::function<void(RowN rowCount)> &&cb)
{
	docServerWorker_->post([this, cb = std::move(cb)]() mutable {
		RowN rowCount = docServer_.rowCount();
		cbWorker_.post([rowCount, cb = std::move(cb)]{
			cb(rowCount);
		});
	});
}


}
