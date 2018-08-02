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
        if (lineCnt > 0)
        {
            const Line & lastLine = m_model.lineAt(lineCnt - 1);
            const CharN charCnt = lastLine.charCnt();
            if (charCnt > 0)
            {
                const UChar lastChar = lastLine.charAt(charCnt - 1);
                if (lastChar == '\r' || lastChar == '\n')
                {
                    setNormalCursor(DocAddr::newCharAddrAfterLastPhase());
                }
                else
                {
                    setNormalCursor(DocAddr::newCharAddrAfterLastChar(lineCnt - 1));
                }
            }
            else
            {
                setNormalCursor(DocAddr::newCharAddrAfterLastChar(lineCnt - 1));
            }
        }
        else
        {
            setNormalCursor(DocAddr::newCharAddrAfterLastPhase());
        }
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







