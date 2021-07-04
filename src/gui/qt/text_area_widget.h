#pragma once

#include <QWidget>
#include <QImage>

#include "core/flagset.h"
#include "core/callbacks.h"


class TextArea;

class TextAreaWidget : public QWidget
{
    Q_OBJECT

public:
    TextAreaWidget(TextArea *view, QWidget *parent = nullptr);

    ~TextAreaWidget();

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
    TextArea &view_;
    FlagSet<DirtyBuffFlag::FlagCount> dirtyBuffFlags_;
    QImage textPaintBuff_;
    CallbackHandle cbhViewLocChanged_;
};

