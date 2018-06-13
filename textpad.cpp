#include "textpad.h"

#include <QFont>
#include <QFontMetrics>
#include <QDebug>
#include <QPainter>
#include <QInputMethodEvent>
#include <QShowEvent>
#include <QResizeEvent>
#include <QMouseEvent>

#include "DocumentModel.h"

namespace
{
    const char *kFontFamilyTimes = "Times";
    const char *kFontFamilyYaHei = "Microsoft YaHei";

    const char *kFontFamily = kFontFamilyTimes;
    const int kFontSize = 30;

    class PainterAutoSaver
    {
    public:
        PainterAutoSaver(QPainter &painter) :painter_(painter)
        {
            painter_.save();
        }
        ~PainterAutoSaver()
        {
            painter_.restore();
        }

    private:
        QPainter & painter_;
    };
}

TextPad::TextPad(DocModel &model, QWidget *parent)
    : font_(kFontFamily, kFontSize)
    , font_metrix_(font_)
    , model_(model)
    , QWidget(parent)
{
    // 等宽："Times"
    // 非等宽："Microsoft YaHei"

    setCursor(Qt::IBeamCursor);
    setAttribute(Qt::WA_InputMethodEnabled);
    setFocus();

    lay_select_ = new QPixmap(size());
}

bool TextPad::IsFixWidthFont() const
{
    return font_metrix_.width('i') == font_metrix_.width('w');
}

TextPad::~TextPad()
{
    delete lay_select_;
    lay_select_ = nullptr;
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

    const DocSel &cursor = model_.GetCursor();

    const int width = rect().width();

    const QColor color = QColor(Qt::blue).lighter(190);

    for (const LineDrawInfo &row : prepared_draw_info_._drawInfos)
    {
        if (row.rowIndex == cursor.GetRow())
        {

            p.fillRect(0, row.drawBottomY - row.lineHeight, width, row.lineHeight, color);

            break;
        }
    }
}

static int kFontMargin = 2;
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
    prepared_draw_info_.Clear();

    const int fontHeight = font_metrix_.height();
    const int lineHeight = fontHeight * 1.2;//fontHeight / 2 + fontHeight;
    
    const bool isFixWidthFont = IsFixWidthFont();
    const int widthForFix = font_metrix_.width('a');

    int leftX = 0;

    const int fontAscent = font_metrix_.ascent();
    const int fontDescent = font_metrix_.descent();

    int baseLineY = font_metrix_.ascent();

    const RowCnt rowCnt = model_.GetRowCnt();

    for (RowIndex row = 0; row < rowCnt; ++row)
    {
        leftX = kLeftGap;
        const ColCnt charCnt = model_.GetColCntOfLine(row);

        int lineTopY = baseLineY - fontAscent;
        int lineBottomY = baseLineY + fontDescent;

        LineDrawInfo *lineDrawInfo = &GrowBack(prepared_draw_info_._drawInfos);
        lineDrawInfo->isWrapLine = false;
        lineDrawInfo->rowIndex = row;
        lineDrawInfo->baseLineY = baseLineY;
        lineDrawInfo->drawTopY = lineTopY;
        lineDrawInfo->drawBottomY = lineBottomY;
        lineDrawInfo->lineHeight = lineHeight;

        for (ColIndex col = 0; col < charCnt; ++col)
        {
            const QChar ch(model_[row][col]);

            // 从字体引擎获得的原始字符宽度
            const int rawFontWidth = font_metrix_.width(ch);
            
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
                if (_wrapLine)
                {
                    leftX = kLeftGap;
                    baseLineY += lineHeight;
                    lineTopY = baseLineY - fontAscent;
                    lineBottomY = baseLineY + fontDescent;

                    
                    lineDrawInfo = &GrowBack(prepared_draw_info_._drawInfos);
                    lineDrawInfo->isWrapLine = true;
                    lineDrawInfo->rowIndex = row;
                    lineDrawInfo->baseLineY = baseLineY;
                    lineDrawInfo->drawTopY = lineTopY;
                    lineDrawInfo->drawBottomY = lineBottomY;
                    lineDrawInfo->lineHeight = lineHeight;
                }
            }

            CharDrawInfo &charDrawInfo = GrowBack(lineDrawInfo->charInfos);
            charDrawInfo.ch = ch;
            charDrawInfo.drawLeftX = leftX + kFontMargin;
            charDrawInfo.drawTotalWidth = drawTotalCharWidth;
            charDrawInfo.rawFontWidth = rawFontWidth;

            leftX += drawTotalCharWidth;
        }

        baseLineY += lineHeight;
    }
}

DocSel TextPad::GetCursorByPoint(int x, int y) const
{
    for (const LineDrawInfo &lineInfo : prepared_draw_info_._drawInfos)
    {
        if (lineInfo.drawBottomY - lineInfo.lineHeight <= y && y < lineInfo.drawBottomY)
        {
            const ColCnt last = ColCnt(lineInfo.charInfos.size()) - 1;
            // 循环到换行符之前就结束
            for (ColIndex col = 0; col < last; ++col)
            {
                const CharDrawInfo &ci = lineInfo.charInfos[col];
                if (ci.drawLeftX <= x && x <= ci.drawLeftX + ci.rawFontWidth)
                {
                    if (x <= ci.drawLeftX + ci.rawFontWidth / 2)
                    {
                        return DocSel(lineInfo.rowIndex, col);
                    }
                    else
                    {
                        return DocSel(lineInfo.rowIndex, col + 1);
                    }
                }
            }
            return DocSel(lineInfo.rowIndex, last);
        }
    }
    const RowCnt rowCnt(prepared_draw_info_._drawInfos.size());
    if (rowCnt > 0)
    {
        const LineDrawInfo &lineInfo = prepared_draw_info_._drawInfos[rowCnt - 1];
        const ColCnt colCnt(lineInfo.charInfos.size());
        if (colCnt > 0)
        {
            return DocSel(rowCnt - 1, colCnt - 1);
        }
    }
    return DocSel();
}

void TextPad::paintInsertCursor(QPainter &p)
{
    PainterAutoSaver painterAutoSaver(p);

    const DocSel &cursor = model_.GetCursor();

    for (const LineDrawInfo &row : prepared_draw_info_._drawInfos)
    {
        if (row.rowIndex == cursor.GetRow())
        {
            const ColCnt colCnt = ColCnt(row.charInfos.size());
            for (ColIndex col = 0; col < colCnt; ++col)
            {
                if (col == cursor.GetCol())
                {
                    const CharDrawInfo &charInfo = row.charInfos[col];

                    p.drawLine(
                        charInfo.drawLeftX,
                        row.drawBottomY - row.lineHeight,
                        charInfo.drawLeftX,
                        row.drawBottomY - 1
                    );

                    break;
                }
            }

            break;
        }
    }
}

void TextPad::paintTextContent(QPainter &p)
{
    PainterAutoSaver painterAutoSaver(p);

    p.setFont(font_);

    for (const LineDrawInfo &lineDrawInfo : prepared_draw_info_._drawInfos)
    {
        for (const CharDrawInfo &charDrawInfo : lineDrawInfo.charInfos)
        {
            p.drawText(charDrawInfo.drawLeftX,
                lineDrawInfo.baseLineY,
                charDrawInfo.ch);
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
        if (newCursor != model_.GetCursor())
        {
            model_.SetCursor(newCursor);
            repaint();
        }
    }
}
