#pragma once

#include <QScrollBar>


namespace gui::qt
{

class TextAreaScrollBar : public QScrollBar {
    Q_OBJECT
public:
    TextAreaScrollBar(QWidget *parent = nullptr);

    virtual ~TextAreaScrollBar();

signals:
    void jumpValueChanged(int value);

protected:
    virtual void mousePressEvent(QMouseEvent *e) override;

    virtual void mouseReleaseEvent(QMouseEvent *e) override;

    virtual void mouseMoveEvent(QMouseEvent *e) override;

private:
    void jumpToY(int y);

    bool jumping_ = false;
};

}
