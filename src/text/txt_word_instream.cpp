#include "txt_word_instream.h"

#include <cassert>
#include "text/char_instream.h"


namespace
{
    inline bool IsWordChar(QChar c)
    {
        return c >= 'a' && c <= 'z'
            || c >= 'A' && c <= 'Z'
            || c >= '0' && c <= '9'
            || c == '_';
    }

    inline bool IsEmptyChar(QChar c)
    {
        return c == ' ' || c == '\t';
    }
}

QString TxtWordStream::Next()
{
    QChar next = PopNextChar();
    if (next == 0)
    {
        return QString();
    }

    QString result;
    result.push_back(next);

    if (IsWordChar(next))
    {
        while (true)
        {
            QChar c = PopNextChar();
            if (c == 0)
            {
                return result;
            }
            else if (IsWordChar(c))
            {
                result.push_back(c);
            }
            else
            {
                PushBackChar(c);
                break;
            }
        }
    }
    else if (IsEmptyChar(next))
    {
        while (true)
        {
            QChar c = PopNextChar();
            if (c == 0)
            {
                return result;
            }
            else if (IsEmptyChar(c))
            {
                result.push_back(c);
            }
            else
            {
                PushBackChar(c);
                break;
            }
        }
    }

    return result;
}

QChar TxtWordStream::PopNextChar()
{
    if (!m_buff.empty())
    {
        QChar first = m_buff.front();
        m_buff.pop_front();
        return first;
    }
    return m_charInStream.Next();
}

