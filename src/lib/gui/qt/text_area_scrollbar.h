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
};

}
