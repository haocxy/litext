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
    // �ȿ�"Times"
    // �ǵȿ�"Microsoft YaHei"

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

    const int lineHeight = GetLineHeight();

    const QColor color = QColor(Qt::blue).lighter(190);

    for (const LineDrawInfo &row : prepared_draw_info_._drawInfos)
    {
        if (row.rowIndex == cursor.GetRow())
        {
            p.fillRect(0, row.drawBottomY - lineHeight, width, lineHeight, color);
            return;
        }
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
    prepared_draw_info_.Clear();

    const int fontHeight = font_metrix_.height();
    const int lineHeight = GetLineHeight();//fontHeight / 2 + fontHeight;
    
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
        lineDrawInfo->rowIndex = row;
        lineDrawInfo->baseLineY = baseLineY;
        lineDrawInfo->drawTopY = lineTopY;
        lineDrawInfo->drawBottomY = lineBottomY;

        for (ColIndex col = 0; col < charCnt; ++col)
        {
            const QChar ch(model_[row][col]);

            // �����������õ�ԭʼ�ַ����
            const int rawFontWidth = font_metrix_.width(ch);
            
            // �ж�����ַ��Ƿ��ǿ��ַ�
            const bool isWideChar = rawFontWidth > widthForFix;
            
            // �����ǰ�����ǵȿ�����������ַ��ǿ��ַ��������ʱ��Ϊ�����ַ�����
            const bool asTwoChar = isWideChar && isFixWidthFont;

            // ��������ַ��ܹ�ռ�Ŀ�ȣ���������հ�
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

            // ������������
            if (leftX + drawTotalCharWidth > areaWidth)
            {
                // ����������Զ�������ѻ���λ�ö�λ����һ�п�ͷ
                // ���δ�����Զ����У����������к���Ļ��ƣ���ʼ������һ��
                if (_wrapLine)
                {
                    leftX = kLeftGap;
                    baseLineY += lineHeight;
                    lineTopY = baseLineY - fontAscent;
                    lineBottomY = baseLineY + fontDescent;

                    
                    lineDrawInfo = &GrowBack(prepared_draw_info_._drawInfos);
                    lineDrawInfo->colOffset = col;
                    lineDrawInfo->rowIndex = row;
                    lineDrawInfo->baseLineY = baseLineY;
                    lineDrawInfo->drawTopY = lineTopY;
                    lineDrawInfo->drawBottomY = lineBottomY;
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

        lineDrawInfo->rowEnd = true;
    }
}

DocSel TextPad::GetCursorByPoint(int x, int y) const
{
    const int lineHeight = GetLineHeight();

    for (const LineDrawInfo &lineInfo : prepared_draw_info_._drawInfos)
    {
        if (lineInfo.drawBottomY - lineHeight <= y && y < lineInfo.drawBottomY)
        {
            const ColCnt colCnt = ColCnt(lineInfo.charInfos.size());
            // ѭ�������з�֮ǰ�ͽ���
            for (ColIndex col = 0; col < colCnt; ++col)
            {
                const CharDrawInfo &ci = lineInfo.charInfos[col];
                if (x < ci.drawLeftX + ci.rawFontWidth / 2)
                {
                    return DocSel(lineInfo.rowIndex, lineInfo.colOffset + col);
                }
            }
            if (lineInfo.rowEnd)
            {
                return DocSel(lineInfo.rowIndex, lineInfo.colOffset + colCnt - 1);
            }
            else
            {
                return DocSel(lineInfo.rowIndex, lineInfo.colOffset + colCnt);
            }
        }
    }
    const RowCnt rowCnt(prepared_draw_info_._drawInfos.size());
    if (rowCnt > 0)
    {
        const LineDrawInfo &lineInfo = prepared_draw_info_._drawInfos[rowCnt - 1];
        const ColCnt colCnt(lineInfo.charInfos.size());
        if (colCnt > 0)
        {
            return DocSel(lineInfo.rowIndex, lineInfo.colOffset + colCnt - 1);
        }
    }
    return DocSel();
}

int TextPad::GetLineHeight() const
{
    const int fontHeight = font_metrix_.height();
    return fontHeight * 1.2;
}

void TextPad::paintInsertCursor(QPainter &p)
{
    PainterAutoSaver painterAutoSaver(p);

    const DocSel &cursor = model_.GetCursor();

    const int lineHeight = GetLineHeight();

    for (const LineDrawInfo &row : prepared_draw_info_._drawInfos)
    {
        if (row.rowIndex == cursor.GetRow())
        {
            const ColCnt colCnt = ColCnt(row.charInfos.size());
            for (ColIndex col = 0; col < colCnt; ++col)
            {
                if (row.colOffset + col == cursor.GetCol())
                {
                    const CharDrawInfo &charInfo = row.charInfos[col];

                    p.drawLine(
                        charInfo.drawLeftX,
                        row.drawBottomY - lineHeight,
                        charInfo.drawLeftX,
                        row.drawBottomY - 1
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
