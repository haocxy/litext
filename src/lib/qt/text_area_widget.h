#pragma once

#include <QWidget>

#include "core/sigconns.h"
#include "doc/doc_define.h"
#include "gui/declare_text_area.h"


namespace gui::qt
{


class TextAreaWidget : public QWidget {
    Q_OBJECT

public:
    explicit TextAreaWidget(TextArea &textArea);

    virtual ~TextAreaWidget();

    virtual void paintEvent(QPaintEvent *e) override;

    virtual void resizeEvent(QResizeEvent *e) override;

    virtual void keyPressEvent(QKeyEvent *e) override;

    virtual void mousePressEvent(QMouseEvent *e) override;

    virtual void wheelEvent(QWheelEvent *e) override;

    RowN rowOffset() const;

signals:
    void qtSigShouldRepaint();

private slots:
    void qtSlotShouldRepaint();

private:
    TextArea &textArea_;
    SigConns sigConns_;
    int scrollLineCount_ = 0;
};


}
