#pragma once

#include <QWidget>
#include "core/callbacks.h"
#include "gui/declare_text_area.h"


class QPainter;

class RulerWidget : public QWidget
{
    Q_OBJECT
public:
    RulerWidget(gui::TextArea *view, QWidget * parent = nullptr);
    virtual ~RulerWidget();

    virtual void paintEvent(QPaintEvent *e) override;

private:
    void paintBackground(QPainter &p);
    void paintLineNum(QPainter &p);

private:
    gui::TextArea &m_view;
    CallbackHandle m_cbhUpdate;
	CallbackHandle m_cbhViewLocChange;
};
