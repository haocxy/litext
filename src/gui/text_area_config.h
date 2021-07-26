#pragma once

#include "core/uchar.h"
#include "core/font_old.h"
#include "core/font_index.h"

#include "pixel.h"
#include "loc_outside_policy.h"


namespace gui
{


class TextAreaConfig {
public:
	static const int kLineHeightScale = 1000;
	static const int kDefaultLineHeightFactor = static_cast<int>(1.2 * kLineHeightScale); // 默认行高系数
	static const int kDefaultHGap = 2; // 默认水平字符间距
	static const int kDefaultHMargin = 2; // 默认水平间距
	static const int kDefaultTabSize = 4; // 默认TAB尺寸

public:
	float lineHeightFactor() const { return lineHeightFactor_ / 1000.0; }
	void setLineHeightFactor(float f) { lineHeightFactor_ = static_cast<int>(f * 1000); }

	Pixel::Raw lineHeight() const { return lineHeightFactor_ * font_.height() / 1000; }

	Pixel::Raw hGap() const { return hGap_; }
	void setHGap(Pixel::Raw hGap) { hGap_ = hGap; }

	Pixel::Raw hMargin() const { return hMargin_; }
	void setHMargin(Pixel::Raw hMargin) { hMargin_ = hMargin; }

	int tabSize() const { return tabSize_; }
	void setTabSize(int tabSize) { tabSize_ = tabSize; }

	bool wrapLine() const { return wrapLine_; }
	void setWrapLine(bool wrapLine) { wrapLine_ = wrapLine; }

	const FontOld &font() const { return font_; }
    void setFont(const FontOld &font);

    const font::FontIndex &fontIndex() const { return fontIndex_; }
    void setFontIndex(const font::FontIndex &fontIndex) { fontIndex_ = fontIndex; }

	Pixel::Raw charWidth(UChar c) const;

	bool showLineNum() const { return showLineNum_; }
	void setShowLineNum(bool showLineNum) { showLineNum_ = showLineNum; }

	int32_t lineNumOffset() const { return lineNumOffset_; }
	void setLineNumOffset(int32_t lineNumOffset) { lineNumOffset_ = lineNumOffset; }

	LocOutsidePolicy locateOutsideOfViewPolicy() const { return locateOutsideOfViewPolicy_; }
	void setLocateOutsideOfViewPolicy(LocOutsidePolicy policy) { locateOutsideOfViewPolicy_ = policy; }

private:
	int lineHeightFactor_ = kDefaultLineHeightFactor; // 行高系数，行高 = 行高系数 * 字体高度 / 1000
	Pixel::Raw hGap_{ kDefaultHGap }; // 水平方向最左侧字符左边的空白
	Pixel::Raw hMargin_{ kDefaultHMargin }; // 水平字符间距
	int tabSize_ = kDefaultTabSize; // 一个TAB的宽度为若干个空格
	bool wrapLine_ = false;
	bool showLineNum_ = false; // 是否显示行号
	int32_t lineNumOffset_ = 0; // 行号偏移，显示行号时，把程序内部从0开始的行索引加上这个值
	LocOutsidePolicy locateOutsideOfViewPolicy_ = LocOutsidePolicy::MoveView;

    FontOld font_;
    font::FontIndex fontIndex_;
    bool isFixWidth_ = false;
    int widthForFix_ = 0;
};


}
