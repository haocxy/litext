#pragma once

#include <QWidget>

class View;

class LineNumBar : public QWidget
{
    Q_OBJECT
public:
    LineNumBar(View * view, QWidget * parent = nullptr);
    virtual ~LineNumBar();

    virtual void paintEvent(QPaintEvent *e) override;

private:
    View & m_view;
};
