#include "editor.h"

#include <assert.h>

#include "doc/doc.h"
#include "doc/doc_row.h"


Editor::Editor(Doc *model, StrandPool &pool, const fs::path &file, Strand &ownerThread)
    : m_model(*model)
    , document_(pool, file, ownerThread)
{
    assert(model);
}

void Editor::onPrimaryKeyPress(const DocLoc & loc)
{
    if (loc.isAfterLastRow())
    {
        const RowN rowCnt = m_model.rowCnt();
        assert(rowCnt > 0);
        setNormalCursor(DocCursor(DocLoc::newDocLocAfterLastChar(rowCnt - 1)));
    }
    else
    {
        setNormalCursor(DocCursor(loc));
    }
}

void Editor::setNormalCursor(const DocLoc & loc)
{
    setNormalCursor(DocCursor(loc));
}

void Editor::setNormalCursor(const DocCursor & cursor)
{
    m_normalCursor = cursor;
    if (!cursor.to().isNull())
    {
        setLastActRow(cursor.to().row());
    }
}

DocLoc Editor::getNextLeftLocByChar(const DocLoc & loc) const
{
    if (loc.isNull())
    {
        return DocLoc();
    }

    if (loc.isAfterLastRow())
    {
        const RowN rowCnt = m_model.rowCnt();
        if (rowCnt <= 0)
        {
            return DocLoc();
        }

        return DocLoc::newDocLocAfterLastChar(rowCnt - 1);
    }

    if (loc.isAfterLastChar())
    {
        const CharN charCnt = m_model.rowAt(loc.row()).charCnt();
        if (charCnt <= 0)
        {
            if (loc.row() > 0)
            {
                return DocLoc::newDocLocAfterLastChar(loc.row() - 1);
            }
            return DocLoc();
        }
        return DocLoc(loc.row(), charCnt - 1);
    }

    if (loc.col() <= 0)
    {
        if (loc.row() > 0)
        {
            return DocLoc::newDocLocAfterLastChar(loc.row() - 1);
        }
        return DocLoc();
    }

    return loc.nextLeft();
}


DocLoc Editor::getNextRightLocByChar(const DocLoc & loc) const
{
    if (loc.isNull())
    {
        return DocLoc();
    }

    if (loc.isAfterLastRow())
    {
        return DocLoc();
    }

    if (loc.isAfterLastChar())
    {
        const RowN rowCnt = m_model.rowCnt();
        if (loc.row() < rowCnt - 1)
        {
            const Row & nextRow = m_model.rowAt(loc.row() + 1);
            if (nextRow.charCnt() > 0)
            {
                return DocLoc(loc.row() + 1, 0);
            }
            else
            {
                return DocLoc::newDocLocAfterLastChar(loc.row() + 1);
            }
        }
        else
        {
            return DocLoc();
        }
    }

    const Row & row = m_model.rowAt(loc.row());
    if (loc.col() < row.charCnt() - 1)
    {
        return loc.nextRight();
    }
    else
    {
        return DocLoc::newDocLocAfterLastChar(loc.row());
    }
}

SigConn Editor::onLastActRowUpdated(std::function<void()>&& action)
{
    return onLastActRowUpdated_.connect(std::move(action));
}

void Editor::setLastActRow(RowN row)
{
    const RowN old = m_lastActRow;
    m_lastActRow = row;

    if (old != m_lastActRow)
    {
        onLastActRowUpdated_();
    }
}







