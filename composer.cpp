#include "composer.h"

#include "font_info.h"
#include "docmodel.h"

#include "module/text/impl/ref_content_char_instream.h"
#include "module/text/impl/txt_word_stream.h"

void Composer::DoInit()
{
    m_lines.clear();

    const FontConfig &config = m_fip.GetDrawConfig();
    const bool wrapLine = config.IsWrapLineForShow();
    const int margin = config.GetCharMargin();
    const int leftGap = config.GetLeftGap();
    const RowCnt maxShowLine = m_modelRowIndex + m_areaHeight / m_fip.GetLineHeight() + 1;

    int leftX = 0;

    int lineIndex = 0;

    const RowCnt rowCnt = std::min(maxShowLine, m_model.GetRowCnt());
    for (RowIndex row = m_modelRowIndex; row < rowCnt; ++row)
    {
        leftX = leftGap;

        const ColCnt charCnt = m_model.GetColCntOfLine(row);

        LineComp &lineComp = Grow(row);

        SubLineComp *subLineComp = lineComp.Grow(lineIndex++);

        const QString &line = m_model[row];

        RefContentQCharInStream charStream(line);
        TxtWordStream wordStream(charStream);

        if (wrapLine)
        {
            while (true)
            {
                const QString word = wordStream.Next();
                if (word.isNull())
                {
                    break;
                }

                if (subLineComp->GetColCnt() == 0)
                {
                    for (const QChar c : word)
                    {
                        const int charWidth = m_fip.GetCharWidth(c.unicode());

                        if (leftX + charWidth > m_areaWidth)
                        {
                            leftX = leftGap;
                            subLineComp = lineComp.Grow(lineIndex++);
                        }

                        CharComp &charComp = subLineComp->Grow();
                        charComp.SetChar(c.unicode());
                        charComp.SetLeftX(leftX);
                        charComp.SetCharWidth(charWidth);

                        leftX += charWidth;
                        leftX += margin;
                    }
                }
                else
                {
                    int wordWidth = 0;
                    for (const QChar c : word)
                    {
                        wordWidth += m_fip.GetCharWidth(c.unicode());
                        wordWidth += margin;
                    }
                    if (leftX + wordWidth > m_areaWidth)
                    {
                        leftX = leftGap;
                        subLineComp = lineComp.Grow(lineIndex++);
                    }
                    for (const QChar c : word)
                    {
                        const int charWidth = m_fip.GetCharWidth(c.unicode());

                        if (leftX + charWidth > m_areaWidth)
                        {
                            leftX = leftGap;
                            subLineComp = lineComp.Grow(lineIndex++);
                        }

                        CharComp &charComp = subLineComp->Grow();
                        charComp.SetChar(c.unicode());
                        charComp.SetLeftX(leftX);
                        charComp.SetCharWidth(charWidth);

                        leftX += charWidth;
                        leftX += margin;
                    }
                }
            }
        }
        else
        {
            for (ColIndex col = 0; col < charCnt; ++col)
            {
                const wchar_t ch(m_model[row][col].unicode());

                const int charWidth = m_fip.GetCharWidth(ch);

                if (leftX + charWidth > m_areaWidth)
                {
                    if (wrapLine)
                    {
                        leftX = leftGap;

                        subLineComp = lineComp.Grow(lineIndex++);
                    }
                }

                CharComp &charComp = subLineComp->Grow();
                charComp.SetChar(ch);
                charComp.SetLeftX(leftX);
                charComp.SetCharWidth(charWidth);

                leftX += charWidth;
                leftX += margin;
            }
        }

        subLineComp->SetLineEnd();
    }
}

void Composer::OnlyWidthChange(int oldWidth, int newWidth)
{
    DoInit();
}

void Composer::OnlyHeightChange(int oldHeight, int newHeight)
{
    DoInit();
}

void Composer::BothWithAndHeightChange(int oldWidth, int oldHeight, int newWidth, int newHeight)
{
    DoInit();
}

void Composer::AfterModelRowIndexChange(RowIndex oldIndex, RowIndex newIndex)
{
    DoInit();
}
