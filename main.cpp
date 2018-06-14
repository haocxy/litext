#include "textpad.h"
#include <QApplication>

#include "DocumentModel.h"

static const char* s = "";
//"Hello, whats your name? My name is HanMeiMei.\n"
//"你好你好nihaohaha你好吗abcabcypabcabcHanMeiMei.\n";

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
