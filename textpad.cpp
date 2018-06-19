#include "textpad.h"

#include <cassert>

#include <QFont>
#include <QFontMetrics>
#include <QDebug>
#include <QPainter>
#include <QInputMethodEvent>
#include <QShowEvent>
#include <QResizeEvent>
#include <QMouseEvent>

#include "composer.h"

namespace
{

    class PainterAutoSaver
    {
    public:
        PainterAutoSaver(QPainter &painter) :m_painterRef(painter)
        {
            m_painterRef.save();
        }
        ~PainterAutoSaver()
        {
            m_painterRef.restore();
        }

    private:
        QPainter & m_painterRef;
    };
}

TextPad::TextPad(DocModel &model, const FontInfoProvider &fontInfo, QWidget *parent)
    : QWidget(parent)
    , m_model(model)
    , m_fontInfo(fontInfo)
{
    setCursor(Qt::IBeamCursor);
    setAttribute(Qt::WA_InputMethodEnabled);
    setFocus();

    m_composer = new Composer(&m_model, &m_fontInfo);
}

TextPad::~TextPad()
{
    delete m_composer;
    m_composer = nullptr;
}

void TextPad::paintBackground(QPainter &p)
{
    PainterAutoSaver painterAutoSaver(p);

    p.fillRect(rect(), QColor(Qt::white));
}

void TextPad::paintRowBackground(QPainter & p)
{
    static const QColor color = QColor(Qt::blue).lighter(190);

    PainterAutoSaver painterAutoSaver(p);

    const DocSel &cursor = m_model.GetCursor();
    const int width = size().width();
    const int lineHeight = m_fontInfo.GetLineHeight();

    const Composer::LineComps &lineComps = m_composer->GetLineComps();
    for (const LineComp &line : lineComps)
    {
        if (line.GetModelIndex() == cursor.GetRow())
        {
            for (const SubLineComp &subLine : line)
            {
                p.fillRect(
                    0,
                    lineHeight * subLine.GetLineIndex(),
                    width,
                    lineHeight,
                    color);
            }
        }
    }

    if (m_model.IsEmpty())
    {
        p.fillRect(0, 0, width, lineHeight, color);
    }
}

namespace
{
    inline int Max(int a, int b)
    {
        return a > b ? a : b;
    }

    template <typename T>
    inline T &GrowBack(std::vector<T> &vec)
    {
        const auto size = vec.size();
        vec.resize(size + 1);
        return vec[size];
    }
}

const SubLineComp *TextPad::GetSubLineByY(int y) const
{
    const int lineHeight = m_fontInfo.GetLineHeight();
    const int fontDes = m_fontInfo.GetFontDescent();

    const Composer::LineComps &lines = m_composer->GetLineComps();

    for (const LineComp &line : lines)
    {
        for (const SubLineComp &subLine : line)
        {
            const int baseline = GetBaseLineByLineInfoIndex(subLine.GetLineIndex());
            const int bottom = baseline + fontDes;
            const int top = bottom - lineHeight;
            if (top <= y && y < bottom)
            {
                return &subLine;
            }
        }
    }

    if (!lines.empty())
    {
        const LineComp &lastLine = lines.back();
        const LineComp::SubLineComps &subLines = lastLine.GetSubLines();

        if (!subLines.empty())
        {
            return &(subLines.back());
        }
    }

    return nullptr;
}

ColIndex TextPad::GetColModelIndexBySubLineAndX(const SubLineComp &subLine, int x) const
{
    for (const CharComp &ch : subLine)
    {
        if (x < ch.GetLeftX() + m_fontInfo.GetCharWidth(ch.GetChar()) / 2)
        {
            return subLine.GetColOffset() + ch.GetColIndex();
        }
    }

    if (subLine.IsLineEnd())
    {
        return subLine.GetColOffset() + subLine.GetColCnt() - 1;
    }
    else
    {
        return subLine.GetColOffset() + subLine.GetColCnt();
    }
}

