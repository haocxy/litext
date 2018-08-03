#include "editor.h"

#include <assert.h>
#include "doc/doc.h"
#include "doc/doc_row.h"

Editor::Editor(Doc * model)
    : m_model(*model)
{
    assert(model);
}

void Editor::onPrimaryKeyPress(const DocAddr & addr)
{
    if (addr.isAfterLastRow())
    {
        const RowN rowCnt = m_model.rowCnt();
        assert(rowCnt > 0);
        setNormalCursor(DocAddr::newDocAddrAfterLastChar(rowCnt - 1));
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
        setLastActRow(cursor.to().row());
    }
}

DocAddr Editor::getNextLeftAddrByChar(const DocAddr & addr) const
{
    if (addr.isNull())
    {
        return DocAddr();
    }

    if (addr.isAfterLastRow())
    {
        const RowN rowCnt = m_model.rowCnt();
        if (rowCnt <= 0)
        {
            return DocAddr();
        }

        return DocAddr::newDocAddrAfterLastChar(rowCnt - 1);
    }

    if (addr.isAfterLastChar())
    {
        const CharN charCnt = m_model.rowAt(addr.row()).charCnt();
        if (charCnt <= 0)
        {
            if (addr.row() > 0)
            {
                return DocAddr::newDocAddrAfterLastChar(addr.row() - 1);
            }
            return DocAddr();
        }
        return DocAddr(addr.row(), charCnt - 1);
    }

    if (addr.col() <= 0)
    {
        if (addr.row() > 0)
        {
            return DocAddr::newDocAddrAfterLastChar(addr.row() - 1);
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

    if (addr.isAfterLastRow())
    {
        return DocAddr();
    }

    if (addr.isAfterLastChar())
    {
        const RowN rowCnt = m_model.rowCnt();
        if (addr.row() < rowCnt - 1)
        {
            const Row & nextRow = m_model.rowAt(addr.row() + 1);
            if (nextRow.charCnt() > 0)
            {
                return DocAddr(addr.row() + 1, 0);
            }
            else
            {
                return DocAddr::newDocAddrAfterLastChar(addr.row() + 1);
            }
        }
        else
        {
            return DocAddr();
        }
    }

    const Row & row = m_model.rowAt(addr.row());
    if (addr.col() < row.charCnt() - 1)
    {
        return addr.nextRight();
    }
    else
    {
        return DocAddr::newDocAddrAfterLastChar(addr.row());
    }
}

ListenerID Editor::addOnLastActRowUpdateListener(std::function<void()>&& action)
{
    return m_lastActRowUpdateListeners.add(std::move(action));
}

void Editor::removeOnLastActRowUpdateListener(ListenerID id)
{
    m_lastActRowUpdateListeners.remove(id);
}

void Editor::setLastActRow(RowN row)
{
    const RowN old = m_lastActRow;
    m_lastActRow = row;

    if (old != m_lastActRow)
    {
        m_lastActRowUpdateListeners.call();
    }
}







