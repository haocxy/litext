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
    RulerWidget(TextArea &view, QWidget *parent = nullptr);
    virtual ~RulerWidget();

    virtual void paintEvent(QPaintEvent *e) override;

private:
    void paintBackground(QPainter &p);
    void paintLineNum(QPainter &p);

private:
    TextArea &m_view;
    SigConns sigConns_;
};


}
