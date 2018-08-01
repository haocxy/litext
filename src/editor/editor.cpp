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
            const DocLine & lastLine = m_model.lineAt(lineCnt - 1);
            const CharN charCnt = lastLine.charCnt();
            assert(charCnt > 0);
            const UChar lastChar = lastLine.charAt(charCnt - 1);
            if (lastChar == '\r' || lastChar == '\n')
            {
                m_lastActLine = m_model.lineCnt();
                m_normalCursor.setAddr(DocAddr::newCharAddrAfterLastPhase());
            }
            else
            {
                m_lastActLine = m_model.lineCnt() - 1;
                m_normalCursor.setAddr(DocAddr::newCharAddrAfterLastChar(lineCnt - 1));
            }
        }
        else
        {
            m_lastActLine = 0;
            m_normalCursor.setAddr(DocAddr::newCharAddrAfterLastPhase());
        }
    }
    else
    {
        m_lastActLine = addr.line();
        m_normalCursor.setAddr(addr);
    }

    m_onLastActLineUpdateListeners.call();
}

void Editor::onDirectionKeyPress(Dir dir)
{
    switch (dir)
    {
    case Dir::Up:

        break;
    case Dir::Down:
    {
        const LineN line = m_normalCursor.addr().line() + 1;
        const CharN col = 0;
        m_normalCursor.setAddr(DocAddr(line, col));
        m_lastActLine = line;
    }
    break;
    case Dir::Left:

        break;
    case Dir::Right:

        break;
    default:
        throw std::logic_error("unknown dir type");
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







