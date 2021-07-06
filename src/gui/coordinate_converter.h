#pragma once

#include "size.h"
#include "page.h"
#include "view_loc.h"


namespace gui
{

// 把不同坐标系中的位置相互转换
// 包括的坐标系：文档坐标、视图坐标、GUI像素坐标
// 这部分逻辑属于TextArea，但是因为类似函数太多且实现代码较长，所以单独放置
class CoordinateConverter {
public:
	CoordinateConverter(const Size &size, const Page &page, const ViewLoc &vloc)
		: size_(size)
		, page_(page)
		, vloc_(vloc) {
	}

	CoordinateConverter() = delete;
	CoordinateConverter(const CoordinateConverter &) = delete;
	CoordinateConverter(CoordinateConverter &&) = delete;
	CoordinateConverter &operator=(const CoordinateConverter &) = delete;
	CoordinateConverter &operator=(CoordinateConverter &&) = delete;

	// 视图内某段落vRowLoc前面的行数
	LineN lineOffset(const VRowLoc &vRowLoc) const;

private:
	const Size &size_;
	const Page &page_;
	const ViewLoc &vloc_;
};


}