DocSel TextPad::GetCursorByPoint(int x, int y) const
{
    const SubLineComp *subLine = GetSubLineByY(y);
    if (!subLine)
    {
        return DocSel(0, 0);
    }

    const ColIndex colModelIndex = GetColModelIndexBySubLineAndX(*subLine, x);
    return DocSel(subLine->GetLineModelIndex(), colModelIndex);
}

int TextPad::GetBaseLineByLineInfoIndex(int lineInfoIndex) const
{
    return (1 + lineInfoIndex) * m_fontInfo.GetLineHeight() - m_fontInfo.GetFontDescent();
}

void TextPad::paintInsertCursor(QPainter &p)
{
    static const int kBottomShrink = 2;

    PainterAutoSaver painterAutoSaver(p);

    const DocSel &cursor = m_model.GetCursor();

    const int lineHeight = m_fontInfo.GetLineHeight();

    const Composer::LineComps &lineComps = m_composer->GetLineComps();

    for (const LineComp &lineComp : lineComps)
    {
        int colIndex = 0;
        for (const SubLineComp &subLine : lineComp)
        {
            for (const CharComp &ch : subLine)
            {
                if (lineComp.GetModelIndex() == cursor.GetRow() && (colIndex++) == cursor.GetCol())
                {
                    const int x = ch.GetLeftX();
                    const int y1 = lineHeight * subLine.GetLineIndex();
                    p.drawLine(x, y1, x, y1 + lineHeight - kBottomShrink);
                    return;
                }
            }
        }
    }

    if (m_model.IsEmpty())
    {
        const int leftGap = m_fontInfo.GetDrawConfig().GetLeftGap();
        p.drawLine(leftGap, 0, leftGap, lineHeight - kBottomShrink);
    }
}

void TextPad::paintTextContent(QPainter &p)
{
    PainterAutoSaver painterAutoSaver(p);

    p.setFont(m_fontInfo.GetDrawConfig().GetFont());

    const Composer::LineComps &lineComps = m_composer->GetLineComps();

    for (const LineComp &lineComp : lineComps)
    {
        for (const SubLineComp &subLine : lineComp)
        {
            const int baseline = GetBaseLineByLineInfoIndex(subLine.GetLineIndex());
            for (const CharComp &ch : subLine)
            {
                p.drawText(ch.GetLeftX(),
                    baseline, QChar(ch.GetChar()));
            }
        }
    }
}

void TextPad::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    paintBackground(p);
    paintRowBackground(p);
    paintTextContent(p);
    paintInsertCursor(p);
}

void TextPad::keyPressEvent(QKeyEvent *e)
{
    const int k = e->key();
    switch (k) {
    case Qt::Key_Up:
        break;
    case Qt::Key_Down:
        break;
    case Qt::Key_Left:
        if (m_model.CursorMovePrevChar())
        {
            update();
        }
        break;
    case Qt::Key_Right:
        if (m_model.CursorMoveNextChar())
        {
            update();
        }
        break;
    case Qt::Key_Escape:
        std::exit(0);
        break;
    default:
        break;
    }
}

void TextPad::inputMethodEvent(QInputMethodEvent *e)
{
    qDebug() << "commit:" << e->commitString();
}

void TextPad::showEvent(QShowEvent * event)
{
    QSize sz(size());
    m_composer->Init(sz.width(), sz.height(), 0);
    QWidget::showEvent(event);
    update();
    //prepareTextContentDrawInfo(width());
}

void TextPad::resizeEvent(QResizeEvent * event)
{
    QSize sz(size());
    m_composer->UpdateAreaSize(sz.width(), sz.height());
    QWidget::resizeEvent(event);
    update();
    //prepareTextContentDrawInfo(event->size().width());
}

void TextPad::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        const DocSel newCursor(GetCursorByPoint(e->x(), e->y()));
        if (newCursor != m_model.GetCursor())
        {
            m_model.SetCursor(newCursor);
            update();
        }
    }
}
