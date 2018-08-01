#pragma once

#include "doc/doc_addr.h"

class DocCursor
{
public:
    enum Flag : uint32_t
    {
        flgNull = 1 << 0, // 有此标志时，这个DocCursor对象为无效对象，类似NULL或者false语义
    };
public:
    DocCursor() : m_flags(flgNull) {}
    DocCursor(const DocAddr & addr) : m_from(addr), m_to(addr) {}

    const DocAddr & addr() const { return m_from; }
    void setAddr(const DocAddr &addr)
    {
        m_from = addr;
        m_to = addr;
    }

    const DocAddr & from() const { return m_from; }
    void setFrom(const DocAddr &from) { m_from = from; }

    const DocAddr & to() const { return m_to; }
    void setTo(const DocAddr &to) { m_to = to; }

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
    DocAddr m_from;
    DocAddr m_to;
};
