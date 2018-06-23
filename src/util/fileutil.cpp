#include "fileutil.h"

#include <QFile>
#include <QTextCodec>
#include <QTextDecoder>

#include "delete_helper.h"

namespace FileUtil
{
    QString ReadFile(const QString & file, const char * encoding)
    {
        QFile f(file);
        if (!f.open(QIODevice::ReadOnly))
        {
            throw Err("cannot open file '%1' for read because '%2'"), file, f.errorString();
        }

        QTextCodec *codec = QTextCodec::codecForName(encoding);
        if (!codec)
        {
            throw Err("unsupported encoding '%1'"), encoding;
        }
        QTextDecoder *decoder = codec->makeDecoder();
        if (!decoder)
        {
            throw Err("cannot create decoder for '%1'"), encoding;
        }
        PtrDeleter<QTextDecoder> deldecoder(decoder);

        f.unsetError();
        QByteArray bytes = f.readAll();
        if (f.error() != QFileDevice::NoError)
        {
            throw Err("cannot read content of file '%1' because '%2'"), file, f.errorString();
        }

        return decoder->toUnicode(bytes);
    }

}
