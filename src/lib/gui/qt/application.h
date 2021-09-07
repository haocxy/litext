#pragma once

#include <vector>

#include <QVector>
#include <QPair>
#include <QApplication>

#include "core/sigconns.h"
#include "core/basetype.h"
#include "gui/init_info.h"
#include "gui/engine.h"

#include "main_window.h"


namespace gui::qt
{

class Application : public QObject {
    Q_OBJECT
public:
    Application();

    ~Application();

    void init(const InitInfo &initInfo);

    int exec();

private:

    void initQtApp();

    void initMainWindow();
    
    void openFile(const doc::OpenInfo &openInfo);

    void openFiles(const std::vector<doc::OpenInfo> &openInfos);

    void bindSignals();

    void showMainWindow();

signals:
    void qtSigShowWindow();

    void qtSigOpenFile(const QString &path, long long row);

private slots:
    void qtSlotOpenFile(const QString &path, long long row);

private:
    Engine engine_;
    uptr<QApplication> qtApp_;
    MainWindow *mainWindow_ = nullptr;
    SigConns sigConns_;
};

}
