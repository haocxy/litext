#pragma once

#include "core/worker.h"
#include "core/declare_thread_pool.h"
#include "doc_server.h"


namespace doc
{


class AsyncDocServer {
public:
	AsyncDocServer(const fs::path &filePath, Worker &callbackWorker);

	~AsyncDocServer();

private:
	DocServer docServer_;
	Worker &callbackWorker_;
	ThreadPool *docServerWorker_ = nullptr;
};


}
