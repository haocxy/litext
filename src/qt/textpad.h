#pragma once

#include <bitset>
#include <QWidget>
#include <QImage>

#include "util/listeners.h"

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
    void prepareTextImage();
    void paintOnPixmap();
    void refresh();
private:
    enum DirtyBuffFlag
    {
        DBF_Text,
        

        DBF_Cnt, // 保证在最后一个，用于计数
    };

private:
    View &m_view;
    std::bitset<DBF_Cnt> m_dirtyBuffFlags;
    QImage m_buff;
    QImage m_textBuff;

private:
    CallbackHandle m_listenerHandleViewLocChange;
};

