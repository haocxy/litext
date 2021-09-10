#include "tray_menu.h"

#include <QApplication>


namespace gui::qt
{

TrayMenu::TrayMenu() {

    actionActivateUI_ = addAction(tr("Activate UI"));
    connect(actionActivateUI_, &QAction::triggered, this, &TrayMenu::qtSigActivateUI);

    actionQuit_ = addAction(tr("Quit"));
    connect(actionQuit_, &QAction::triggered, this, &TrayMenu::qtSigQuit);
}

TrayMenu::~TrayMenu()
{
}

}
