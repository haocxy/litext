#pragma once

#include "doc/doc_loc.h"

class DocCursor
{
public:
    enum Flag : uint32_t
    {
        flgNull = 1 << 0, // 有此标志时，这个DocCursor对象为无效对象，类似NULL或者false语义
    };
public:
    DocCursor() : m_flags(flgNull) {}
    explicit DocCursor(const DocLoc & loc) : m_from(loc), m_to(loc) {}

    const DocLoc & loc() const { return m_from; }
    void setLoc(const DocLoc & loc)
    {
        m_from = loc;
        m_to = loc;
    }

    const DocLoc & from() const { return m_from; }
    void setFrom(const DocLoc &from) { m_from = from; }

    const DocLoc & to() const { return m_to; }
    void setTo(const DocLoc &to) { m_to = to; }

    bool hasFlag(uint32_t flag) const
    {
        return ((m_flags & flag) != 0);
    }

    void setFlag(uint32_t flag)
    {
        m_flags |= flag;
    }

    bool isNull() const
    {
        return hasFlag(flgNull);
    }

    bool isInsert() const
    {
        return m_from == m_to;
    }

private:
    uint32_t m_flags = 0;
    DocLoc m_from;
    DocLoc m_to;
};
