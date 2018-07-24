#pragma once

#include <QWidget>

class View;
class DocController;

class TextPad2 : public QWidget
{
    //Q_OBJECT

public:
    TextPad2(View *view, DocController *controller, QWidget *parent = nullptr);
    ~TextPad2();

    virtual void paintEvent(QPaintEvent *e) override;
    virtual void showEvent(QShowEvent *e) override;
    virtual void resizeEvent(QResizeEvent *e) override;
    virtual void keyPressEvent(QKeyEvent *e) override;
    virtual void mousePressEvent(QMouseEvent *e) override;

private:
    void paintBackground(QPainter &p);
    void paintTextContent(QPainter &p);
    void paintCursor(QPainter &p);

private:
    View &m_view;
    DocController & m_controller;
};

