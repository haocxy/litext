#include <set>

#include <QApplication>
#include <QPixmap>
#include <QPainter>
#include <QImage>

#include "core/system_util.h"
#include "core/font.h"
#include "core/logger.h"
#include "gui/qt/main_window.h"
#include "gui/config.h"


static void useDrawText()
{
    QPixmap pix(32, 32);
    QPainter painter(&pix);
    painter.drawText(0, 0, "0");
}



static void selectFont(font::FontContext &context, font::FontFile &fileTo, font::FontFace &faceTo) {
    static std::set<std::string> GoodFontFamilies{ "Microsoft YaHei", "Noto Sans Mono CJK SC"};
    for (const fs::path &file : SystemUtil::fonts()) {
        font::FontFile fontFile(context, file);
        if (!fontFile) {
            continue;
        }

        for (long i = 0; i < fontFile.faceCount(); ++i) {
            font::FontFace face(fontFile, i);
            if (!face || face.isBold() || face.isItalic() || !face.isScalable()) {
                continue;
            }

            if (GoodFontFamilies.find(face.familyName()) != GoodFontFamilies.end()) {
                fileTo = std::move(fontFile);
                faceTo = std::move(face);
                return;
            }
        }
    }
}

static QString makeTestImgSavePath(const std::string &relative)
{
#ifdef WIN32
    const fs::path base = "D:/tmp/";
#else
    const fs::path base = SystemUtil::userHome() / "tmp";
#endif
    const fs::path full = base / relative;
    fs::create_directories(full.parent_path());
    const std::string s = full.generic_string();
    return QString::fromUtf8(s.c_str());
}

int entry(int argc, char *argv[])
{
    QApplication app(argc, argv);

    font::FontContext fontContext;
    font::FontFile fontFile;
    font::FontFace fontFace;
    selectFont(fontContext, fontFile, fontFace);
    LOGI << "selected fontFile: " << fontFile.path();
    LOGI << "selected fontFace: " << fontFace.familyName();
    fontFace.setPointSize(16);
    LOGI << "setPointSize(16) done";
    const char32_t unicode = 0x7f16; // "编程" 的 "编" 的 unicode 编码
    const int64_t glyphIndex = fontFace.mapUnicodeToGlyphIndex(unicode);
    LOGI << "glyphIndex: " << glyphIndex;
    
    fontFace.loadGlyph(glyphIndex);
    LOGI << "loadGlyph done";

    fontFace.renderGlyph();
    LOGI << "renderGlyph done";

    font::FontFace::BitmapInfo b = fontFace.bitmapInfo();
    QImage glyph(b.buffer, b.width, b.rows, b.pitch, QImage::Format_Indexed8);
    QVector<QRgb> colorTable;
    for (int i = 0; i < 256; ++i) {
        colorTable << qRgba(0, 0, 0, i);
    }
    glyph.setColorTable(colorTable);
    const QString glyphSaveFile = makeTestImgSavePath("glyph.png");
    glyph.save(glyphSaveFile, "png");
    LOGI << "saved: " << glyphSaveFile.toStdString();

    QPixmap img(800, 600);
    QPainter painter(&img);
    painter.fillRect(img.rect(), Qt::white);
    painter.drawImage(QPoint(0, 0), glyph);

    const QString renderedSaveFile = makeTestImgSavePath("rendered.png");
    img.save(renderedSaveFile, "png");
    LOGI << "saved: " << renderedSaveFile.toStdString();

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
