#pragma once


#include <QWidget>

class QFont;
class QFontMetrics;
class DocumentModel;

class TextPad : public QWidget
{
    Q_OBJECT

public:
    TextPad(DocumentModel &model, QWidget *parent = 0);
    ~TextPad();

    virtual void keyPressEvent(QKeyEvent *e) override;
    virtual void paintEvent(QPaintEvent* e) override;
    virtual void inputMethodEvent(QInputMethodEvent *e) override;
    virtual void showEvent(QShowEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    bool IsFixWidthFont() const;
    void paintBackground(QPainter &p);
    void paintTextContent(QPainter &p);
    void paintInsertCursor(QPainter &p);

private:

    struct CharDrawInfo
    {
        QChar ch = 0;
        int leftX = 0;
        int drawLeftX = 0; // 这一字符的绘制区域的最左侧x坐标
        int drawTotalWidth = 0; // 这一字符的绘制区域的宽度
    };

    struct LineDrawInfo
    {
        bool isWrapLine = false;
        int rowIndex = 0;
        int baseLineY = 0; // 基线y坐标
        int drawTopY = 0; // 这一行的绘制区域上限y坐标
        int drawBottomY = 0; // 这一行的绘制区域下限y坐标
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

private:
    DocumentModel & model_;
    QFont font_;
    QFontMetrics font_metrix_;

private:
    QPixmap *lay_select_ = nullptr;

private:
    int64_t insert_line_row_ = 0;
    int64_t insert_line_col_ = 0;
    bool _wrapLine = true;
};

