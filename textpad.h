#pragma once

#include <QWidget>

#include "drawinfo.h"
#include "docmodel.h"
#include "font_info.h"

class QFont;
class QFontMetrics;

class TextPad : public QWidget
{
    Q_OBJECT

public:
    TextPad(DocModel &model, const FontInfo &fontInfo, QWidget *parent = 0);
    ~TextPad();

    virtual void keyPressEvent(QKeyEvent *e) override;
    virtual void paintEvent(QPaintEvent* e) override;
    virtual void inputMethodEvent(QInputMethodEvent *e) override;
    virtual void showEvent(QShowEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *e) override;

private:
    void paintBackground(QPainter &p);
    void paintRowBackground(QPainter &p);
    void paintTextContent(QPainter &p);
    void paintInsertCursor(QPainter &p);
    

private:
    DrawInfo m_drawInfo;

    void prepareTextContentDrawInfo(int areaWidth);

    int GetDrawLineIndexByY(int y) const;

    const LineDrawInfo *GetLineDrawInfo(int lineDrawIndex) const;

    RowIndex GetLineModelIndexByDrawIndex(int lineDrawIndex) const;

    ColIndex GetColModelIndexBylineDrawIndexAndX(int lineDrawIndex, int x) const;

    DocSel GetCursorByPoint(int x, int y) const;

    int GetBaseLineByLineInfoIndex(int lineInfoIndex) const;

private:
    DocModel & m_model;
    const FontInfo &m_fontInfo;

private:
    bool m_wrapLine = true;
};

