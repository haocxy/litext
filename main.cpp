#include "textpad.h"
#include <QApplication>

#include "docmodel.h"

#include "module/text/impl/qchar_instream_impl_load_once.h"


static const char* s = ""
"\tHello, w\thats your name? My name is HanMeiMei.\n"
"    ab好你好anihaohaha你好吗abcabcypabcabcHanMeiMei.\n"
"我的名字叫韩梅梅";

int main(int argc, char *argv[])
{
    DocModel model;
    model.LoadFromFile("F:\\a.cpp");
    //model.ParseStr(s);

    QApplication a(argc, argv);

    const char *kFontFamilyTimes = "Times";
    const char *kFontFamilyYaHei = "Microsoft YaHei";

    const char *kFontFamily = kFontFamilyTimes;
    const int kFontSz = 12;

    FontConfig c;
    c.SetFont(QFont(kFontFamily, kFontSz));
    c.SetCharMargin(2);
    c.SetTabSize(2);
    c.SetWrapLineForShow(true);

    FontInfoProvider fontInfo(c);


    TextPad w(model, fontInfo);
    w.resize(800, 600);
    w.show();


    

    return a.exec();
}
