#pragma once

#include "doc/doc_loc.h"
#include "editor/declare_editor.h"
#include "doc/page.h"
#include "doc/view_loc.h"
#include "doc/line_loc.h"
#include "doc/char_loc.h"
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
		const doc::Page &page,
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

	LineOffset::Raw toLineOffset(const doc::VRowLoc &vRowLoc) const;

	LineOffset::Raw toLineOffset(const doc::VLineLoc &vLineLoc) const;

	LineOffset::Raw toLineOffset(Pixel y) const;

	Pixel::Raw toX(const doc::VCharLoc &charLoc) const;

	Pixel::Raw toBaselineY(LineOffset lineOffset) const;

    doc::VRowLoc toRowLoc(VRowOffset vRowOffset) const;

    doc::VLineLoc toVLineLoc(LineOffset lineOffset) const;

    doc::VCharLoc toCharLoc(const doc::VLineLoc &vLineLoc, Pixel x) const;

    doc::VCharLoc toCharLoc(Pixel x, Pixel y) const;

    doc::VCharLoc toCharLoc(const DocLoc &docLoc) const;

	DocLoc toDocLoc(const doc::VCharLoc &vCharLoc) const;

	DocLoc toDocLoc(Pixel x, Pixel y) const;

private:
	bool isLastLineOfRow(const doc::VLineLoc &lineLoc) const;

private:
	const Editor &editor_;
	const Size &size_;
	const doc::Page &page_;
	const ViewLoc &vloc_;
	const TextAreaConfig &config_;
};


}
