#include <QApplication>
#include <QPixmap>
#include <QPainter>

#include "core/logger.h"
#include "gui/qt/main_window.h"
#include "gui/config.h"


#include <cairo.h>
#include <ft2build.h>
#include <freetype/freetype.h>
#include <fontconfig.h>


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

class MyObj : public QWidget {
public:
    auto dpiX() const {
        return this->physicalDpiX();
    }

    auto dpiY() const {
        return this->physicalDpiY();
    }
};

static void testFreeType()
{
    MyObj obj;

    FT_Library lib = nullptr;
    FT_Error error = 0;

    error = FT_Init_FreeType(&lib);

    FT_Face face = nullptr;
    error = FT_New_Face(lib, "D:/tmp/msyh.ttc", 0, &face);
    if (error != 0) {
        LOGE << "error on FT_New_Face(): " << error;
    }

    auto dpix = obj.dpiX();
    auto dpiy = obj.dpiY();

    error = FT_Set_Char_Size(face, 0, 14 * 64, dpix, dpiy);

    FT_UInt index = FT_Get_Char_Index(face, 'X');

    error = FT_Load_Glyph(face, index, FT_LOAD_DEFAULT);



    FT_Done_Face(face);
    face = nullptr;

    error = FT_Done_FreeType(lib);
    lib = nullptr;
}

static void testFontConfig()
{
    FcConfig *config = FcInitLoadConfigAndFonts();
    if (!config) {
        LOGE << "FcInitLoadConfigAndFonts() error";
    } else {
        LOGI << "FcInitLoadConfigAndFonts() done";
    }
}

static void useDrawText()
{
    QPixmap pix(32, 32);
    QPainter painter(&pix);
    painter.drawText(0, 0, "0");
}

int entry(int argc, char *argv[])
{
    QApplication app(argc, argv);

    //testCairo();
    //testFreeType();
    testFontConfig();
    return 0;


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
