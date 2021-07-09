#pragma once

#include "core/fs.h"


namespace doc
{

// 文档服务
// 负责和文档有关的功能
// 该类不考虑线程情况，线程处理由AsyncDocServer类负责
class DocServer {
public:
	DocServer(const fs::path &filePath);

	~DocServer();

private:
	const fs::path filePath_;

};


}
