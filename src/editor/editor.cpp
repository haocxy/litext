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
}







