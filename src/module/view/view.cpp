#include <cassert>
#include "view.h"
#include "view_config.h"
#include "module/model/model.h"
#include "module/model/line.h"

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
