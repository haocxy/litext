#pragma once

#include "core/fs.h"
#include "core/ustring.h"
#include "core/callbacks.h"
#include "core/thread_pool.h"
#include "core/declare_thread_pool.h"
#include "declare_doc_server.h"
#include "charset.h"
#include "row_range.h"


namespace doc
{

// 异步文档服务
//
// 不包含业务逻辑，负责把 '文档服务 DocServer 类' 的异步化
// 把DocServer类的功能进行异步化包装
// 在让DocServer的逻辑在不同于当前线程的单一线程中异步地执行，
// 执行完成后在由构造函数的参数cbWorker制定的回调线程中执行
class AsyncDocServer {
public:
	AsyncDocServer(Worker &cbWorker, const fs::path &filePath);

	~AsyncDocServer();

	void loadRows(const doc::RowRange &range, std::function<void(std::vector<UString> &&rows)> &&cb);

	void queryRowCount(std::function<void(RowN rowCount)> &&cb);

	CallbackHandle addCharsetDetectedCallback(std::function<void(Charset charset)> &&action);

private:
	void detectCharset();

private:
	ThreadPool docServerThread_{ 1 };
	Callbacks<void(Charset charset)> cbsCharsetDetected_;
	Worker &cbWorker_;
	DocServer *docServer_ = nullptr;
};


}
