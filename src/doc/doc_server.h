#pragma once

#include <vector>

#include "core/fs.h"
#include "core/ustring.h"
#include "row_range.h"
#include "charset.h"
#include "declare_doc_cutter_by_row.h"


namespace doc
{

// 文档服务
// 负责和文档有关的功能
// 该类不考虑线程情况，线程处理由AsyncDocServer类负责
// 该类在实现时以该类对象被封闭在单一线程中为前提
class DocServer {
public:
	DocServer(const fs::path &filePath);

	~DocServer();

	// 检测字符集
	// 检测字符集的速度在特定机器中是常量级的时间复杂度，所以只需要一个单独的函数
	void detectCharset();

	Charset charset() const {
		return charset_;
	}

	// 开始检测行数
	// 检测行数的时间复杂度取决于文档大小，所以在设计函数时要分为若干体现不同阶段的函数
	void startDetectRowCount();

	std::vector<UString> loadRows(const RowRange &range);

	RowN rowCount();

private:
	const fs::path filePath_;
	Charset charset_ = Charset::Unknown;
	DocCutterByRow *cutter_ = nullptr;
};


}
