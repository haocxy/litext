#pragma once

#include <QWidget>

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

signals:
    void qtSigShouldRepaint();

private:
    TextArea &textArea_;
    SigConns textAreaSigConns_;
};


}
