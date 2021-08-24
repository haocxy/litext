#include <set>
#include <iostream>

#include <QApplication>
#include <QPainter>

#include "core/system.h"
#include "core/font.h"
#include "core/logger.h"
#include "core/time.h"
#include "gui/qt/main_window.h"
#include "gui/config.h"
#include "gui/text_area_config.h"
#include "doc/dbfiles.h"

#include "cmdopt.h"


static void useDrawText()
{
    QPixmap pix(32, 32);
    QPainter painter(&pix);
    painter.drawText(0, 0, "0");
}

static void selectFont(font::FontContext &context, font::FontFile &fileTo, font::FontFace &faceTo) {
    static std::set<std::string> GoodFontFamilies{ "Microsoft YaHei", "Noto Sans Mono CJK SC"};
    ElapsedTime elapsed;

    const std::vector<fs::path> fontFiles = SystemUtil::fonts();

    for (const fs::path &file : fontFiles) {
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
                LOGI << "selectFont time usage: [" << elapsed.milliSec() << " ms]";
                return;
            }
        }
    }

    for (const fs::path &file : fontFiles) {
        font::FontFile fontFile(context, file);
        if (fontFile) {
            for (long i = 0; i < fontFile.faceCount(); ++i) {
                font::FontFace face(fontFile, i);
                if (face) {
                    fileTo = std::move(fontFile);
                    faceTo = std::move(face);
                    LOGI << "selectFont done without bad font, time usage: [" << elapsed.milliSec() << " ms]";
                    return;
                }
            }
        }
    }
}

static font::FontIndex selectFont()
{
    font::FontContext context;
    font::FontFile fontFile;
    font::FontFace fontFace;
    selectFont(context, fontFile, fontFace);
    if (fontFile && fontFace) {
        return font::FontIndex(fontFile.path(), fontFace.faceIndex());
    } else {
        return font::FontIndex();
    }
}

int entry(int argc, char *argv[])
{
    try {

        doc::dbfiles::removeUselessDbFiles();

        CmdOpt cmdOpt(argc, argv);
        if (cmdOpt.needHelp()) {
            cmdOpt.help(std::cout);
            return 0;
        }

        const std::vector<doc::OpenInfo> &files = cmdOpt.files();
        for (const doc::OpenInfo &e : files) {
            LOGI << "open info: file [" << e.file() << "], row [" << e.row() << "]";
        }

        QApplication app(argc, argv);

        // 在 Windows 平台发现窗口首次打开时会有一段时间全部为白色，
        // 调查后发现是卡在了 QPainter::drawText(...) 的首次有效调用，
        // 虽然没有看 Qt 的内部实现，但猜测是由于某种延迟加载的机制导致的，
        // 所以解决办法就是在窗口显示前提前使用这个函数（注意，绘制的文本不能为空字符串）
        useDrawText();

        logger::control::Option logOpt;
        logOpt.setLevel("all");
        logOpt.setDir("./tmp/log");
        logOpt.setBasename("notesharplog");
        logOpt.setAlwaysFlush(true);
        logger::control::init(logOpt);

        gui::Config config;
        const font::FontIndex fontIndex = selectFont();
        if (!fontIndex) {
            LOGE << "select font failed";
            return 1;
        }

        config.textAreaConfig().setFontIndex(fontIndex);

        gui::qt::MainWindow mainWindow(config);
        if (argc > 1) {
            mainWindow.openDocument(argv[1]);
        }
        mainWindow.show();

        return app.exec();
    }
    catch (const std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return 1;
    }
}
