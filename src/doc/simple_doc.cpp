#include "simple_doc.h"

#include <memory>
#include <iostream>
#include <sstream>
#include <fstream>

#include <QFile>
#include <QTextStream>
#include <QTextCodec>


bool SimpleDoc::LoadFromFile(const std::string &path)
{
    QFile file(QString::fromStdString(path));
    if (!file.open(QIODevice::ReadOnly)) {
        std::cerr << "cannot open file: " << path << ", msg: " << file.errorString().toStdString() << std::endl;
        return false;
    }

    // QTextCodec 对象不需要由用户释放
    const char *encoding = "gbk";
    QTextCodec *codec = QTextCodec::codecForName(encoding);
    if (!codec) {
        std::cerr << "cannot find codec for: " << encoding;
        return false;
    }

    std::unique_ptr<QTextDecoder> decoder(codec->makeDecoder());
    if (!decoder) {
        std::cerr << "cannot make decoder";
        return false;
    }

    QString content = decoder->toUnicode(file.readAll());

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
                SimpleRow row;
                row.setContent(std::move(buff));
                row.setRowEnd(RowEnd::LF);
                m_lines.push_back(std::move(row));
            }
            else
            {
                buff.push_back(c);
            }
            break;
        case ST_CR:
            if (c == '\r')
            {
                SimpleRow row;
                row.setContent(std::move(buff));
                row.setRowEnd(RowEnd::CR);
                m_lines.push_back(std::move(row));
            }
            else if (c == '\n')
            {
                SimpleRow row;
                row.setContent(std::move(buff));
                row.setRowEnd(RowEnd::CRLF);
                m_lines.push_back(std::move(row));

                state = ST_Normal;
            }
            else
            {
                SimpleRow row;
                row.setContent(std::move(buff));
                row.setRowEnd(RowEnd::CR);
                m_lines.push_back(std::move(row));

                state = ST_Normal;
            }
            break;
        default:
            break;
        }
    } // while end

    if (state == ST_CR)
    {
        SimpleRow row;
        row.setContent(std::move(buff));
        row.setRowEnd(RowEnd::CR);
        m_lines.push_back(std::move(row));
    }
    else
    {
        // 把buff中的残留字节解析并添加进去
        if (buff.length() > 0)
        {
            SimpleRow row;
            row.setContent(std::move(buff));
            m_lines.push_back(std::move(row));
        }
    }

    // 如果最后一行以换行结束，则添加一个空行
    if (m_lines.empty())
    {
        SimpleRow row;
        row.setRowEnd(RowEnd::NO);
        m_lines.push_back(std::move(row));
    }
    else
    {
        const SimpleRow &lastRow = m_lines.back();

        if (lastRow.rowEnd() != RowEnd::NO)
        {
            SimpleRow row;
            row.setRowEnd(RowEnd::NO);
            m_lines.push_back(std::move(row));
        }
    }

    return true;
}
