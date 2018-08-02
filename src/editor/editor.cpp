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







