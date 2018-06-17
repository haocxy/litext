#pragma once

#include <QWidget>

#include "docmodel.h"
#include "font_info.h"

class QFont;
class QFontMetrics;
class SubLineComp;
class Composer;

class TextPad : public QWidget
{
    Q_OBJECT

public:
    TextPad(DocModel &model, const FontInfoProvider &fontInfo, QWidget *parent = 0);
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
    const SubLineComp *GetSubLineByY(int y) const;

    ColIndex GetColModelIndexBySubLineAndX(const SubLineComp & subLine, int x) const;

    DocSel GetCursorByPoint(int x, int y) const;

    int GetBaseLineByLineInfoIndex(int lineInfoIndex) const;

private:
    DocModel & m_model;
    const FontInfoProvider &m_fontInfo;
    Composer *m_composer = nullptr;
};

