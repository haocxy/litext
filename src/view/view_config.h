#pragma once

#include "font.h"

namespace view
{
    class Config
    {
    public:
        const float kDefaultLineHeightFactor = 1.2f; // Ĭ���и�ϵ��
        static const int kDefaultHGap = 2; // Ĭ��ˮƽ�ַ����
        static const int kDefaultHMargin = 2; // Ĭ��ˮƽ���
        static const int kDefaultTabSize = 4; // Ĭ��TAB�ߴ�

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

        bool showLineNum() const { return m_showLineNum; }
        void setShowLineNum(bool showLineNum) { m_showLineNum = showLineNum; }

        int32_t lineNumOffset() const { return m_lineNumOffset; }
        void setLineNumOffset(int32_t lineNumOffset) { m_lineNumOffset = lineNumOffset; }

    private:
        float m_lineHeightFactor = kDefaultLineHeightFactor; // �и�ϵ�����и� = �и�ϵ�� x ����߶ȣ������������
        int m_hGap = kDefaultHGap; // ˮƽ����������ַ���ߵĿհ�
        int m_hMargin = kDefaultHMargin; // ˮƽ�ַ����
        int m_tabSize = kDefaultTabSize; // һ��TAB�Ŀ��Ϊ���ɸ��ո�
        bool m_wrapLine = false;
        bool m_showLineNum = false; // �Ƿ���ʾ�к�
        int32_t m_lineNumOffset = 0; // �к�ƫ�ƣ���ʾ�к�ʱ���ѳ����ڲ���0��ʼ���������������ֵ
        
        Font m_font;
    };
}