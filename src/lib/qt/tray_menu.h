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

signals:
    void qtSigQuit();

private slots:
    

private:
    QPointer<QAction> actionQuit_;
};

}
