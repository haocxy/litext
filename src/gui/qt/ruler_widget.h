#pragma once

#include <QWidget>
#include "core/callbacks.h"


class TextArea;
class QPainter;

class RulerWidget : public QWidget
{
    Q_OBJECT
public:
    RulerWidget(TextArea *view, QWidget * parent = nullptr);
    virtual ~RulerWidget();

    virtual void paintEvent(QPaintEvent *e) override;

private:
    void paintBackground(QPainter & p);
    void paintLineNum(QPainter & p);

private:
    TextArea &m_view;
    CallbackHandle m_cbhUpdate;
	CallbackHandle m_cbhViewLocChange;
};
