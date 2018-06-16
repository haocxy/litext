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
    model.ParseStr(s);

    QApplication a(argc, argv);
    TextPad w(model);
    w.show();


    w.resize(1200,600);

    return a.exec();
}
