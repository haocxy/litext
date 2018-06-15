#pragma once

#include <QWidget>

#include "docmodel.h"

class QFont;
class QFontMetrics;

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

    struct RowDrawInfo
    {
        // ��һ�����е���ƫ��
        // ����������Զ����У�������ֶα�ʾǰ�漸�е��ַ�������
        // ���û�п����Զ����У������ֵʼ��Ϊ0
        ColCnt colOffset = 0;
        RowIndex rowIndex = 0;
        int baseLineY = 0; // ����y����
        int drawTopY = 0; // ��һ�еĻ�����������y����
        int drawBottomY = 0; // ��һ�еĻ�����������y����
        bool rowEnd = false;
        std::vector<CharDrawInfo> charInfos;
    };

    class DrawInfo
    {
    public:

        void Clear()
        {
            _drawInfos.clear();
        }

        std::vector<RowDrawInfo> _drawInfos;
    };

    DrawInfo m_drawInfo;

    void prepareTextContentDrawInfo(int areaWidth);

    DocSel GetCursorByPoint(int x, int y) const;

    int GetLineHeight() const;

private:
    DocModel & m_model;
    QFont m_font;
    QFontMetrics m_fontMetrix;

private:
    bool m_wrapLine = true;
};

