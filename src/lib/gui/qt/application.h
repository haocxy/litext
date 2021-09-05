#pragma once

#include <vector>

#include <QApplication>

#include "core/basetype.h"
#include "gui/init_info.h"
#include "gui/engine.h"

#include "main_window.h"


namespace gui::qt
{

class Application : public QObject {
    Q_OBJECT
public:
    Application(const InitInfo &initInfo);

    ~Application();

    int exec();

private:
    void initLogger(const InitInfo &initInfo);

    void cleanUseless();

    void initQtApp();

    void initMainWindow();

    void openFiles(const std::vector<doc::OpenInfo> &openInfos);

private:
    Engine engine_;
    uptr<QApplication> qtApp_;
    MainWindow *mainWindow_ = nullptr;
};

}
