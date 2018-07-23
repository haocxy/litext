#pragma once

#include "module/model/model_define.h"

namespace view
{
    class LineAddr
    {
    public:
        LineAddr() :m_isNull(true), m_phase(0), m_line(0) {}
        LineAddr(int phase, int line) :m_isNull(false), m_phase(phase), m_line(line) {}
        int phase() const { return m_phase; }
        void setPhase(int phase) { m_phase = phase; }
        int line() const { return m_line; }
        void setLine(int line) { m_line = line; }
        bool isNull() const { return m_isNull; }
    private:
        bool m_isNull = true;
        int m_phase = 0;
        int m_line = 0;
    };

    class CharAddr : public LineAddr
    {
    public:
        CharAddr() = default;
        CharAddr(const LineAddr & lineAddr, CharN col) : LineAddr(lineAddr), m_col(col) {}
        CharAddr(int phase, int line, CharN col) :LineAddr(phase, line), m_col(col) {}
        CharN col() const { return m_col; }
        void setCol(CharN col) { m_col = col; }
    private:
        CharN m_col = 0;
    };
}
