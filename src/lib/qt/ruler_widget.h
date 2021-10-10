#pragma once

#include <QWidget>

#include "core/basetype.h"
#include "core/sigconns.h"
#include "gui/declare_text_area.h"


class QPainter;


namespace gui::qt
{


class RulerWidget : public QWidget {
    Q_OBJECT
public:
    explicit RulerWidget(TextArea &textArea);
    virtual ~RulerWidget();

    virtual void paintEvent(QPaintEvent *e) override;

private:
    void paintBackground(QPainter &p);

    void paintLineNum(QPainter &p);

    void setRulerWidth(int digitCount);

private:
signals:
    void qtSigUpdateContent();

    void qtSigUpdateWidth(int digitCount);

    void qtSigFontSizeUpdated(int pt);

private slots:
    void qtSlotUpdateContent();

    void qtSlotUpdateWidth(int digitCount);

private:
    void adjustRulerWidth();

private:
    TextArea &textArea_;
    SigConns textAreaSigConns_;
    int maxDigitCount_ = 0;
};


}
