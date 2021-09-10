#include "tray_menu.h"

#include <QApplication>


namespace gui::qt
{

TrayMenu::TrayMenu() {
    actionQuit_ = addAction(tr("Quit"));
    connect(actionQuit_, &QAction::triggered, this, &TrayMenu::qtSlotActionQuit);
}

TrayMenu::~TrayMenu()
{
}

void TrayMenu::qtSlotActionQuit()
{
    hide();

    QApplication::quit();
}

}
