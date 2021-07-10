#pragma once

#include "core/ustring.h"
#include "core/worker.h"
#include "core/declare_thread_pool.h"
#include "doc_server.h"
#include "row_range.h"


namespace doc
{

// 异步文档服务
//
// 不包含业务逻辑，负责把 '文档服务 DocServer 类' 的异步化
// 把DocServer类的功能进行异步化包装
// 在让DocServer的逻辑在不同于当前线程的单一线程中异步地执行
//
// 注意！
// 异步执行完成后，外部传入的回调函数对象会在执行DocServer逻辑的线程中执行，
// 需要由外部把回调函数对象投递到正确的线程中执行。
// 这是因为AsyncDocServer无法假设被外部逻辑从什么线程调用，甚至可能是多个不同的线程。
class AsyncDocServer {
public:
	AsyncDocServer(const fs::path &filePath);

	~AsyncDocServer();

	void loadRows(const doc::RowRange &range, std::function<void(std::vector<UString> &&rows)> &&cb);

private:
	DocServer docServer_;
	ThreadPool *docServerWorker_ = nullptr;
};


}
