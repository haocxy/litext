#include "async_doc_server.h"

#include "core/block_door.h"
#include "doc_server.h"


namespace doc
{


AsyncDocServer::AsyncDocServer(Worker &cbWorker, const fs::path &filePath)
	: cbWorker_(cbWorker) {

	docServerThread_.post([this, filePath] {
		docServer_ = new DocServer(filePath);
	});
}

AsyncDocServer::~AsyncDocServer()
{
	// 确保docServerWorker_在其所属线程内被析构完成后才析构本对象
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


}
