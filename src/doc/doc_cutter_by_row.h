#pragma once

#include <cstdint>
#include <string>
#include <fstream>

#include "core/fs.h"
#include "row_range.h"


namespace doc
{


// 根据行对文档切分
class DocCutterByRow {
public:

	// filePath: 文件路径
	// charset: 文件字符集
	// partSizeHint: 建议的分段大小，实际分段的结果会尽可能接近这个值
	DocCutterByRow(const fs::path &filePath, const std::string &charset, uintmax_t partSizeHint);

	// 是否有下一段
	bool hasNext() const {
		return offset_ < bytecount_;
	}

	// 取下一段，如果没有下一段则返回空对象，可直接用布尔表达式检测返回的对象是否为空对象
	RowRange next();

private:
	const fs::path filePath_;
	const std::string charset_;
	const uintmax_t partSizeHint_;
	const uintmax_t bytecount_;
	std::fstream ifs_;
	uintmax_t offset_ = 0;
};


}
