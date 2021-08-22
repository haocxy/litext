#pragma once

#include "doc/doc_loc.h"
#include "editor/declare_editor.h"
#include "page.h"
#include "view_locs.h"
#include "row_offset.h"
#include "line_offset.h"
#include "pixel.h"
#include "size.h"
#include "declare_text_area_config.h"


namespace gui
{

// 把不同坐标系中的位置相互转换
// 包括的坐标系：文档坐标、视图坐标、GUI像素坐标
// 这部分逻辑属于TextArea，但是因为类似函数太多且实现代码较长，所以单独放置
class CoordinateConverter {
public:
	CoordinateConverter(
		const Editor &editor,
		const Size &size,
		const Page &page,
		const ViewLoc &vloc,
		const TextAreaConfig &config)
		: editor_(editor)
		, size_(size)
		, page_(page)
		, vloc_(vloc)
		, config_(config) {
	}

	CoordinateConverter() = delete;
	CoordinateConverter(const CoordinateConverter &) = delete;
	CoordinateConverter(CoordinateConverter &&) = delete;
	CoordinateConverter &operator=(const CoordinateConverter &) = delete;
	CoordinateConverter &operator=(CoordinateConverter &&) = delete;

	LineOffset::Raw toLineOffset(const VRowLoc &vRowLoc) const;

	LineOffset::Raw toLineOffset(const VLineLoc &vLineLoc) const;

	LineOffset::Raw toLineOffset(Pixel y) const;

	Pixel::Raw toX(const VCharLoc &charLoc) const;

	Pixel::Raw toBaselineY(LineOffset lineOffset) const;

    VRowLoc toRowLoc(VRowOffset vRowOffset) const;

    VLineLoc toVLineLoc(LineOffset lineOffset) const;

    VCharLoc toCharLoc(const VLineLoc &vLineLoc, Pixel x) const;

    VCharLoc toCharLoc(Pixel x, Pixel y) const;

    VCharLoc toCharLoc(const DocLoc &docLoc) const;

	DocLoc toDocLoc(const VCharLoc &vCharLoc) const;

	DocLoc toDocLoc(Pixel x, Pixel y) const;

private:
	bool isLastLineOfRow(const VLineLoc &lineLoc) const;

private:
	const Editor &editor_;
	const Size &size_;
	const Page &page_;
	const ViewLoc &vloc_;
	const TextAreaConfig &config_;
};


}
