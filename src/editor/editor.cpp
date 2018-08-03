#include "editor.h"

#include <assert.h>
#include "doc/doc.h"
#include "doc/doc_line.h"

Editor::Editor(Doc * model)
    : m_model(*model)
{
    assert(model);
}

void Editor::onPrimaryKeyPress(const DocAddr & addr)
{
    if (addr.isAfterLastPhase())
    {
        const LineN lineCnt = m_model.lineCnt();
        assert(lineCnt > 0);
        setNormalCursor(DocAddr::newCharAddrAfterLastChar(lineCnt - 1));
    }
    else
    {
        setNormalCursor(addr);
    }
}

void Editor::setNormalCursor(const DocCursor & cursor)
{
    m_normalCursor = cursor;
    if (!cursor.to().isNull())
    {
        setLastActLine(cursor.to().line());
    }
}

DocAddr Editor::getNextLeftAddrByChar(const DocAddr & addr) const
{
    if (addr.isNull())
    {
        return DocAddr();
    }

    if (addr.isAfterLastPhase())
    {
        const LineN lineCnt = m_model.lineCnt();
        if (lineCnt <= 0)
        {
            return DocAddr();
        }

        return DocAddr::newCharAddrAfterLastChar(lineCnt - 1);
    }

    if (addr.isAfterLastChar())
    {
        const CharN charCnt = m_model.lineAt(addr.line()).charCnt();
        if (charCnt <= 0)
        {
            if (addr.line() > 0)
            {
                return DocAddr::newCharAddrAfterLastChar(addr.line() - 1);
            }
            return DocAddr();
        }
        return DocAddr(addr.line(), charCnt - 1);
    }

    if (addr.col() <= 0)
    {
        if (addr.line() > 0)
        {
            return DocAddr::newCharAddrAfterLastChar(addr.line() - 1);
        }
        return DocAddr();
    }

    return addr.nextLeft();
}


DocAddr Editor::getNextRightAddrByChar(const DocAddr & addr) const
{
    if (addr.isNull())
    {
        return DocAddr();
    }

    if (addr.isAfterLastPhase())
    {
        return DocAddr();
    }

    if (addr.isAfterLastChar())
    {
        const LineN lineCnt = m_model.lineCnt();
        if (addr.line() < lineCnt - 1)
        {
            const Line & nextLine = m_model.lineAt(addr.line() + 1);
            if (nextLine.charCnt() > 0)
            {
                return DocAddr(addr.line() + 1, 0);
            }
            else
            {
                return DocAddr::newCharAddrAfterLastChar(addr.line() + 1);
            }
        }
        else
        {
            return DocAddr();
        }
    }

    const Line & line = m_model.lineAt(addr.line());
    if (addr.col() < line.charCnt() - 1)
    {
        return addr.nextRight();
    }
    else
    {
        return DocAddr::newCharAddrAfterLastChar(addr.line());
    }
}

ListenerID Editor::addOnLastActLineUpdateListener(std::function<void()>&& action)
{
    return m_onLastActLineUpdateListeners.add(std::move(action));
}

void Editor::removeOnLastActLineUpdateListener(ListenerID id)
{
    m_onLastActLineUpdateListeners.remove(id);
}

void Editor::setLastActLine(LineN line)
{
    const LineN old = m_lastActLine;
    m_lastActLine = line;

    if (old != m_lastActLine)
    {
        m_onLastActLineUpdateListeners.call();
    }
}







