#include "quick_font_chooser.h"

#include <vector>

#include "core/font.h"
#include "core/logger.h"
#include "core/time.h"


// 不同操作系统的代码只有使用的字体文件等差异,没有和编译相关的差异
// 我希望所有平台的代码都被编译
static bool isWindows()
{
#ifdef WIN32
    return true;
#else
    return false;
#endif
}

namespace font
{

QuickFountChooser::QuickFountChooser()
{
}

QuickFountChooser::~QuickFountChooser()
{
}

opt<FontIndex> QuickFountChooser::choose()
{
    if (isWindows()) {
        const opt<FontIndex> fontIndex = chooseForWindows();
        if (fontIndex) {
            return fontIndex;
        }
    }

    {
        const opt<FontIndex> fontIndex = chooseByScanAllFoundFont();
        if (fontIndex) {
            return fontIndex;
        }
    }

    return std::nullopt;
}

opt<FontIndex> QuickFountChooser::chooseForWindows()
{
    // 这段代码极少执行,通常只在部署后首次启动时执行一次,所以不需要做为static局部变量
    const std::vector<FontIndex> goodChoices{
        {"C:/Windows/Fonts/msyh.ttc", 0}, // 微软雅黑
        {"C:/Windows/Fonts/simsun.ttc", 0}, // 宋体
    };

    for (const FontIndex &fontIndex : goodChoices) {
        FontFile fontFile(context_, fontIndex.file());
        if (!fontFile) {
            continue;
        }
        FontFace fontFace(fontFile, fontIndex.faceIndex());
        if (!fontFace) {
            continue;
        }
        return fontIndex;
    }

    return std::nullopt;
}

opt<FontIndex> QuickFountChooser::chooseByScanAllFoundFont() {

    const std::vector<std::string> GoodFontFamilies{
        "Noto Sans SC",
        "Noto Sans Mono CJK SC",
        "Microsoft YaHei"
    };

    ElapsedTime elapsed;

    elapsed.start();

    const std::vector<fs::path> fontFiles = SystemUtil::fonts();

    auto bestIt = GoodFontFamilies.end();

    fs::path bestPath;
    long bestFace = -1;

    for (const fs::path &file : fontFiles) {
        font::FontFile fontFile(context_, file);
        if (!fontFile) {
            continue;
        }

        for (long i = 0; i < fontFile.faceCount(); ++i) {
            font::FontFace face(fontFile, i);
            if (!face || face.isBold() || face.isItalic() || !face.isScalable()) {
                continue;
            }

            auto it = std::find(GoodFontFamilies.begin(), GoodFontFamilies.end(), face.familyName());
            if ((it != GoodFontFamilies.end())) {
                if (bestIt == GoodFontFamilies.end() || it < bestIt) {
                    bestIt = it;
                    bestPath = file;
                    bestFace = i;
                }
            }
        }
    }

    if (bestIt != GoodFontFamilies.end()) {
        LOGI << "chooseByScanAllFoundFont time usage: [" << elapsed.ms() << " ms]";
        return FontIndex(bestPath, bestFace);
    }

    for (const fs::path &file : fontFiles) {
        font::FontFile fontFile(context_, file);
        if (fontFile) {
            for (long i = 0; i < fontFile.faceCount(); ++i) {
                font::FontFace face(fontFile, i);
                if (face) {
                    LOGI << "chooseByScanAllFoundFont done without bad font, time usage: [" << elapsed.ms() << " ms]";
                    return FontIndex(file, face.faceIndex());
                }
            }
        }
    }

    return std::nullopt;
}

}
