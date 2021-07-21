#include <QApplication>
#include <QPixmap>
#include <QPainter>

#include "gui/qt/main_window.h"
#include "gui/config.h"


static void useDrawText()
{
    QPixmap pix(32, 32);
    QPainter painter(&pix);
    painter.drawText(0, 0, "0");
}

int entry(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 在 Windows 平台发现窗口首次打开时会有一段时间全部为白色，
    // 调查后发现是卡在了 QPainter::drawText(...) 的首次有效调用，
    // 虽然没有看 Qt 的内部实现，但猜测是由于某种延迟加载的机制导致的，
    // 所以解决办法就是在窗口显示前提前使用这个函数（注意，绘制的文本不能为空字符串）
    useDrawText();

    gui::Config config;

    gui::qt::MainWindow mainWindow(config, argc >= 2 ? argv[1] : "D:\\tmp\\bigtest.txt");
    mainWindow.show();
    return app.exec();
}
