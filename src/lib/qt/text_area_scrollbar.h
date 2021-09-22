#pragma once

#include <QScrollBar>

#include "core/sigconns.h"

#include "gui/declare_text_area.h"


namespace gui::qt
{

class TextAreaScrollBar : public QScrollBar {
    Q_OBJECT
public:
    TextAreaScrollBar(TextArea &textArea, QWidget *parent = nullptr);

    virtual ~TextAreaScrollBar();

    void disconnectAllSignals();

protected:
    virtual void mousePressEvent(QMouseEvent *e) override;

    virtual void mouseReleaseEvent(QMouseEvent *e) override;

    virtual void mouseMoveEvent(QMouseEvent *e) override;

private:
signals:
    void qtSigTextAreaShouldJumpTo(int scrollPos);

    void qtSigUpdateValue(int value);

    void qtSigUpdateRange(int minimum, int maximum);

private:
    void jumpToY(int y);

private:
    TextArea &textArea_;

    bool jumping_ = false;

    SigConns sigConns_;
};

}
