#include "txt_word_instream.h"

#include <cassert>
#include "text/char_instream.h"


namespace
{
    inline bool IsWordChar(char32_t c)
    {
        return c >= 'a' && c <= 'z'
            || c >= 'A' && c <= 'Z'
            || c >= '0' && c <= '9'
            || c == '_';
    }

    inline bool IsEmptyChar(char32_t c)
    {
        return c == ' ' || c == '\t';
    }
}

std::u32string TxtWordStream::Next()
{
    char32_t next = PopNextChar();
    if (next == 0)
    {
        return std::u32string();
    }

    std::u32string result;
    result.push_back(next);

    if (IsWordChar(next))
    {
        while (true)
        {
            char32_t c = PopNextChar();
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
            char32_t c = PopNextChar();
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

char32_t TxtWordStream::PopNextChar()
{
    if (!m_buff.empty())
    {
        char32_t first = m_buff.front();
        m_buff.pop_front();
        return first;
    }
    return m_charInStream.Next();
}

