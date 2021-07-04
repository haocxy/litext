#pragma once

#include <QWidget>
#include "util/callbacks.h"


class View;
class QPainter;

class RulerWidget : public QWidget
{
    Q_OBJECT
public:
    RulerWidget(View * view, QWidget * parent = nullptr);
    virtual ~RulerWidget();

    virtual void paintEvent(QPaintEvent *e) override;

private:
    void paintBackground(QPainter & p);
    void paintLineNum(QPainter & p);

private:
    View & m_view;
    CallbackHandle m_cbhUpdate;
	CallbackHandle m_cbhViewLocChange;
};
