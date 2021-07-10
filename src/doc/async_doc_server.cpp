#include "async_doc_server.h"

#include "core/thread_pool.h"


namespace doc
{


AsyncDocServer::AsyncDocServer(const fs::path &filePath)
	: docServer_(filePath) {

	docServerWorker_ = new ThreadPool(1);
}

AsyncDocServer::~AsyncDocServer()
{
	delete docServerWorker_;
	docServerWorker_ = nullptr;
}

void AsyncDocServer::loadRows(const doc::RowRange &range, std::function<void(std::vector<UString> &&rows)> &&cb)
{
	docServerWorker_->post([this, range, cb = std::move(cb)]{
		cb(docServer_.loadRows(range));
	});
}


}
