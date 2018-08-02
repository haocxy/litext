#pragma once

#include <QWidget>
#include <QPixmap>

class View;

class TextPad : public QWidget
{
    // Q_OBJECT

public:
    TextPad(View *view, QWidget *parent = nullptr);
    ~TextPad();

    virtual QSize sizeHint() const override;
    virtual void paintEvent(QPaintEvent *e) override;
    virtual void showEvent(QShowEvent *e) override;
    virtual void resizeEvent(QResizeEvent *e) override;
    virtual void keyPressEvent(QKeyEvent *e) override;
    virtual void mousePressEvent(QMouseEvent *e) override;

private:
    void paintBackground(QPainter &p);
    void paintLastActLine(QPainter &p);
    void paintCursor(QPainter &p);
    void paintTextContent(QPainter &p);
    void paintOnPixmap();
    void refresh();
private:
    View &m_view;
    QPixmap m_buff;
};

