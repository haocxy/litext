#pragma once

#include "core/fs.h"
#include "core/ustring.h"
#include "row_range.h"


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

	std::vector<UString> loadRows(const RowRange &range);

	RowN rowCount();

private:
	const fs::path filePath_;

};


}
