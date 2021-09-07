#pragma once

#include <QPointer>
#include <QMenu>


namespace gui::qt
{

class TrayMenu : public QMenu {
    Q_OBJECT
public:
    TrayMenu();

    virtual ~TrayMenu();

private slots:
    void qtSlotActionQuit();

private:
    QPointer<QAction> actionQuit_;
};

}
