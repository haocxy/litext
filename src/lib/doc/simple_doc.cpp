#include "simple_doc.h"

#include <memory>
#include <iostream>
#include <sstream>
#include <fstream>

#include <QFile>
#include <QTextStream>
#include <QTextCodec>

#include "core/uchar.h"
#include "core/charset.h"


static std::u32string toUString(const QString &qstr)
{
    const int u16count = qstr.length();

    std::u32string result;
    result.reserve(u16count);

    char16_t high = 0;
    bool highFound = false;

    for (int i = 0; i < u16count; ++i) {
        const char16_t u16ch = qstr[i].unicode();
        if (!highFound) {
            if (!UCharUtil::isSurrogate(u16ch)) {
                result.push_back(u16ch);
            } else if (UCharUtil::isHighSurrogate(u16ch)) {
                high = u16ch;
                highFound = true;
            } else { // if (UCharUtil::isLowSurrogate(u16ch))
                // do nothing
            }
        } else { // if (highFound)
            if (UCharUtil::isLowSurrogate(u16ch)) {
                result.push_back(UCharUtil::u16SurrogatePairToUnicode(high, u16ch));
            } else if (!UCharUtil::isSurrogate(u16ch)) {
                result.push_back(u16ch);
            } else {
                // do nothing
            }
            high = 0;
            highFound = false;
        }
    }

    return result;
}

bool SimpleDoc::LoadFromFile(const std::string &path)
{
    bool useSimpleDoc = false;
    if (!useSimpleDoc) {
        m_lines.push_back(Row(RowEnd::NO));
        return true;
    }

    QFile file(QString::fromStdString(path));
    if (!file.open(QIODevice::ReadOnly)) {
        std::cerr << "cannot open file: " << path << ", msg: " << file.errorString().toStdString() << std::endl;
        return false;
    }

    const QByteArray ba = file.readAll();

    CharsetDetector charsetDetector;
    charsetDetector.feed(ba.constData(), ba.size());
    charsetDetector.end();

    // QTextCodec 对象不需要由用户释放
    const std::string charset = charsetDetector.charset();
    QTextCodec *codec = QTextCodec::codecForName(charset.c_str());
    if (!codec) {
        std::cerr << "cannot find codec for: " << charset;
        return false;
    }

    uptr<QTextDecoder> decoder(codec->makeDecoder());
    if (!decoder) {
        std::cerr << "cannot make decoder";
        return false;
    }

    QString content = decoder->toUnicode(ba);

    enum State { ST_Normal, ST_CR };

    int state = ST_Normal;

    QString buff;

    const int len = content.length();

    for (int i = 0; i < len; ++i)
    {
        QChar c = content.at(i);
        switch (state)
        {
        case ST_Normal:
            if (c == '\r')
            {
                state = ST_CR;
            }
            else if (c == '\n')
            {
                m_lines.push_back(Row(toUString(buff), RowEnd::LF));
                buff.clear();
            }
            else
            {
                buff.push_back(c);
            }
            break;
        case ST_CR:
            if (c == '\r')
            {
                m_lines.push_back(Row(toUString(buff), RowEnd::CR));
                buff.clear();
            }
            else if (c == '\n')
            {
                m_lines.push_back(Row(toUString(buff), RowEnd::CRLF));
                buff.clear();
                state = ST_Normal;
            }
            else
            {
                m_lines.push_back(Row(toUString(buff), RowEnd::CR));
                buff.clear();
                state = ST_Normal;
            }
            break;
        default:
            break;
        }
    } // while end

    if (state == ST_CR)
    {
        m_lines.push_back(Row(toUString(buff), RowEnd::CR));
        buff.clear();
    }
    else
    {
        // 把buff中的残留字节解析并添加进去
        if (buff.length() > 0)
        {
            m_lines.push_back(Row(toUString(buff), RowEnd::NO));
        }
    }

    // 如果最后一行以换行结束，则添加一个空行
    if (m_lines.empty())
    {
        m_lines.push_back(Row(RowEnd::NO));
    }
    else
    {
        const Row &lastRow = m_lines.back();

        if (lastRow.rowEnd() != RowEnd::NO)
        {
            m_lines.push_back(Row(RowEnd::NO));
        }
    }

    return true;
}
