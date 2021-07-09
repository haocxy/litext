#pragma once

#include <QImage>

#include "core/flagset.h"
#include "core/callbacks.h"
#include "gui/declare_text_area.h"
#include "my_widget.h"


namespace gui::qt
{


class TextAreaWidget : public MyWidget {
    Q_OBJECT

public:
    TextAreaWidget(QWidget *parent = nullptr);

    ~TextAreaWidget();

    virtual QSize sizeHint() const override;

    virtual void paintEvent(QPaintEvent *e) override;

    virtual void showEvent(QShowEvent *e) override;

    virtual void resizeEvent(QResizeEvent *e) override;

    virtual void keyPressEvent(QKeyEvent *e) override;

    virtual void mousePressEvent(QMouseEvent *e) override;

    void setTextArea(TextArea *area);

private:
    void paintBackground(QPainter &p);

    void paintLastActLine(QPainter &p);

    void paintCursor(QPainter &p);

    void prepareTextImage();

    void paintWidget(QPainter &p);

    void refresh();

    void dumpToFile(const char *path);

private:
    class DirtyBuffFlag {
    public:
        enum {
            Text,
            FlagCount, // 保证在最后一个，用于计数
        };
    };

private:
    TextArea *area_ = nullptr;
    FlagSet<DirtyBuffFlag::FlagCount> dirtyBuffFlags_;
    QImage textPaintBuff_;
    CallbackHandle cbhViewLocChanged_;
};


}
