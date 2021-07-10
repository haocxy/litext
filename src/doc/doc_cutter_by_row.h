#pragma once

#include <cstdint>
#include <string>
#include <fstream>
#include <optional>

#include "core/fs.h"
#include "offset_range.h"
#include "charset.h"


namespace doc
{


// 根据行对文档切分
// 切分结果保证落在行边界
class DocCutterByRow {
public:

	// filePath: 文件路径
	// charset: 文件字符集
	// partSizeHint: 建议的分段大小，实际分段的结果会尽可能接近这个值
	DocCutterByRow(const fs::path &filePath, Charset charset, uintmax_t partSizeHint);

	// 取下一段
	std::optional<OffsetRange> next();

private:
	const fs::path filePath_;
	const Charset charset_;
	const uintmax_t partSizeHint_;
	const uintmax_t fileSize_;
	std::ifstream ifs_;
	uintmax_t offset_ = 0;
};


}
