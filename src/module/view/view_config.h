#pragma once

#include "font.h"

namespace view
{
    class Config
    {
    public:
        const float kDefaultLineHeightFactor = 1.2f; // 默认行高系数
        static const int kDefaultHGap = 2; // 默认水平字符间距
        static const int kDefaultHMargin = 2; // 默认水平间距
        static const int kDefaultTabSize = 4; // 默认TAB尺寸

    public:
        float lineHeightFactor() const { return m_lineHeightFactor; }
        void setLineHeightFactor(float f) { m_lineHeightFactor = f; }

        int lineHeight() const { return m_lineHeightFactor * m_font.height(); }

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

        int charWidth(UChar c) const;

    private:
        float m_lineHeightFactor = kDefaultLineHeightFactor; // 行高系数，行高 = 行高系数 x 字体高度，结果四舍五入
        int m_hGap = kDefaultHGap; // 水平方向最左侧字符左边的空白
        int m_hMargin = kDefaultHMargin; // 水平字符间距
        int m_tabSize = kDefaultTabSize; // 一个TAB的宽度为若干个空格
        bool m_wrapLine = false;
        
        Font m_font;
    };
}