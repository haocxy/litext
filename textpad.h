#pragma once


#include <QWidget>

#include "DocumentModel.h"

class QFont;
class QFontMetrics;
class DocModel;

class TextPad : public QWidget
{
    Q_OBJECT

public:
    TextPad(DocModel &model, QWidget *parent = 0);
    ~TextPad();

    virtual void keyPressEvent(QKeyEvent *e) override;
    virtual void paintEvent(QPaintEvent* e) override;
    virtual void inputMethodEvent(QInputMethodEvent *e) override;
    virtual void showEvent(QShowEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *e) override;

private:
    bool IsFixWidthFont() const;
    void paintBackground(QPainter &p);
    void paintRowBackground(QPainter &p);
    void paintTextContent(QPainter &p);
    void paintInsertCursor(QPainter &p);
    

private:

    struct CharDrawInfo
    {
        QChar ch = 0;
        int drawLeftX = 0; // 这一字符的绘制区域的最左侧x坐标
        int drawTotalWidth = 0; // 这一字符的绘制区域的宽度
        int rawFontWidth = 0; // 从字体引擎得到的原始字符宽度
    };

    struct LineDrawInfo
    {
        bool isWrapLine = false;
        int rowIndex = 0;
        int baseLineY = 0; // 基线y坐标
        int drawTopY = 0; // 这一行的绘制区域上限y坐标
        int drawBottomY = 0; // 这一行的绘制区域下限y坐标
        int lineHeight = 0;
        std::vector<CharDrawInfo> charInfos;
    };

    class PreparedDrawInfo
    {
    public:

        void Clear()
        {
            _drawInfos.clear();
        }

        std::vector<LineDrawInfo> _drawInfos;
    };

    PreparedDrawInfo prepared_draw_info_;

    void prepareTextContentDrawInfo(int areaWidth);

    DocSel GetCursorByPoint(int x, int y) const;

private:
    DocModel & model_;
    QFont font_;
    QFontMetrics font_metrix_;

private:
    QPixmap *lay_select_ = nullptr;

private:
    bool _wrapLine = true;
};

