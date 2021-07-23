#include <QApplication>
#include <QPixmap>
#include <QPainter>

#include "core/logger.h"
#include "gui/qt/main_window.h"
#include "gui/config.h"

#include <cairo/cairo.h>

typedef struct hex_color {
    uint16_t r, g, b;
} hex_color_t;

hex_color_t HI_COLOR_1 = { 0xff, 0x33, 0xff };

static void
set_hex_color(cairo_t *cr, hex_color_t color)
{
    cairo_set_source_rgb(cr,
        color.r / 255.0,
        color.g / 255.0,
        color.b / 255.0);
}



static void testCairo()
{
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 800, 600);

    cairo_t *cr = cairo_create(surface);

    cairo_rectangle(cr, 100, 100, 300, 200);
    set_hex_color(cr, HI_COLOR_1);
    cairo_fill(cr);

    cairo_surface_write_to_png(surface, "D:/tmp/bycairo.png");

    cairo_destroy(cr);
    cr = nullptr;

    cairo_surface_destroy(surface);
    surface = nullptr;
}

static void useDrawText()
{
    QPixmap pix(32, 32);
    QPainter painter(&pix);
    painter.drawText(0, 0, "0");
}

int entry(int argc, char *argv[])
{
    testCairo();
    return 0;

    QApplication app(argc, argv);

    // 在 Windows 平台发现窗口首次打开时会有一段时间全部为白色，
    // 调查后发现是卡在了 QPainter::drawText(...) 的首次有效调用，
    // 虽然没有看 Qt 的内部实现，但猜测是由于某种延迟加载的机制导致的，
    // 所以解决办法就是在窗口显示前提前使用这个函数（注意，绘制的文本不能为空字符串）
    useDrawText();

    logger::control::Option logOpt;
    logOpt.setLevel("all");
    logOpt.setDir("D:/tmp/log");
    logOpt.setBasename("notesharplog");
    logOpt.setAlwaysFlush(true);
    logger::control::init(logOpt);

    gui::Config config;

    gui::qt::MainWindow mainWindow(config);
    if (argc > 1) {
        mainWindow.openDocument(argv[1]);
    }
    mainWindow.show();

    return app.exec();
}
