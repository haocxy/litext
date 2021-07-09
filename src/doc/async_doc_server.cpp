#include "async_doc_server.h"

#include "core/thread_pool.h"


namespace doc
{


AsyncDocServer::AsyncDocServer(const fs::path &filePath, Worker &callbackWorker)
	: docServer_(filePath)
	, callbackWorker_(callbackWorker) {

	docServerWorker_ = new ThreadPool(1);
}

AsyncDocServer::~AsyncDocServer()
{
	delete docServerWorker_;
	docServerWorker_ = nullptr;
}


}
