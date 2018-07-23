#include <cassert>
#include "view.h"
#include "view_config.h"
#include "module/model/model.h"
#include "module/model/line.h"
#include "module/text/impl/doc_line_char_instream.h"
#include "module/text/impl/txt_word_stream2.h"

void View::Init(LineN viewStart, const view::Size & size)
{
    m_viewStart = viewStart;
    m_size = size;

    remakePage();
}

const view::Page & View::page() const
{
    return m_page;
}

int View::getBaseLineByLineOffset(int off) const
{
    assert(m_config != nullptr);

    return (1 + off) * m_config->lineHeight() - m_config->font().descent();
}

void View::remakePage()
{
    assert(m_config != nullptr);
    assert(m_model != nullptr);

    m_page.clear();

    const LineN maxShowLine = m_viewStart + m_size.height() / m_config->lineHeight() + 1;

    const LineN rowCnt = std::min(maxShowLine, m_model->lineCnt());

    for (LineN i = m_viewStart; i < rowCnt; ++i)
    {
        view::Phase &vphase = m_page.grow();

        DocLineToViewPhase(m_model->lineAt(i), vphase);
    }

}

void View::DocLineToViewPhase(const Line& line, view::Phase & phase)
{
    assert(m_config != nullptr);

    if (m_config->wrapLine())
    {
        DocLineToViewPhaseWithWrapLine(line, phase);
    }
    else
    {
        DocLineToViewPhaseNoWrapLine(line, phase);
    }
}

void View::DocLineToViewPhaseWithWrapLine(const Line& line, view::Phase & phase)
{
    assert(m_config != nullptr);
    assert(phase.size() == 0);

    const int hGap = m_config->hGap();
    const int hMargin = m_config->hMargin();

    view::Line *vline = &phase.grow();

    DocLineCharInStream charStream(line);
    TxtWordStream2 wordStream(charStream);

    int leftX = hGap;

    while (true)
    {
        const UString word = wordStream.Next();
        if (check::isNull(word))
        {
            return;
        }

        if (vline->size() == 0)
        {
            for (const UChar c : word)
            {
                const int charWidth = m_config->charWidth(c);

                if (leftX + charWidth > m_size.width())
                {
                    leftX = hGap;
                    vline = &phase.grow();
                }

                view::Char &vc = vline->grow();
                vc.setUnicode(c);
                vc.setX(leftX);
                vc.setWidth(charWidth);

                leftX += charWidth;
                leftX += hMargin;
            }
        }
        else
        {
            int wordWidth = 0;
            for (const UChar c : word)
            {
                wordWidth += m_config->charWidth(c);
                wordWidth += hMargin;
            }
            if (leftX + wordWidth > m_size.width())
            {
                leftX = hGap;
                vline = &phase.grow();
            }
            for (const UChar c : word)
            {
                const int charWidth = m_config->charWidth(c);

                if (leftX + charWidth > m_size.width())
                {
                    leftX = hGap;
                    vline = &phase.grow();
                }

                view::Char &vc = vline->grow();
                vc.setUnicode(c);
                vc.setX(leftX);
                vc.setWidth(charWidth);

                leftX += charWidth;
                leftX += hMargin;
            }
        }
    }
}

void View::DocLineToViewPhaseNoWrapLine(const Line& line, view::Phase & phase)
{
    assert(m_config != nullptr);
    assert(phase.size() == 0);

    const int hGap = m_config->hGap();
    const int hMargin = m_config->hMargin();

    view::Line &vline = phase.grow();

    int leftX = hGap;

    const CharN cnt = line.charCnt();
    for (CharN i = 0; i < cnt; ++i)
    {
        const UChar c = line.charAt(i);
        const int charWidth = m_config->charWidth(c);

        view::Char &vchar = vline.grow();
        vchar.setUnicode(c);
        vchar.setX(leftX);
        vchar.setWidth(charWidth);

        leftX += charWidth;
        leftX += hMargin;
    }
}
