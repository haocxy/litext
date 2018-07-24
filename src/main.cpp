#include <QApplication>
#include <QDebug>

#include "widget/textpad.h"
#include "model/simple_model.h"
#include "view/view_config.h"
#include "view/view.h"
#include "control/doc_controller.h"

const char *kFontFamilyTimes = "Times";
const char *kFontFamilyYaHei = "Microsoft YaHei";

static void setupConfig(view::Config &c)
{
    c.setLineHeightFactor(1.5f);
    c.setHGap(2);
    c.setHMargin(0);
    c.setTabSize(4);
    c.setWrapLine(true);

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

    DocController c(&model);
    c.normalCursor().setAddr(DocAddr(1, 2));

    TextPad tp2(&v, &c);
    tp2.show();

    return app.exec();
}
