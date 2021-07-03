#pragma once

#include "util/font.h"
#include "loc_outside_policy.h"


namespace view
{


class Config {
public:
	static const int kLineHeightScale = 1000;
	static const int kDefaultLineHeightFactor = static_cast<int>(1.2 * kLineHeightScale); // 默认行高系数
	static const int kDefaultHGap = 2; // 默认水平字符间距
	static const int kDefaultHMargin = 2; // 默认水平间距
	static const int kDefaultTabSize = 4; // 默认TAB尺寸

public:
	float lineHeightFactor() const { return lineHeightFactor_ / 1000.0; }
	void setLineHeightFactor(float f) { lineHeightFactor_ = static_cast<int>(f * 1000); }

	int lineHeight() const { return lineHeightFactor_ * m_font.height() / 1000; }

	int hGap() const { return m_hGap; }
	void setHGap(int hGap) { m_hGap = hGap; }

	int hMargin() const { return m_hMargin; }
	void setHMargin(int hMargin) { m_hMargin = hMargin; }

	int tabSize() const { return m_tabSize; }
	void setTabSize(int tabSize) { m_tabSize = tabSize; }

	bool wrapLine() const { return m_wrapLine; }
	void setWrapLine(bool wrapLine) { m_wrapLine = wrapLine; }

	Font &rfont() { return m_font; }
	const Font &font() const { return m_font; }

	Font &rStatusBarFont() { return m_statusBarFont; }
	const Font &statusBarFont() const { return m_statusBarFont; }

	int charWidth(QChar c) const;

	bool showLineNum() const { return m_showLineNum; }
	void setShowLineNum(bool showLineNum) { m_showLineNum = showLineNum; }

	int32_t lineNumOffset() const { return m_lineNumOffset; }
	void setLineNumOffset(int32_t lineNumOffset) { m_lineNumOffset = lineNumOffset; }

	LocOutsidePolicy locateOutsideOfViewPolicy() const { return m_locateOutsideOfViewPolicy; }
	void setLocateOutsideOfViewPolicy(LocOutsidePolicy policy) { m_locateOutsideOfViewPolicy = policy; }

private:
	int lineHeightFactor_ = kDefaultLineHeightFactor; // 行高系数，行高 = 行高系数 * 字体高度 / 1000
	int m_hGap = kDefaultHGap; // 水平方向最左侧字符左边的空白
	int m_hMargin = kDefaultHMargin; // 水平字符间距
	int m_tabSize = kDefaultTabSize; // 一个TAB的宽度为若干个空格
	bool m_wrapLine = false;
	bool m_showLineNum = false; // 是否显示行号
	int32_t m_lineNumOffset = 0; // 行号偏移，显示行号时，把程序内部从0开始的行索引加上这个值
	LocOutsidePolicy m_locateOutsideOfViewPolicy = LocOutsidePolicy::MoveView;

	Font m_font;
	Font m_statusBarFont; // 状态栏字体
};


}
