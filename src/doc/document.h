#pragma once

#include "core/fs.h"
#include "core/worker.h"

#include "async_doc_server.h"


namespace doc
{

class Document {
public:
	Document(const fs::path &file, Worker &ownerThread);

	~Document();

private:
	const fs::path file_;
	Worker &ownerThread_;
	AsyncDocServer server_;
};

}
