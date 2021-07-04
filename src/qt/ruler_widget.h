#pragma once

#include <QWidget>
#include "util/callbacks.h"

class View;
class QPainter;

class LineNumBarWidget : public QWidget
{
    Q_OBJECT
public:
    LineNumBarWidget(View * view, QWidget * parent = nullptr);
    virtual ~LineNumBarWidget();

    virtual void paintEvent(QPaintEvent *e) override;

private:
    void paintBackground(QPainter & p);
    void paintLineNum(QPainter & p);

private:
    View & m_view;
    CallbackHandle m_cbhUpdate;
	CallbackHandle m_cbhViewLocChange;
};
