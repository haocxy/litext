#include "tray_menu.h"


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

    std::exit(0);
}

}
