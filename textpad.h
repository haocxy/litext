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
        int drawLeftX = 0; // ��һ�ַ��Ļ�������������x����
        int drawTotalWidth = 0; // ��һ�ַ��Ļ�������Ŀ��
        int rawFontWidth = 0; // ����������õ���ԭʼ�ַ����
    };

    struct LineDrawInfo
    {
        // ��һ�����е���ƫ��
        // ����������Զ����У�������ֶα�ʾǰ�漸�е��ַ�������
        // ���û�п����Զ����У������ֵʼ��Ϊ0
        ColCnt colOffset = 0;
        RowIndex rowIndex = 0;
        int baseLineY = 0; // ����y����
        int drawTopY = 0; // ��һ�еĻ�����������y����
        int drawBottomY = 0; // ��һ�еĻ�����������y����
        int lineHeight = 0;
        bool rowEnd = false;
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

