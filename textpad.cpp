#include "textpad.h"

#include <QFont>
#include <QFontMetrics>
#include <QDebug>
#include <QPainter>
#include <QInputMethodEvent>

#include "DocumentModel.h"

TextPad::TextPad(DocumentModel &model, QWidget *parent)
    : model_(model)
    , QWidget(parent)
{
    // 等宽："Times"
    // 非等宽："Microsoft YaHei"
    font_ = new QFont("Times", 26);
    font_metrix_ = new QFontMetrics(*font_);

    setCursor(Qt::IBeamCursor);
    setAttribute(Qt::WA_InputMethodEnabled);
    setFocus();

}

void TextPad::test()
{
    qDebug() << font_->family();
    qDebug() << font_metrix_->width('z');
}

void TextPad::SetContent(const QString &s)
{
    content_ = s;
}

bool TextPad::IsFixWidthFont() const
{
    return font_metrix_->width('i') == font_metrix_->width('w');
}

TextPad::~TextPad()
{
    delete font_metrix_;
    font_metrix_ = nullptr;

    delete font_;
    font_ = nullptr;
}

void TextPad::paintBackground(QPainter &p)
{
    p.save();
    if (hasFocus()) {
        p.setBrush(Qt::white);
    }
    p.drawRect(rect());
    p.restore();
}

void TextPad::paintTextContent(QPainter &p)
{
    p.save();
    p.setFont(*font_);
    const int fh = font_metrix_->height();
    const int area_width = width();
    int y = fh;
    int left = 0;
    const bool isFixWidth = IsFixWidthFont();
    const int fixwidth = font_metrix_->width('a');
    int row = 0;
    int col = 0;
    for (int i = 0; i < content_.size(); ++i) {
        const QChar ch(content_.at(i));
        int fw = font_metrix_->width(ch);
        const bool isWideChar = fw > fixwidth;
        const bool asTwoChar = isFixWidth && isWideChar;
        if (asTwoChar) {
            fw = 2 * fixwidth;
        }
        const bool isNewLineChar = ch == '\n';
        if (isNewLineChar || left + fw > area_width) {
            left = 0;
            y += fh;
        }
        if (isNewLineChar) {
            ++row;
            col = 0;
            continue;
        }
        const int dynHGap = (asTwoChar ? 2 * hgap_ : hgap_);
        if (row == insert_line_row_ && col == insert_line_col_) {
            p.drawLine(left, row*fh+fh*0.2, left, row*fh+fh);
        }
        p.drawText(left, y, QString(ch));
        left += fw;
        left += dynHGap;
        ++col;
    }
    p.restore();
}

static int kFontMargin = 5;

namespace
{
    inline int Max(int a, int b)
    {
        return a > b ? a : b;
    }
}

void TextPad::paintTextContentES(QPainter &p)
{
    const int64_t viewLineCnt = model_.GetViewLineCnt();
    if (viewLineCnt <= 0)
    {
        return;
    }

    p.save();
    p.setFont(*font_);

    const int fontHeight = font_metrix_->height();
    const int areaWidth = width();
    
    const bool isFixWidthFont = IsFixWidthFont();
    const int widthForFix = font_metrix_->width('a');

    int leftX = 0;
    int topY = 0;

    for (int64_t row = 0; row < viewLineCnt; ++row)
    {
        leftX = 0;
        const int64_t charCnt = model_.GetViewCharCntOfLine(row);
        for (int64_t col = 0; col < charCnt; ++col)
        {
            const QChar ch(model_.GetCharByViewPos(row, col));

            // 从字体引擎获得的原始字符宽度
            const int rawFontWidth = font_metrix_->width(ch);
            
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
                    leftX = 0;
                    topY += fontHeight;
                }
                else
                {
                    break;
                }
            }

            // 绘制字符
            p.drawText(leftX + kFontMargin,
                topY + fontHeight,
                QString(ch));

            // 绘制插入光标
            if (row == insert_line_row_ && col == insert_line_col_)
            {
                p.drawLine(leftX + kFontMargin / 2, topY + fontHeight * 0.2, leftX + kFontMargin / 2, topY + fontHeight);
            }

            leftX += drawTotalCharWidth;
        }

        // 绘制行末的插入光标
        if (row == insert_line_row_ && insert_line_col_ == charCnt)
        {
            p.drawLine(leftX + kFontMargin / 2, topY + fontHeight * 0.2, leftX + kFontMargin / 2, topY + fontHeight);
        }

        topY += fontHeight;
    }

    p.restore();
}

void TextPad::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    paintBackground(p);
    paintTextContentES(p);
}

void TextPad::keyPressEvent(QKeyEvent *e)
{
    const int old_row = insert_line_row_;
    const int old_col = insert_line_col_;

    const int k = e->key();
    switch (k) {
    case Qt::Key_Up:
        if (insert_line_row_ > 0)
        {
            --insert_line_row_;
        }
        break;
    case Qt::Key_Down:
        if (insert_line_row_ + 1 < model_.GetViewLineCnt())
        {
            ++insert_line_row_;
        }
        break;
    case Qt::Key_Left:
        if (insert_line_col_ > 0)
        {
            --insert_line_col_;
        }
        else
        {
            if (insert_line_row_ > 0)
            {
                --insert_line_row_;
                insert_line_col_ = model_.GetViewCharCntOfLine(insert_line_row_) - 1;
                if (insert_line_col_ < 0)
                {
                    insert_line_col_ = 0;
                }
            }
        }
        break;
    case Qt::Key_Right:
        ++insert_line_col_;
        if (insert_line_col_ > model_.GetViewCharCntOfLine(insert_line_row_))
        {
            ++insert_line_row_;
            insert_line_col_ = 0;
            if (insert_line_row_ >= model_.GetViewLineCnt())
            {
                insert_line_row_ = old_row;
                insert_line_col_ = old_col;
            }
        }
        break;
    default:
        break;
    }

    if (insert_line_row_ != old_row || insert_line_col_ != old_col)
    {
        repaint();
    }
}

void TextPad::inputMethodEvent(QInputMethodEvent *e)
{
    qDebug() << "start:" << e->replacementStart();
    qDebug() << "length:" << e->replacementLength();
    qDebug() << "preedit:" << e->preeditString();
    qDebug() << "commit:" << e->commitString();
}
