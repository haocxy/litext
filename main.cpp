#include "textpad.h"
#include <QApplication>
#include <QDebug>

#include "docmodel.h"

#include "module/text/impl/ref_content_char_instream.h"
#include "module/text/impl/txt_word_stream.h"
#include "util/fileutil.h"

static const char* s = ""
"\tHello, w\thats your name? My name is HanMeiMei.\n"
"    ab好你好anihaohaha你好吗abcabcypabcabcHanMeiMei.\n"
"我的名字叫韩梅梅";

int main(int argc, char *argv[])
{
    const QString content = FileUtil::ReadFile("F:/a.cpp");
    RefContentQCharInStream charStream(content);
    WordInStream *wordStream = new TxtWordStream(charStream);
    while (true)
    {
        QString word = wordStream->Next();
        if (word.isNull())
        {
            return 0;
        }
        qDebug() << word;
    }
    return 0;

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
