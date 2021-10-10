#pragma once

#include <QWidget>


namespace gui::qt
{

class PopupWidget : public QWidget {
    Q_OBJECT
public:
    explicit PopupWidget(QWidget *parent = nullptr);

    virtual ~PopupWidget();

    void locate(QWidget *base);

private:
    void locateRelativeTo(QWidget *parent);

    void locateWithoutParent();

    void locateWithParent(const QWidget &parent);
};

}
