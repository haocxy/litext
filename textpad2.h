#pragma once

#include <QWidget>

class View;

class TextPad2 : public QWidget
{
    Q_OBJECT

public:
    TextPad2(View *view, QWidget *parent = nullptr);
    ~TextPad2();

    virtual void paintEvent(QPaintEvent *e) override;
    virtual void showEvent(QShowEvent *e) override;
    virtual void resizeEvent(QResizeEvent *e) override;

private:
    void paintBackground(QPainter &p);
    void paintTextContent(QPainter &p);

private:
    View &m_view;
};

