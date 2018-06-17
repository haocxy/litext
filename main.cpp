#include "textpad.h"
#include <QApplication>

#include "docmodel.h"

static const char* s = ""
"\tHello, w\thats your name? My name is HanMeiMei.\n"
"    ab好你好anihaohaha你好吗abcabcypabcabcHanMeiMei.\n"
"我的名字叫韩梅梅";

int main(int argc, char *argv[])
{
    DocModel model;
    //model.LoadFromFile("F:\\a.cpp");
    model.ParseStr(s);

    QApplication a(argc, argv);

    const char *kFontFamilyTimes = "Times";
    const char *kFontFamilyYaHei = "Microsoft YaHei";

    const char *kFontFamily = kFontFamilyTimes;
    const int kFontSz = 30;

    FontConfig c;
    c.SetFont(QFont(kFontFamily, kFontSz));
    c.SetCharMargin(2);
    c.SetTabSize(2);

    FontInfo fontInfo(c);


    TextPad w(model, fontInfo);
    w.show();


    w.resize(1200,600);

    return a.exec();
}
