#pragma once

#include <QWidget>
#include <QImage>

#include "core/flagset.h"
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

    virtual void showEvent(QShowEvent *e) override;

    virtual void resizeEvent(QResizeEvent *e) override;

    virtual void keyPressEvent(QKeyEvent *e) override;

    virtual void mousePressEvent(QMouseEvent *e) override;

    RowN rowOffset() const;

private:
    void paintBackground(QPainter &p);

    void paintLastActLine(QPainter &p);

    void paintCursor(QPainter &p);

    void prepareTextImage();

    void paintWidget(QPainter &p);

signals:
    void qtSigShouldRepaint();

private slots:
    void qtSlotShouldRepaint();

private:
    class DirtyBuffFlag {
    public:
        enum {
            Text,
            FlagCount, // 保证在最后一个，用于计数
        };
    };

private:
    TextArea &textArea_;
    FlagSet<DirtyBuffFlag::FlagCount> dirtyBuffFlags_;
    QImage textPaintBuff_;
    SigConns sigConns_;
};


}
