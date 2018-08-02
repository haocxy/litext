#pragma once

#include <QWidget>
#include "util/listeners.h"

class View;
class QPainter;

class LineNumBar : public QWidget
{
    Q_OBJECT
public:
    LineNumBar(View * view, QWidget * parent = nullptr);
    virtual ~LineNumBar();

    virtual void paintEvent(QPaintEvent *e) override;

private:
    void paintBackground(QPainter & p);
    void paintLineNum(QPainter & p);

private:
    View & m_view;
    ListenerID m_listenerIdForUpdate = 0;
};
