#pragma once

#include "doc/doc_define.h"

namespace view
{
    class PhaseAddr
    {
    public:
        static PhaseAddr newPhaseAddrAfterLastPhase()
        {
            PhaseAddr phaseAddr(0);
            phaseAddr.setFlag(kAfterLastPhase);
            phaseAddr.setFlag(kAfterLastLine);
            phaseAddr.setFlag(kAfterLastChar);
            return phaseAddr;
        }
    public:
        PhaseAddr() :m_flag(kIsNull), m_phase(0) {}
        PhaseAddr(int phase) : m_phase(phase) {}
        int phase() const { return m_phase; }
        void setPhase(int phase) { m_phase = phase; }
        bool isNull() const { return hasFlag(kIsNull); }
        bool isAfterLastPhase() const { return hasFlag(kAfterLastPhase); }
    protected:
        typedef uint_least8_t flag_t;
        enum : flag_t
        {
            kIsNull = 1,
            kAfterLastPhase = 1 << 1,
            kAfterLastLine = 1 << 2,
            kAfterLastChar = 1 << 3,
        };
        bool hasFlag(flag_t f) const { return (m_flag & f) != 0; }
        void clearFlag(flag_t f) { m_flag &= (~f); }
        void setFlag(flag_t f) { m_flag |= f; }
    private:
        flag_t m_flag = 0;
        int m_phase = 0;
    };

    class LineAddr : public PhaseAddr
    {
    public:
        static LineAddr newLineAddrAfterLastPhase()
        {
            LineAddr lineAddr(0, 0);
            lineAddr.setFlag(kAfterLastPhase);
            lineAddr.setFlag(kAfterLastLine);
            lineAddr.setFlag(kAfterLastChar);
            return lineAddr;
        }
    public:
        LineAddr() = default;
        LineAddr(int phase, int line) :PhaseAddr(phase), m_line(line) {}
        int line() const { return m_line; }
        void setLine(int line) { m_line = line; }
    private:
        int m_line = 0;
    };

    class CharAddr : public LineAddr
    {
    public:
        static CharAddr newCharAddrAfterLastPhase()
        {
            CharAddr addr(0, 0, 0);
            addr.setFlag(kAfterLastPhase);
            addr.setFlag(kAfterLastLine);
            addr.setFlag(kAfterLastChar);
            return addr;
        }
        static CharAddr newCharAddrAfterLastChar(const LineAddr & lineAddr)
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
