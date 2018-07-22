//#include "textpad.h"
#include <QApplication>
#include <QDebug>

//#include "docmodel.h"
//
//#include "module/text/impl/ref_content_char_instream.h"
//#include "module/text/impl/txt_word_stream.h"
//#include "util/fileutil.h"

#include "textpad2.h"
#include "module/model/impl/simple_model.h"
#include "module/view/view_config.h"
#include "module/view/view.h"

const char *kFontFamilyTimes = "Times";
const char *kFontFamilyYaHei = "Microsoft YaHei";

static void setupConfig(view::Config &c)
{
    c.setLineHeightFactor(1.5f);
    c.setHGap(2);
    c.setHMargin(2);
    c.setTabSize(4);
    c.setWrapLine(false);

    view::Font &f = c.rfont();
    view::FontInfo fi;
    fi.family = kFontFamilyYaHei;
    fi.size = 18;
    fi.bold = true;

    f.setFont(fi);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SimpleModel model;
    model.LoadFromFile(R"(F:\a.txt)");

    view::Config config;
    setupConfig(config);

    View v(&model, &config);

    TextPad2 tp2(&v);
    tp2.show();

    return app.exec();
}
