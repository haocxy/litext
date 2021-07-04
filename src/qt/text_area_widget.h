#pragma once

#include <bitset>
#include <QWidget>
#include <QImage>

#include "util/callbacks.h"

class View;

class TextAreaWidget : public QWidget
{
    Q_OBJECT

public:
    TextAreaWidget(View *view, QWidget *parent = nullptr);
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
    enum DirtyBuffFlag
    {
        DBF_Text,
        

        DBF_Cnt, // 保证在最后一个，用于计数
    };

private:
    View &view_;
    std::bitset<DBF_Cnt> m_dirtyBuffFlags;
    QImage m_textBuff;

private:
    CallbackHandle m_listenerHandleViewLocChange;
};

