#pragma once

#include <QWidget>
#include <QImage>

#include "core/flagset.h"
#include "core/callbacks.h"
#include "gui/declare_text_area.h"
#include "object_worker.h"


namespace gui::qt
{


class TextAreaWidget : public QWidget {
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

    void bind(TextArea *area);

    void unbind();

private:
    void paintBackground(QPainter &p);

    void paintLastActLine(QPainter &p);

    void paintCursor(QPainter &p);

    void prepareTextImage();

    void paintWidget(QPainter &p);

    void refresh();

private:
    class DirtyBuffFlag {
    public:
        enum {
            Text,
            FlagCount, // 保证在最后一个，用于计数
        };
    };

private:
    ObjectWorker worker_;
    TextArea *area_ = nullptr;
    FlagSet<DirtyBuffFlag::FlagCount> dirtyBuffFlags_;
    QImage textPaintBuff_;
    CallbackHandle cbhViewLocChanged_;
};


}
