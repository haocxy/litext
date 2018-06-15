#include "textpad.h"

#include <QFont>
#include <QFontMetrics>
#include <QDebug>
#include <QPainter>
#include <QInputMethodEvent>
#include <QShowEvent>
#include <QResizeEvent>
#include <QMouseEvent>

namespace
{
    const char *kFontFamilyTimes = "Times";
    const char *kFontFamilyYaHei = "Microsoft YaHei";

    const char *kFontFamily = kFontFamilyYaHei;
    const int kFontSize = 30;

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

TextPad::TextPad(DocModel &model, QWidget *parent)
    : m_font(kFontFamily, kFontSize)
    , m_fontMetrix(m_font)
    , m_model(model)
    , QWidget(parent)
{
    // 等宽："Times"
    // 非等宽："Microsoft YaHei"

    setCursor(Qt::IBeamCursor);
    setAttribute(Qt::WA_InputMethodEnabled);
    setFocus();
}

bool TextPad::IsFixWidthFont() const
{
    return m_fontMetrix.width('i') == m_fontMetrix.width('w');
}

TextPad::~TextPad()
{

}

void TextPad::paintBackground(QPainter &p)
{
    PainterAutoSaver painterAutoSaver(p);

    if (hasFocus()) {
        p.setBrush(Qt::white);
    }
    p.drawRect(rect());
}

void TextPad::paintRowBackground(QPainter &p)
{
    PainterAutoSaver painterAutoSaver(p);

    const DocSel &cursor = m_model.GetCursor();

    const int width = rect().width();

    const int lineHeight = GetLineHeight();

    const QColor color = QColor(Qt::blue).lighter(190);

    bool cursorLineDrawed = false;

    const int fontDes = m_fontMetrix.descent();

    const int lineInfoCnt = static_cast<int>(m_drawInfo.lineInfos.size());

    for (int i = 0; i < lineInfoCnt; ++i)
    {
        const LineDrawInfo &lineDrawInfo = m_drawInfo.lineInfos[i];
        if (lineDrawInfo.rowModelIndex == cursor.GetRow())
        {
            const int y = GetBaseLineByLineInfoIndex(i) + fontDes - lineHeight;

            p.fillRect(0, y, width, lineHeight, color);
            cursorLineDrawed = true;
        }
    }

    if (cursorLineDrawed)
    {
        return;
    }

    p.fillRect(0, 0, width, lineHeight, color);
}

static int kFontMargin = 1;
static int kLeftGap = 2;

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

void TextPad::prepareTextContentDrawInfo(int areaWidth)
{
    m_drawInfo.lineInfos.clear();
    
    const bool isFixWidthFont = IsFixWidthFont();
    const int widthForFix = m_fontMetrix.width('a');

    int leftX = 0;

    const RowCnt rowCnt = m_model.GetRowCnt();

    for (RowIndex row = 0; row < rowCnt; ++row)
    {
        leftX = kLeftGap;
        const ColCnt charCnt = m_model.GetColCntOfLine(row);

        LineDrawInfo *lineDrawInfo = &GrowBack(m_drawInfo.lineInfos);
        lineDrawInfo->rowModelIndex = row;

        for (ColIndex col = 0; col < charCnt; ++col)
        {
            const QChar ch(m_model[row][col]);

            // 从字体引擎获得的原始字符宽度
            const int rawFontWidth = m_fontMetrix.width(ch);
            
            // 判断这个字符是否是宽字符
            const bool isWideChar = rawFontWidth > widthForFix;
            
            // 如果当前字体是等宽字体且这个字符是宽字符，则绘制时作为两个字符处理
            const bool asTwoChar = isWideChar && isFixWidthFont;

            // 绘制这个字符总共占的宽度，包括两侧空白
            int drawTotalCharWidth = 0;
            if (isWideChar)
            {
                if (isFixWidthFont)
                {
                    drawTotalCharWidth = widthForFix * 2 + kFontMargin * 2;
                }
                else
                {
                    drawTotalCharWidth = rawFontWidth + kFontMargin;
                }
            }
            else
            {
                drawTotalCharWidth = rawFontWidth + kFontMargin;
            }

            // 如果超出最大宽度
            if (leftX + drawTotalCharWidth > areaWidth)
            {
                // 如果开启了自动换行则把绘制位置定位到下一行开头
                // 如果未开启自动换行，则跳过改行后面的绘制，开始绘制下一行
                if (m_wrapLine)
                {
                    leftX = kLeftGap;

                    lineDrawInfo = &GrowBack(m_drawInfo.lineInfos);
                    lineDrawInfo->colOffset = col;
                    lineDrawInfo->rowModelIndex = row;
                }
            }

            CharDrawInfo &charDrawInfo = GrowBack(lineDrawInfo->charInfos);
            charDrawInfo.ch = ch.unicode();
            charDrawInfo.drawLeftX = leftX + kFontMargin;
            charDrawInfo.drawTotalWidth = drawTotalCharWidth;
            charDrawInfo.rawFontWidth = rawFontWidth;

            leftX += drawTotalCharWidth;
        }

        lineDrawInfo->flag |= NSDrawInfo::LineFlag::RowEnd;
    }
}

DocSel TextPad::GetCursorByPoint(int x, int y) const
{
    const int lineHeight = GetLineHeight();
    const int fontDes = m_fontMetrix.descent();
    const int lineInfoCnt = static_cast<int>(m_drawInfo.lineInfos.size());

    for (int i = 0; i < lineInfoCnt; ++i)
    {
        const LineDrawInfo &lineInfo = m_drawInfo.lineInfos[i];
        const int baseline = GetBaseLineByLineInfoIndex(i);
        const int bottom = baseline + fontDes;
        const int top = bottom - lineHeight;
        if (top <= y && y < bottom)
        {
            const ColCnt colCnt = ColCnt(lineInfo.charInfos.size());
            // 循环到换行符之前就结束
            for (ColIndex col = 0; col < colCnt; ++col)
            {
                const CharDrawInfo &ci = lineInfo.charInfos[col];
                if (x < ci.drawLeftX + ci.rawFontWidth / 2)
                {
                    return DocSel(lineInfo.rowModelIndex, lineInfo.colOffset + col);
                }
            }
            if ((lineInfo.flag & NSDrawInfo::LineFlag::RowEnd) != 0)
            {
                return DocSel(lineInfo.rowModelIndex, lineInfo.colOffset + colCnt - 1);
            }
            else
            {
                return DocSel(lineInfo.rowModelIndex, lineInfo.colOffset + colCnt);
            }
        }
    }
    const RowCnt rowCnt(m_drawInfo.lineInfos.size());
    if (rowCnt > 0)
    {
        const LineDrawInfo &lineInfo = m_drawInfo.lineInfos[rowCnt - 1];
        const ColCnt colCnt = static_cast<ColCnt>(lineInfo.charInfos.size());
        if (colCnt > 0)
        {
            return DocSel(lineInfo.rowModelIndex, lineInfo.colOffset + colCnt - 1);
        }
    }
    return DocSel();
}

int TextPad::GetLineHeight() const
{
    const int fontHeight = m_fontMetrix.height();
    return fontHeight * 1.2;
}

int TextPad::GetBaseLineByLineInfoIndex(int lineInfoIndex) const
{
    return m_fontMetrix.ascent() + lineInfoIndex * GetLineHeight();
}

void TextPad::paintInsertCursor(QPainter &p)
{
    PainterAutoSaver painterAutoSaver(p);

    const DocSel &cursor = m_model.GetCursor();

    const int lineHeight = GetLineHeight();

    const int fontDes = m_fontMetrix.descent();

    const int lineInfoCnt = static_cast<int>(m_drawInfo.lineInfos.size());

    for (int i = 0; i < lineInfoCnt; ++i)
    {
        const LineDrawInfo &row = m_drawInfo.lineInfos[i];
        if (row.rowModelIndex == cursor.GetRow())
        {
            const ColCnt colCnt = ColCnt(row.charInfos.size());
            for (ColIndex col = 0; col < colCnt; ++col)
            {
                if (row.colOffset + col == cursor.GetCol())
                {
                    const CharDrawInfo &charInfo = row.charInfos[col];
                    const int baseline = GetBaseLineByLineInfoIndex(i);
                    const int bottom = baseline + fontDes;

                    p.drawLine(
                        charInfo.drawLeftX,
                        bottom - lineHeight,
                        charInfo.drawLeftX,
                        bottom - 1
                    );

                    return;
                }
            }
        }
    }

    p.drawLine(kLeftGap, 0, kLeftGap, lineHeight - 1);
}

void TextPad::paintTextContent(QPainter &p)
{
    PainterAutoSaver painterAutoSaver(p);

    p.setFont(m_font);

    const int lineInfoCnt = static_cast<int>(m_drawInfo.lineInfos.size());

    for (int i = 0; i < lineInfoCnt; ++i)
    {
        const LineDrawInfo &lineDrawInfo = m_drawInfo.lineInfos[i];
        const int baseline = GetBaseLineByLineInfoIndex(i);

        for (const CharDrawInfo &charDrawInfo : lineDrawInfo.charInfos)
        {
            p.drawText(charDrawInfo.drawLeftX,
                baseline,
                QChar(charDrawInfo.ch));
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
        break;
    case Qt::Key_Right:
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
    QWidget::showEvent(event);
    prepareTextContentDrawInfo(width());
}

void TextPad::resizeEvent(QResizeEvent * event)
{
    QWidget::resizeEvent(event);
    prepareTextContentDrawInfo(event->size().width());
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
