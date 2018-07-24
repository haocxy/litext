#pragma once

#include "model/model_define.h"

namespace view
{
    class LineAddr
    {
    public:
        static LineAddr newLineAddrAfterLastLine()
        {
            LineAddr lineAddr(0, 0);
            lineAddr.setFlag(kAfterLastLine);
            lineAddr.setFlag(kAfterLastChar);
            return lineAddr;
        }
    public:
        LineAddr() :m_flag(kIsNull), m_phase(0), m_line(0) {}
        LineAddr(int phase, int line) :m_flag(0), m_phase(phase), m_line(line) {}
        int phase() const { return m_phase; }
        void setPhase(int phase) { m_phase = phase; }
        int line() const { return m_line; }
        void setLine(int line) { m_line = line; }
        bool isNull() const { return hasFlag(kIsNull); }
        bool isAfterLastLine() const { return hasFlag(kAfterLastLine); }
    protected:
        typedef uint_least8_t flag_t;
        enum : flag_t
        {
            kIsNull = 1,
            kAfterLastLine = 1 << 1,
            kAfterLastChar = 1 << 2,
        };
        bool hasFlag(flag_t f) const { return (m_flag & f) != 0; }
        void clearFlag(flag_t f) { m_flag &= (~f); }
        void setFlag(flag_t f) { m_flag |= f; }
    private:
        flag_t m_flag = 0;
        int m_phase = 0;
        int m_line = 0;
    };

    class CharAddr : public LineAddr
    {
    public:
        static CharAddr newAfterLastLine()
        {
            CharAddr addr(0, 0, 0);
            addr.setFlag(kAfterLastLine);
            addr.setFlag(kAfterLastChar);
            return addr;
        }
        static CharAddr newAfterLastChar(const LineAddr & lineAddr)
        {
            CharAddr charAddr(lineAddr, 0);
            charAddr.setFlag(kAfterLastChar);
            return charAddr;
        }
    public:
        CharAddr() = default;
        CharAddr(const LineAddr & lineAddr, CharN col) : LineAddr(lineAddr), m_col(col) {}
        CharAddr(int phase, int line, CharN col) :LineAddr(phase, line), m_col(col) {}
        CharN col() const { return m_col; }
        void setCol(CharN col) { m_col = col; }
        bool isAfterLastChar() const { return hasFlag(kAfterLastChar); }
    private:
        CharN m_col = 0;
    };
}
